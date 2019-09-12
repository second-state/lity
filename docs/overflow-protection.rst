Overflow Protection
===================

.. _overflow-protection:

Introduction
------------

In order to prevent crypto token leak incident like `BeautyChain(BEC) <https://medium.com/cybermiles/27c96a7e78fd>`_,
Lity provides two approaches: improving compiler or Ethereum virtual machine(EVM).
The first approach converts every integer operation to SafeMath operations at compile time.
The Second approach modifies EVM so that any overflow is detected at runtime.

Lity Compiler
`````````````

Lity automatically converts integer operations to SafeMath operations,
so any overflow would cause a contract execution fail.
Compiler options could be set manually to enable/disable this feature.

Lity's Ethereum Virtual machine
```````````````````````````````

While executes a smart contract, Lity's EVM analyzes each integer operation.
When an overflow occurs, it will terminate the contract execution and report an error.

Currently, three arithmetic operations are overflow-checked: ADD, SUB and MUL.
And overflow checking only applies for 32-byte integers.


Note that this approach only applies for Lity's version of compiler and EVM.

Examples
--------

This example shows how Lity and CyberMiles Virtual Machine helps users to avoid potential security issues of number overflow.

Run a CyberMiles Local Node
```````````````````````````

Follow the instructions here to have a `running CyberMiles local node to deploy a contract <https://www.litylang.org/getting_started/>`_.

Compile BEC Contract
````````````````````

There is an overflow issue at this statement :code:`uint256 amount = uint256(cnt) * _value;` of the function :code:`batchTransfer` .

.. code-block:: Lity

  pragma lity ^1.2.4;

  /**
  * @title SafeMath
  * @dev Math operations with safety checks that throw on error
  */
  library SafeMath {
    function mul(uint256 a, uint256 b) internal constant returns (uint256) {
      uint256 c = a * b;
      assert(a == 0 || c / a == b);
      return c;
    }

    function div(uint256 a, uint256 b) internal constant returns (uint256) {
      // assert(b > 0); // Solidity automatically throws when dividing by 0
      uint256 c = a / b;
      // assert(a == b * c + a % b); // There is no case in which this doesn't hold
      return c;
    }

    function sub(uint256 a, uint256 b) internal constant returns (uint256) {
      assert(b <= a);
      return a - b;
    }

    function add(uint256 a, uint256 b) internal constant returns (uint256) {
      uint256 c = a + b;
      assert(c >= a);
      return c;
    }
  }

  /**
  * @title ERC20Basic
  * @dev Simpler version of ERC20 interface
  * @dev see https://github.com/ethereum/EIPs/issues/179
  */
  contract ERC20Basic {
    uint256 public totalSupply;
    function balanceOf(address who) public constant returns (uint256);
    function transfer(address to, uint256 value) public returns (bool);
    event Transfer(address indexed from, address indexed to, uint256 value);
  }

  /**
  * @title Basic token
  * @dev Basic version of StandardToken, with no allowances.
  */
  contract BasicToken is ERC20Basic {
    using SafeMath for uint256;

    mapping(address => uint256) balances;

    /**
    * @dev transfer token for a specified address
    * @param _to The address to transfer to.
    * @param _value The amount to be transferred.
    */
    function transfer(address _to, uint256 _value) public returns (bool) {
      require(_to != address(0));
      require(_value > 0 && _value <= balances[msg.sender]);

      // SafeMath.sub will throw if there is not enough balance.
      balances[msg.sender] = balances[msg.sender].sub(_value);
      balances[_to] = balances[_to].add(_value);
      Transfer(msg.sender, _to, _value);
      return true;
    }

    /**
    * @dev Gets the balance of the specified address.
    * @param _owner The address to query the the balance of.
    * @return An uint256 representing the amount owned by the passed address.
    */
    function balanceOf(address _owner) public constant returns (uint256 balance) {
      return balances[_owner];
    }
  }

  /**
  * @title ERC20 interface
  * @dev see https://github.com/ethereum/EIPs/issues/20
  */
  contract ERC20 is ERC20Basic {
    function allowance(address owner, address spender) public constant returns (uint256);
    function transferFrom(address from, address to, uint256 value) public returns (bool);
    function approve(address spender, uint256 value) public returns (bool);
    event Approval(address indexed owner, address indexed spender, uint256 value);
  }

  /**
  * @title Standard ERC20 token
  *
  * @dev Implementation of the basic standard token.
  * @dev https://github.com/ethereum/EIPs/issues/20
  * @dev Based on code by FirstBlood: https://github.com/Firstbloodio/token/blob/master/smart_contract/FirstBloodToken.sol
  */
  contract StandardToken is ERC20, BasicToken {

    mapping (address => mapping (address => uint256)) internal allowed;

    /**
    * @dev Transfer tokens from one address to another
    * @param _from address The address which you want to send tokens from
    * @param _to address The address which you want to transfer to
    * @param _value uint256 the amount of tokens to be transferred
    */
    function transferFrom(address _from, address _to, uint256 _value) public returns (bool) {
      require(_to != address(0));
      require(_value > 0 && _value <= balances[_from]);
      require(_value <= allowed[_from][msg.sender]);

      balances[_from] = balances[_from].sub(_value);
      balances[_to] = balances[_to].add(_value);
      allowed[_from][msg.sender] = allowed[_from][msg.sender].sub(_value);
      Transfer(_from, _to, _value);
      return true;
    }

    /**
    * @dev Approve the passed address to spend the specified amount of tokens on behalf of msg.sender.
    *
    * Beware that changing an allowance with this method brings the risk that someone may use both the old
    * and the new allowance by unfortunate transaction ordering. One possible solution to mitigate this
    * race condition is to first reduce the spender's allowance to 0 and set the desired value afterwards:
    * https://github.com/ethereum/EIPs/issues/20#issuecomment-263524729
    * @param _spender The address which will spend the funds.
    * @param _value The amount of tokens to be spent.
    */
    function approve(address _spender, uint256 _value) public returns (bool) {
      allowed[msg.sender][_spender] = _value;
      Approval(msg.sender, _spender, _value);
      return true;
    }

    /**
    * @dev Function to check the amount of tokens that an owner allowed to a spender.
    * @param _owner address The address which owns the funds.
    * @param _spender address The address which will spend the funds.
    * @return A uint256 specifying the amount of tokens still available for the spender.
    */
    function allowance(address _owner, address _spender) public constant returns (uint256 remaining) {
      return allowed[_owner][_spender];
    }
  }

  /**
  * @title Ownable
  * @dev The Ownable contract has an owner address, and provides basic authorization control
  * functions, this simplifies the implementation of "user permissions".
  */
  contract Ownable {
    address public owner;
    event OwnershipTransferred(address indexed previousOwner, address indexed newOwner);

    /**
    * @dev The Ownable constructor sets the original `owner` of the contract to the sender
    * account.
    */
    function Ownable() {
      owner = msg.sender;
    }

    /**
    * @dev Throws if called by any account other than the owner.
    */
    modifier onlyOwner() {
      require(msg.sender == owner);
      _;
    }

    /**
    * @dev Allows the current owner to transfer control of the contract to a newOwner.
    * @param newOwner The address to transfer ownership to.
    */
    function transferOwnership(address newOwner) onlyOwner public {
      require(newOwner != address(0));
      OwnershipTransferred(owner, newOwner);
      owner = newOwner;
    }
  }

  /**
  * @title Pausable
  * @dev Base contract which allows children to implement an emergency stop mechanism.
  */
  contract Pausable is Ownable {
    event Pause();
    event Unpause();

    bool public paused = false;

    /**
    * @dev Modifier to make a function callable only when the contract is not paused.
    */
    modifier whenNotPaused() {
      require(!paused);
      _;
    }

    /**
    * @dev Modifier to make a function callable only when the contract is paused.
    */
    modifier whenPaused() {
      require(paused);
      _;
    }

    /**
    * @dev called by the owner to pause, triggers stopped state
    */
    function pause() onlyOwner whenNotPaused public {
      paused = true;
      Pause();
    }

    /**
    * @dev called by the owner to unpause, returns to normal state
    */
    function unpause() onlyOwner whenPaused public {
      paused = false;
      Unpause();
    }
  }

  /**
  * @title Pausable token
  *
  * @dev StandardToken modified with pausable transfers.
  **/

  contract PausableToken is StandardToken, Pausable {
    function transfer(address _to, uint256 _value) public whenNotPaused returns (bool) {
      return super.transfer(_to, _value);
    }
    function transferFrom(address _from, address _to, uint256 _value) public whenNotPaused returns (bool) {
      return super.transferFrom(_from, _to, _value);
    }
    function approve(address _spender, uint256 _value) public whenNotPaused returns (bool) {
      return super.approve(_spender, _value);
    }
    function batchTransfer(address[] _receivers, uint256 _value) public whenNotPaused returns (bool) {
      uint cnt = _receivers.length;
      uint256 amount = uint256(cnt) * _value;
      require(cnt > 0 && cnt <= 20);
      require(_value > 0 && balances[msg.sender] >= amount);

      balances[msg.sender] = balances[msg.sender].sub(amount);
      for (uint i = 0; i < cnt; i++) {
        balances[_receivers[i]] = balances[_receivers[i]].add(_value);
        Transfer(msg.sender, _receivers[i], _value);
      }
      return true;
    }
  }

  /**
  * @title Bec Token
  *
  * @dev Implementation of Bec Token based on the basic standard token.
  */
  contract BecToken is PausableToken {
    /**
    * Public variables of the token
    * The following variables are OPTIONAL vanities. One does not have to include them.
    * They allow one to customise the token contract & in no way influences the core functionality.
    * Some wallets/interfaces might not even bother to look at this information.
    */
    string public name = "BeautyChain";
    string public symbol = "BEC";
    string public version = '1.0.0';
    uint8 public decimals = 18;

    /**
    * @dev Function to check the amount of tokens that an owner allowed to a spender.
    */
    function BecToken() {
      totalSupply = 7000000000 * (10**(uint256(decimals)));
      balances[msg.sender] = totalSupply;    // Give the creator all initial tokens
    }

    function () {
      //if ether is sent to this address, send it back.
      revert();
    }
  }

and we could compile it using :code:`lityc`:

Make sure you are in the directory where you downloaded lityc

.. code:: bash

  $ mkdir output
  $ ./lityc/lityc --abi --bin -o output BEC.sol
  $ cat output/BecToken.abi
  [{"constant":true,"inputs":[],"name":"name","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_spender","type":"address"},{"name":"_value","type":"uint256"}],"name":"approve","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"totalSupply","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_from","type":"address"},{"name":"_to","type":"address"},{"name":"_value","type":"uint256"}],"name":"transferFrom","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"decimals","outputs":[{"name":"","type":"uint8"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[],"name":"unpause","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"version","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[],"name":"paused","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[{"name":"_owner","type":"address"}],"name":"balanceOf","outputs":[{"name":"balance","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_receivers","type":"address[]"},{"name":"_value","type":"uint256"}],"name":"batchTransfer","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":false,"inputs":[],"name":"pause","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"owner","outputs":[{"name":"","type":"address"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[],"name":"symbol","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_to","type":"address"},{"name":"_value","type":"uint256"}],"name":"transfer","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[{"name":"_owner","type":"address"},{"name":"_spender","type":"address"}],"name":"allowance","outputs":[{"name":"remaining","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"newOwner","type":"address"}],"name":"transferOwnership","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"inputs":[],"payable":false,"stateMutability":"nonpayable","type":"constructor"},{"payable":false,"stateMutability":"nonpayable","type":"fallback"},{"anonymous":false,"inputs":[],"name":"Pause","type":"event"},{"anonymous":false,"inputs":[],"name":"Unpause","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"name":"previousOwner","type":"address"},{"indexed":true,"name":"newOwner","type":"address"}],"name":"OwnershipTransferred","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"name":"owner","type":"address"},{"indexed":true,"name":"spender","type":"address"},{"indexed":false,"name":"value","type":"uint256"}],"name":"Approval","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"name":"from","type":"address"},{"indexed":true,"name":"to","type":"address"},{"indexed":false,"name":"value","type":"uint256"}],"name":"Transfer","type":"event"}]
  $ cat output/BecToken.bin


Deploy contract to Travis locally
`````````````````````````````````

After we get contract ABI and bytecode, we could deploy it to Travis chain.

.. code:: bash

  # Get Travis console
  travis attach http://127.0.0.1:8545

  # Deploy contract (in Travis console)
  personal.unlockAccount(cmt.accounts[0], '1234');
  abi = [{"constant":true,"inputs":[],"name":"name","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_spender","type":"address"},{"name":"_value","type":"uint256"}],"name":"approve","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"totalSupply","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_from","type":"address"},{"name":"_to","type":"address"},{"name":"_value","type":"uint256"}],"name":"transferFrom","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"decimals","outputs":[{"name":"","type":"uint8"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[],"name":"unpause","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"version","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[],"name":"paused","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[{"name":"_owner","type":"address"}],"name":"balanceOf","outputs":[{"name":"balance","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_receivers","type":"address[]"},{"name":"_value","type":"uint256"}],"name":"batchTransfer","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":false,"inputs":[],"name":"pause","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"owner","outputs":[{"name":"","type":"address"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[],"name":"symbol","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"_to","type":"address"},{"name":"_value","type":"uint256"}],"name":"transfer","outputs":[{"name":"","type":"bool"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[{"name":"_owner","type":"address"},{"name":"_spender","type":"address"}],"name":"allowance","outputs":[{"name":"remaining","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":false,"inputs":[{"name":"newOwner","type":"address"}],"name":"transferOwnership","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"},{"inputs":[],"payable":false,"stateMutability":"nonpayable","type":"constructor"},{"payable":false,"stateMutability":"nonpayable","type":"fallback"},{"anonymous":false,"inputs":[],"name":"Pause","type":"event"},{"anonymous":false,"inputs":[],"name":"Unpause","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"name":"previousOwner","type":"address"},{"indexed":true,"name":"newOwner","type":"address"}],"name":"OwnershipTransferred","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"name":"owner","type":"address"},{"indexed":true,"name":"spender","type":"address"},{"indexed":false,"name":"value","type":"uint256"}],"name":"Approval","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"name":"from","type":"address"},{"indexed":true,"name":"to","type":"address"},{"indexed":false,"name":"value","type":"uint256"}],"name":"Transfer","type":"event"}]
  bytecode = "0x60806040526000600360146101000a81548160ff0219169083151502179055506040805190810160405280600b81526020017f426561757479436861696e000000000000000000000000000000000000000000815250600490805190602001906200006c929190620001e3565b506040805190810160405280600381526020017f424543000000000000000000000000000000000000000000000000000000000081525060059080519060200190620000ba929190620001e3565b506040805190810160405280600581526020017f312e302e300000000000000000000000000000000000000000000000000000008152506006908051906020019062000108929190620001e3565b506012600760006101000a81548160ff021916908360ff1602179055503480156200013257600080fd5b5033600360006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff160217905550600760009054906101000a900460ff1660ff16600a0a6401a13b860002600081905550600054600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000208190555062000292565b82805460018160011615610100020d166002900490600052602060002090601f016020900481019282601f106200022657805160ff191683800117855562000257565b8280016001018555821562000257579182015b828111156200025657825182559160200191906001019062000239565b5b5090506200026691906200026a565b5090565b6200028f91905b808211156200028b57600081600090555060010162000271565b5090565b90565b6116f380620002a26000396000f3006080604052600436106100e6576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff16806306fdde03146100f8578063095ea7b31461018857806318160ddd146101ed57806323b872dd14610218578063313ce5671461029d5780633f4ba83a146102ce57806354fd4d50146102e55780635c975abb1461037557806370a08231146103a457806383f12fec146103fb5780638456cb59146104835780638da5cb5b1461049a57806395d89b41146104f1578063a9059cbb14610581578063dd62ed3e146105e6578063f2fde38b1461065d575b3480156100f257600080fd5b50600080fd5b34801561010457600080fd5b5061010d6106a0565b6040518080602001828103825283818151815260200191508051906020019080838360005b8381101561014d578082015181840152602081019050610132565b50505050905090810190601f16801561017a5780820d805160018360200d6101000a0d1916815260200191505b509250505060405180910390f35b34801561019457600080fd5b506101d3600480360381019080803573ffffffffffffffffffffffffffffffffffffffff1690602001909291908035906020019092919050505061073e565b604051808215151515815260200191505060405180910390f35b3480156101f957600080fd5b5061020261076e565b6040518082815260200191505060405180910390f35b34801561022457600080fd5b50610283600480360381019080803573ffffffffffffffffffffffffffffffffffffffff169060200190929190803573ffffffffffffffffffffffffffffffffffffffff16906020019092919080359060200190929190505050610774565b604051808215151515815260200191505060405180910390f35b3480156102a957600080fd5b506102b26107a6565b604051808260ff1660ff16815260200191505060405180910390f35b3480156102da57600080fd5b506102e36107b9565b005b3480156102f157600080fd5b506102fa610879565b6040518080602001828103825283818151815260200191508051906020019080838360005b8381101561033a57808201518184015260208101905061031f565b50505050905090810190601f1680156103675780820d805160018360200d6101000a0d1916815260200191505b509250505060405180910390f35b34801561038157600080fd5b5061038a610917565b604051808215151515815260200191505060405180910390f35b3480156103b057600080fd5b506103e5600480360381019080803573ffffffffffffffffffffffffffffffffffffffff16906020019092919050505061092a565b6040518082815260200191505060405180910390f35b34801561040757600080fd5b506104696004803603810190808035906020019082018035906020019080806020026020016040519081016040528093929190818152602001838360200280828437820191505050505050919291929080359060200190929190505050610973565b604051808215151515815260200191505060405180910390f35b34801561048f57600080fd5b50610498610c0f565b005b3480156104a657600080fd5b506104af610cd0565b604051808273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200191505060405180910390f35b3480156104fd57600080fd5b50610506610cf6565b6040518080602001828103825283818151815260200191508051906020019080838360005b8381101561054657808201518184015260208101905061052b565b50505050905090810190601f1680156105735780820d805160018360200d6101000a0d1916815260200191505b509250505060405180910390f35b34801561058d57600080fd5b506105cc600480360381019080803573ffffffffffffffffffffffffffffffffffffffff16906020019092919080359060200190929190505050610d94565b604051808215151515815260200191505060405180910390f35b3480156105f257600080fd5b50610647600480360381019080803573ffffffffffffffffffffffffffffffffffffffff169060200190929190803573ffffffffffffffffffffffffffffffffffffffff169060200190929190505050610dc4565b6040518082815260200191505060405180910390f35b34801561066957600080fd5b5061069e600480360381019080803573ffffffffffffffffffffffffffffffffffffffff169060200190929190505050610e4b565b005b6004805460018160011615610100020d166002900480601f016020809104026020016040519081016040528092919081815260200182805460018160011615610100020d166002900480156107365780601f1061070b57610100808354040283529160200191610736565b820191906000526020600020905b8154815290600101906020018083116107195782900d601f168201915b505050505081565b6000600360149054906101000a900460ff1615151561075c57600080fd5b6107668383610fa3565b905092915050565b60005481565b6000600360149054906101000a900460ff1615151561079257600080fd5b61079d848484611095565b90509392505050565b600760009054906101000a900460ff1681565b600360009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff1614151561081557600080fd5b600360149054906101000a900460ff16151561083057600080fd5b6000600360146101000a81548160ff0219169083151502179055507f7805862f689e2f13df9f062ff482ad3ad112aca9e0847911ed832e158c525b3360405160405180910390a1565b6006805460018160011615610100020d166002900480601f016020809104026020016040519081016040528092919081815260200182805460018160011615610100020d1660029004801561090f5780601f106108e45761010080835404028352916020019161090f565b820191906000526020600020905b8154815290600101906020018083116108f25782900d601f168201915b505050505081565b600360149054906101000a900460ff1681565b6000600160008373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020549050919050565b600080600080600360149054906101000a900460ff1615151561099557600080fd5b8551925084830291506000831180156109af575060148311155b15156109ba57600080fd5b600085118015610a09575081600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205410155b1515610a1457600080fd5b610a6682600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461146090919063ffffffff16565b600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002081905550600090505b82811015610c0257610b1f85600160008985815181101515610acc57fe5b9060200190602002015173ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461147990919063ffffffff16565b600160008884815181101515610b3157fe5b9060200190602002015173ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020819055508581815181101515610b8757fe5b9060200190602002015173ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff167fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef876040518082815260200191505060405180910390a38080600101915050610aae565b6001935050505092915050565b600360009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff16141515610c6b57600080fd5b600360149054906101000a900460ff16151515610c8757600080fd5b6001600360146101000a81548160ff0219169083151502179055507f6985a02210a168e66602d3235cb6db0e70f92b3ba4d376a33c0f3d9434bff62560405160405180910390a1565b600360009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1681565b6005805460018160011615610100020d166002900480601f016020809104026020016040519081016040528092919081815260200182805460018160011615610100020d16600290048015610d8c5780601f10610d6157610100808354040283529160200191610d8c565b820191906000526020600020905b815481529060010190602001808311610d6f5782900d601f168201915b505050505081565b6000600360149054906101000a900460ff16151515610db257600080fd5b610dbc8383611497565b905092915050565b6000600260008473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060008373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002054905092915050565b600360009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff16141515610ea757600080fd5b600073ffffffffffffffffffffffffffffffffffffffff168173ffffffffffffffffffffffffffffffffffffffff1614151515610ee357600080fd5b8073ffffffffffffffffffffffffffffffffffffffff16600360009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff167f8be0079c531659141344cd1fd0a4f28419497f9722a3daafe3b4186f6b6457e060405160405180910390a380600360006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff16021790555050565b600081600260003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060008573ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020819055508273ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff167f8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925846040518082815260200191505060405180910390a36001905092915050565b60008073ffffffffffffffffffffffffffffffffffffffff168373ffffffffffffffffffffffffffffffffffffffff16141515156110d257600080fd5b6000821180156111215750600160008573ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020548211155b151561112c57600080fd5b600260008573ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205482111515156111b757600080fd5b61120982600160008773ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461146090919063ffffffff16565b600160008673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000208190555061129e82600160008673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461147990919063ffffffff16565b600160008573ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000208190555061137082600260008773ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461146090919063ffffffff16565b600260008673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020819055508273ffffffffffffffffffffffffffffffffffffffff168473ffffffffffffffffffffffffffffffffffffffff167fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef846040518082815260200191505060405180910390a3600190509392505050565b600082821115151561146e57fe5b818303905092915050565b600080828401905083811015151561148d57fe5b8091505092915050565b60008073ffffffffffffffffffffffffffffffffffffffff168373ffffffffffffffffffffffffffffffffffffffff16141515156114d457600080fd5b6000821180156115235750600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020548211155b151561152e57600080fd5b61158082600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461146090919063ffffffff16565b600160003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000208190555061161582600160008673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019081526020016000205461147990919063ffffffff16565b600160008573ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001908152602001600020819055508273ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff167fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef846040518082815260200191505060405180910390a360019050929150505600a165627a7a723058208448cdec846a44aa55c98cfadce74410e5ea1c7c7ffbd6c21457a86ddf4b88aa0029"
  contract = web3.cmt.contract(abi);
  c = contract.new(
    {
      from: web3.cmt.accounts[0],
      data: bytecode,
      gas: "4700000"
    },
    function(e, contract) {
      console.log("contract address: " + contract.address);
      console.log("transactionHash: " + contract.transactionHash);
    }
  );

Test overflow protection
````````````````````````

.. _BECTransaction: https://etherscan.io/tx/0xad89ff16fd1ebe3a0a7cf4ed282302c06626c1af33221ebe0d3a470aba4a660f

Let's follow this transaction BECTransaction_ to replay the BEC event.

.. code:: bash

  # Call betchTransfer with overflow parameters to test.
  > c.batchTransfer.sendTransaction(["0xb4d30cac5124b46c2df0cf3e3e1be05f42119033", "0x0e823ffe018727585eaf5bc769fa80472f76c3d7"], 0x8000000000000000000000000000000000000000000000000000000000000000, {from:web3.cmt.accounts[0], gas: 1000000})
  "0x64c8c5f06bf467917a20b45f52dca8f03613455367c3f36222f27d9fa8992d41"

  > t="0x64c8c5f06bf467917a20b45f52dca8f03613455367c3f36222f27d9fa8992d41"
  "0x64c8c5f06bf467917a20b45f52dca8f03613455367c3f36222f27d9fa8992d41"

  # Our vm consume all of the gas and make this transaction fail, because it will overflow and trigger the `throw` opcode.
  > cmt.getTransactionReceipt(t)
  {
    blockHash: "0xdbae44e6da9087f234a8bac16676e6feee03d02a536cc5b8dc7ccfbceacca3f5",
    blockNumber: 30,
    contractAddress: null,
    cumulativeGasUsed: 1000000,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 1000000,
    logs: [],
    logsBloom: "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    root: "0xe34245bd2165cf9be9f4f65ff09d19544fcae32fcd4f301e5f73d1bfa39c78b1",
    to: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
    transactionHash: "0x64c8c5f06bf467917a20b45f52dca8f03613455367c3f36222f27d9fa8992d41",
    transactionIndex: 0
  }

  > cmt.getTransaction(t)
  {
    blockHash: "0xdbae44e6da9087f234a8bac16676e6feee03d02a536cc5b8dc7ccfbceacca3f5",
    blockNumber: 30,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gas: 1000000,
    gasPrice: 2000000000,
    hash: "0x64c8c5f06bf467917a20b45f52dca8f03613455367c3f36222f27d9fa8992d41",
    input: "0x83f12fec0000000000000000000000000000000000000000000000000000000000000040800000000000016c889a28c160ce0422bb9138ff1d4e482740000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000b4d30cac5124b46c2df0cf3e3e1be05f421190330000000000000000000000000e823ffe018727585eaf5bc769fa80472f76c3d7",
    nonce: 1,
    r: "0x123fbcffac70f01bdb2d71b15431e945867c6a6c735194fc4ecb5c462ed2e67a",
    s: "0x656d875d5131e4d91026f4a952bcc4daeeba7ee7bea45c95e7b2949051920a1f",
    to: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
    transactionIndex: 0,
    v: "0x49",
    value: 0
  }

  # If we modify the second parameter from 0x8000000000000000000000000000000000000000000000000000000000000000 to 0x8, this transaction has no overflow issue. Now it can execute normally.
  > t=c.batchTransfer.sendTransaction(["0xb4d30cac5124b46c2df0cf3e3e1be05f42119033", "0x0e823ffe018727585eaf5bc769fa80472f76c3d7"], 0x8, {from:web3.cmt.accounts[0], gas: 1000000})
  "0xef69425c20b1d540be8c8e23bd7565be727b37571c4f47a89a3457b40eb81cbf"

  > cmt.getTransaction(t)
  {
    blockHash: "0xcb0253add2bb035db135a68a07c4d9a6f7b81e0587df47b86d6456331a04fca4",
    blockNumber: 52,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gas: 1000000,
    gasPrice: 2000000000,
    hash: "0xef69425c20b1d540be8c8e23bd7565be727b37571c4f47a89a3457b40eb81cbf",
    input: "0x83f12fec000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000002000000000000000000000000b4d30cac5124b46c2df0cf3e3e1be05f421190330000000000000000000000000e823ffe018727585eaf5bc769fa80472f76c3d7",
    nonce: 2,
    r: "0x71e99ddcc132d738836b117f95aed93bf366d3a8d009742349423a878de0545",
    s: "0x7d4df4f45931c5b48a36567bef4b84db966faf0188bd317379189cf3c61a1005",
    to: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
    transactionIndex: 0,
    v: "0x4a",
    value: 0
  }

  # This transaction only cost 76776 gas.
  > cmt.getTransactionReceipt(t)
  {
    blockHash: "0xcb0253add2bb035db135a68a07c4d9a6f7b81e0587df47b86d6456331a04fca4",
    blockNumber: 52,
    contractAddress: null,
    cumulativeGasUsed: 76776,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 76776,
    logs: [{
        address: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
        blockHash: "0xcb0253add2bb035db135a68a07c4d9a6f7b81e0587df47b86d6456331a04fca4",
        blockNumber: 52,
        data: "0x0000000000000000000000000000000000000000000000000000000000000008",
        logIndex: 0,
        removed: false,
        topics: ["0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef", "0x0000000000000000000000007eff122b94897ea5b0e2a9abf47b86337fafebdc", "0x000000000000000000000000b4d30cac5124b46c2df0cf3e3e1be05f42119033"],
        transactionHash: "0xef69425c20b1d540be8c8e23bd7565be727b37571c4f47a89a3457b40eb81cbf",
        transactionIndex: 0
    }, {
        address: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
        blockHash: "0xcb0253add2bb035db135a68a07c4d9a6f7b81e0587df47b86d6456331a04fca4",
        blockNumber: 52,
        data: "0x0000000000000000000000000000000000000000000000000000000000000008",
        logIndex: 1,
        removed: false,
        topics: ["0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef", "0x0000000000000000000000007eff122b94897ea5b0e2a9abf47b86337fafebdc", "0x0000000000000000000000000e823ffe018727585eaf5bc769fa80472f76c3d7"],
        transactionHash: "0xef69425c20b1d540be8c8e23bd7565be727b37571c4f47a89a3457b40eb81cbf",
        transactionIndex: 0
    }],
    logsBloom: "0x00000000000000000000000000000000000001000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000008000000000008000010000000000002000000000000000000000000000000110000000004000000000000000000000000000000000000000000080000000000000000000000000000000000000000000001000000000000000000000000000000000000000000002000200000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000000000",
    root: "0x0b74d2224a735a6af818664b9ae7cb52ff3575b41dcab616114274bdb311c756",
    to: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
    transactionHash: "0xef69425c20b1d540be8c8e23bd7565be727b37571c4f47a89a3457b40eb81cbf",
    transactionIndex: 0
  }

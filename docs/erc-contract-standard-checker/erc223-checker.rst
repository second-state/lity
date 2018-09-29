.. _erc223-contract-standard-checker:

ERC223 Contract Standard Checker
================================

Make sure you are in the folder where lityc is installed. 

For example your location should be "~/lity/build" if you followed `this tutorial <http://lity.readthedocs.io/en/latest/download.html>`_.

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC223 <contract file>


Examples
--------

- ERC223 Interface (erc223_interface.sol)

.. code:: ts

  //pragma solidity ^0.4.23;
  pragma lity ^1.2.0;

  contract ERC223Interface {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success);
    function transfer(address to, uint tokens, bytes data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
  }


- Expect output

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC223 erc223_interface.sol

  erc223_standard.sol:3:1: Info: ERC223Interface is compatible to ERC223.
  contract ERC223Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



- ERC223 Token example (erc223_token.sol)

.. code:: ts

  //pragma solidity ^0.4.11;
  pragma lity ^1.2.0;

  library SafeMath {
    function mul(uint a, uint b) internal pure returns (uint) {
    uint c = a * b;
    _assert(a == 0 || c / a == b);
    return c;
  }

  function div(uint a, uint b) internal pure returns (uint) {
    // _assert(b > 0); // Solidity automatically throws when dividing by 0
    uint c = a / b;
    // _assert(a == b * c + a % b); // There is no case in which this doesn't hold
    return c;
  }

  function sub(uint a, uint b) internal pure returns (uint) {
    _assert(b <= a);
    return a - b;
  }

  function add(uint a, uint b) internal pure returns (uint) {
    uint c = a + b;
    _assert(c >= a);
    return c;
  }

  function max64(uint64 a, uint64 b) internal pure returns (uint64) {
    return a >= b ? a : b;
  }

  function min64(uint64 a, uint64 b) internal pure returns (uint64) {
    return a < b ? a : b;
  }

  function max256(uint256 a, uint256 b) internal pure returns (uint256) {
    return a >= b ? a : b;
  }

  function min256(uint256 a, uint256 b) internal pure returns (uint256) {
    return a < b ? a : b;
  }

  function _assert(bool assertion) internal pure {
    if (!assertion) {
      revert();
    }
  }
  }

  contract ERC223Interface {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens, bytes data) public returns (bool success);
    function transfer(address to, uint tokens) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
  }

  contract ERC223ReceivingContract {
    function tokenFallback(address _from, uint _value, bytes _data) public;
  }

  contract ERC223Token is ERC223Interface {
    using SafeMath for uint;

    mapping(address => uint) balances; // List of user balances.

    function totalSupply() public view returns (uint) {
      return 2**18;
    }

    function transfer(address _to, uint _value, bytes _data) public returns (bool) {
      // Standard function transfer similar to ERC20 transfer with no _data .
      // Added due to backwards compatibility reasons .
      uint codeLength;

      assembly {
        // Retrieve the size of the code on target address, this needs assembly .
        codeLength := extcodesize(_to)
      }

      balances[msg.sender] = balances[msg.sender].sub(_value);
      balances[_to] = balances[_to].add(_value);
      if(codeLength>0) {
        ERC223ReceivingContract receiver = ERC223ReceivingContract(_to);
        receiver.tokenFallback(msg.sender, _value, _data);
        return true;
      }
      emit Transfer(msg.sender, _to, _value, _data);
      return true;
    }

    function transfer(address _to, uint _value) public returns (bool) {
      uint codeLength;
      bytes memory empty;

      assembly {
        // Retrieve the size of the code on target address, this needs assembly .
        codeLength := extcodesize(_to)
      }

      balances[msg.sender] = balances[msg.sender].sub(_value);
      balances[_to] = balances[_to].add(_value);
      if(codeLength>0) {
        ERC223ReceivingContract receiver = ERC223ReceivingContract(_to);
        receiver.tokenFallback(msg.sender, _value, empty);
        return true;
      }
      emit Transfer(msg.sender, _to, _value, empty);
      return true;
    }

    function balanceOf(address _owner) public view returns (uint balance) {
      return balances[_owner];
    }
  }


- Expect output

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC223 erc223_token.sol

  erc223_token.sol:6:1: Info: Missing 'totalSupply' with type signature 'function () view external returns (uint256)'. SafeMath is not compatible to ERC223.
  library SafeMath {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc223_token.sol:54:1: Info: ERC223Interface is compatible to ERC223.
  contract ERC223Interface {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc223_token.sol:65:1: Info: Missing 'totalSupply' with type signature 'function () view external returns (uint256)'. ERC223ReceivingContract is not compatible to ERC223.
  contract ERC223ReceivingContract {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc223_token.sol:78:1: Info: ERC223Token is compatible to ERC223.
  contract ERC223Token is ERC223Interface {
  ^ (Relevant source part starts here and spans across multiple lines).

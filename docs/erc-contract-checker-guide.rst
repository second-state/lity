ERC contract checker guide
~~~~~~~~~~~~~~~~~~~~~~~~~~

Please make sure that lityc has been installed on your system. If not, please follow :doc:`Getting started <getting-started>`.

.. _erc-contract-checker-guide:

ERC20 Contract Standard Checker
===============================

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ lityc --contract-standard ERC20 <contract file>


Examples
--------

- ERC20 Interface (erc20_interface.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC20Interface {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC20 erc20_interface.sol

  erc20_standard.sol:3:1: Info: ERC20Interface is compatible to ERC20.
  contract ERC20Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



- ERC20 Interface with wrong mutability (wrong_mutability.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC20Interface {
    function totalSupply() public pure returns (uint); // mutability should be view, not pure
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function allowance(address tokenOwner, address spender) public view returns (uint remaining);
    function transfer(address to, uint tokens) public returns (bool success);
    function approve(address spender, uint tokens) public returns (bool success);
    function transferFrom(address from, address to, uint tokens) public returns (bool success);

    event Transfer(address indexed from, address indexed to, uint tokens);
    event Approval(address indexed tokenOwner, address indexed spender, uint tokens);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC20 wrong_mutability.sol

  wrong_mutability.sol:3:1: Info: Missing 'totalSupply' with type signature 'function () view external returns (uint256)'. ERC20Interface is not compatible to ERC20.
  contract ERC20Interface {
  ^ (Relevant source part starts here and spans across multiple lines).


ERC223 Contract Standard Checker
================================

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ lityc --contract-standard ERC223 <contract file>


Examples
--------

- ERC223 Interface (erc223_interface.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC223Interface {
    function totalSupply() public view returns (uint);
    function balanceOf(address tokenOwner) public view returns (uint balance);
    function transfer(address to, uint tokens) public returns (bool success);
    function transfer(address to, uint tokens, bytes data) public returns (bool success);
    event Transfer(address indexed from, address indexed to, uint tokens, bytes data);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC223 erc223_interface.sol

  erc223_standard.sol:3:1: Info: ERC223Interface is compatible to ERC223.
  contract ERC223Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



- ERC223 Token example (erc223_token.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

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

  $ lityc --contract-standard ERC223 erc223_token.sol

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


ERC721 Contract Standard Checker
================================

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ lityc --contract-standard ERC721 <contract file>


Examples
--------

- ERC721 Interface (erc721_interface.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC721Interface {
    event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
    event Approval(address indexed _owner, address indexed _approved, uint256 _tokenId);
    event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);
    function balanceOf(address _owner) external view returns (uint256);
    function ownerOf(uint256 _tokenId) external view returns (address);
    function safeTransferFrom(address _from, address _to, uint256 _tokenId, bytes data) external payable;
    function safeTransferFrom(address _from, address _to, uint256 _tokenId) external payable;
    function transferFrom(address _from, address _to, uint256 _tokenId) external payable;
    function approve(address _approved, uint256 _tokenId) external payable;
    function setApprovalForAll(address _operator, bool _approved) external;
    function getApproved(uint256 _tokenId) external view returns (address);
    function isApprovedForAll(address _owner, address _operator) external view returns (bool);
    function supportsInterface(bytes4 interfaceID) external view returns (bool);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC721 erc721_interface.sol
  erc721_interface.sol:3:1: Info: ERC721Interface is compatible to ERC721.
  contract ERC721Interface {
  ^ (Relevant source part starts here and spans across multiple lines).




- ERC721 Interface with wrong modification level (wrong_modification_level.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC721Interface {
    event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
    event Approval(address indexed _owner, address indexed _approved, uint256 _tokenId);
    event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);
    function balanceOf(address _owner) external view returns (uint256);
    function ownerOf(uint256 _tokenId) external view returns (address);
    function safeTransferFrom(address _from, address _to, uint256 _tokenId, bytes data) external payable;
    function safeTransferFrom(address _from, address _to, uint256 _tokenId) external payable;
    function transferFrom(address _from, address _to, uint256 _tokenId) external; // missing payable
    function approve(address _approved, uint256 _tokenId) external payable;
    function setApprovalForAll(address _operator, bool _approved) external;
    function getApproved(uint256 _tokenId) external view returns (address);
    function isApprovedForAll(address _owner, address _operator) external view returns (bool);
    function supportsInterface(bytes4 interfaceID) external view returns (bool);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC721 wrong_modification_level.sol

  wrong_modification_level.sol:3:1: Info: Missing 'transferFrom' with type signature 'function (address,address,uint256) payable external'. ERC721Interface is not compatible to ERC721.
  contract ERC721Interface {
  ^ (Relevant source part starts here and spans across multiple lines).


ERC827 Contract Standard Checker
================================

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ lityc --contract-standard ERC827 <contract file>


Examples
--------

- ERC827 Interface (erc827_standard_no_inheritance.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC827Interface {
    function totalSupply() public view returns (uint256);
    function balanceOf(address who) public view returns (uint256);
    function transfer(address to, uint256 value) public returns (bool);
    function allowance(address owner, address spender) public view returns (uint256);
    function transferFrom(address from, address to, uint256 value) public returns (bool);
    function approve(address spender, uint256 value) public returns (bool);
    event Approval(address indexed owner, address indexed spender, uint256 value);
    event Transfer(address indexed from, address indexed to, uint256 value);
    function transferAndCall(address _to, uint256 _value, bytes _data) public payable returns (bool);
    function transferFromAndCall( address _from, address _to, uint256 _value, bytes _data) public payable returns (bool);
    function approveAndCall(address _spender, uint256 _value, bytes _data) public payable returns (bool);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC827 erc827_standard_no_inheritance.sol

  erc827_standard_no_inheritance.sol:3:1: Info: ERC827Interface is compatible to ERC827.
  contract ERC827Interface {
  ^ (Relevant source part starts here and spans across multiple lines).


- ERC827 Interface inheritance (erc827_inheritance.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC827Base {
    function totalSupply() public view returns (uint256);
    function balanceOf(address who) public view returns (uint256);
    function transfer(address to, uint256 value) public returns (bool);
    function allowance(address owner, address spender) public view returns (uint256);
    function transferFrom(address from, address to, uint256 value) public returns (bool);
    function approve(address spender, uint256 value) public returns (bool);
    event Approval(address indexed owner, address indexed spender, uint256 value);
    event Transfer(address indexed from, address indexed to, uint256 value);
  }
  contract ERC827Interface is ERC827Base {
    function transferAndCall(address _to, uint256 _value, bytes _data) public payable returns (bool);
    function transferFromAndCall( address _from, address _to, uint256 _value, bytes _data) public payable returns (bool);
    function approveAndCall(address _spender, uint256 _value, bytes _data) public payable returns (bool);
  }

- Expect output

.. code:: bash

  $ lityc --contract-standard ERC827 erc827_standard.sol

  erc827_standard.sol:3:1: Info: Missing 'transferAndCall' with type signature 'function (address,uint256,bytes memory) payable external returns (bool)'. ERC827Base is not compatible to ERC827.
  contract ERC827Base {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc827_standard.sol:13:1: Info: ERC827Interface is compatible to ERC827.
  contract ERC827Interface is ERC827Base {
  ^ (Relevant source part starts here and spans across multiple lines).


ERC884 Contract Standard Checker
================================

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ lityc --contract-standard ERC884 <contract file>


Examples
--------

- ERC884 Interface (erc884_interface.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract C20 {
    function totalSupply() public view returns (uint256);
    function balanceOf(address who) public view returns (uint256);
    function transfer(address to, uint256 value) public returns (bool);
    function allowance(address owner, address spender) public view returns (uint256);
    function transferFrom(address from, address to, uint256 value) public returns (bool);
    function approve(address spender, uint256 value) public returns (bool);
    event Approval(address indexed owner, address indexed spender, uint256 value);
    event Transfer(address indexed from, address indexed to, uint256 value);
  }
  contract ERC884Interface is C20 {
    event VerifiedAddressAdded( address indexed addr, bytes32 hash, address indexed sender);
    event VerifiedAddressRemoved(address indexed addr, address indexed sender);
    event VerifiedAddressUpdated( address indexed addr, bytes32 oldHash, bytes32 hash, address indexed sender);
    event VerifiedAddressSuperseded( address indexed original, address indexed replacement, address indexed sender);
    function addVerified(address addr, bytes32 hash) public;
    function removeVerified(address addr) public;
    function updateVerified(address addr, bytes32 hash) public;
    function cancelAndReissue(address original, address replacement) public;
    function transfer(address to, uint256 value) public returns (bool);
    function transferFrom(address from, address to, uint256 value) public returns (bool);
    function isVerified(address addr) public view returns (bool);
    function isHolder(address addr) public view returns (bool);
    function hasHash(address addr, bytes32 hash) public view returns (bool);
    function holderCount() public view returns (uint);
    function holderAt(uint256 index) public view returns (address);
    function isSuperseded(address addr) public view returns (bool);
    function getCurrentFor(address addr) public view returns (address);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC884 erc884_standard.sol

  erc884_standard.sol:3:1: Info: Missing 'VerifiedAddressAdded' with type signature 'function (address,bytes32,address)'. C20 is not compatible to ERC884.
  contract C20 {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc884_standard.sol:13:1: Info: ERC884Interface is compatible to ERC884.
  contract ERC884Interface is C20 {
  ^ (Relevant source part starts here and spans across multiple lines).



- ERC884 Interface with missing function (missing_function.sol)

.. code-block:: Lity

  pragma lity ^1.2.4;

  contract ERC884Interface {
    event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
    event Approval(address indexed _owner, address indexed _approved, uint256 _tokenId);
    event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);
    function balanceOf(address _owner) external view returns (uint256);
    function ownerOf(uint256 _tokenId) external view returns (address);
    function safeTransferFrom(address _from, address _to, uint256 _tokenId, bytes data) external payable;
    function safeTransferFrom(address _from, address _to, uint256 _tokenId) external payable;
    function transferFrom(address _from, address _to, uint256 _tokenId) external; // missing payable
    function approve(address _approved, uint256 _tokenId) external payable;
    function setApprovalForAll(address _operator, bool _approved) external;
    function getApproved(uint256 _tokenId) external view returns (address);
    function isApprovedForAll(address _owner, address _operator) external view returns (bool);
    function supportsInterface(bytes4 interfaceID) external view returns (bool);
  }


- Expect output

.. code:: bash

  $ lityc --contract-standard ERC884 missing_function.sol

  missing_function.sol:3:1: Info: Missing 'VerifiedAddressAdded' with type signature 'function (address,bytes32,address)'. ERC884Interface is not compatible to ERC884.
  contract ERC884Interface {
  ^ (Relevant source part starts here and spans across multiple lines).


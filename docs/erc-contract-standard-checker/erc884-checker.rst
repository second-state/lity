.. _erc884-contract-standard-checker:

ERC884 Contract Standard Checker
================================

Make sure you are in the folder where lityc is installed. 

For example your location should be "~/lity/build" if you followed `this tutorial <http://lity.readthedocs.io/en/latest/download.html>`_.

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC884 <contract file>


Examples
--------

- ERC884 Interface (erc884_interface.sol)

.. code:: ts

  pragma solidity ^0.4.23;

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

  $ ./lityc/lityc --contract-standard ERC884 erc884_standard.sol

  erc884_standard.sol:3:1: Info: Missing 'VerifiedAddressAdded' with type signature 'function (address,bytes32,address)'. C20 is not compatible to ERC884.
  contract C20 {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc884_standard.sol:13:1: Info: ERC884Interface is compatible to ERC884.
  contract ERC884Interface is C20 {
  ^ (Relevant source part starts here and spans across multiple lines).



- ERC884 Interface with missing function (missing_function.sol)

.. code:: ts

  pragma solidity ^0.4.23;

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

  $ ./lityc/lityc --contract-standard ERC884 missing_function.sol

  missing_function.sol:3:1: Info: Missing 'VerifiedAddressAdded' with type signature 'function (address,bytes32,address)'. ERC884Interface is not compatible to ERC884.
  contract ERC884Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



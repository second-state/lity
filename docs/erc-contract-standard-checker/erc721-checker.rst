.. _erc721-contract-standard-checker:

ERC721 Contract Standard Checker
================================

Make sure you are in the folder where lityc is installed. 

For example your location should be "~/lity/build" if you followed `this tutorial <http://lity.readthedocs.io/en/latest/download.html>`_.

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC721 <contract file>


Examples
--------

- ERC721 Interface (erc721_interface.sol)

.. code:: ts

  //pragma solidity ^0.4.23;
  pragma lity ^1.2.0;

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

  $ ./lityc/lityc --contract-standard ERC721 erc721_interface.sol
  erc721_interface.sol:3:1: Info: ERC721Interface is compatible to ERC721.
  contract ERC721Interface {
  ^ (Relevant source part starts here and spans across multiple lines).




- ERC721 Interface with wrong modification level (wrong_modification_level.sol)

.. code:: ts

  //pragma solidity ^0.4.23;
  pragma lity ^1.2.0;

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

  $ ./lityc/lityc --contract-standard ERC721 wrong_modification_level.sol

  wrong_modification_level.sol:3:1: Info: Missing 'transferFrom' with type signature 'function (address,address,uint256) payable external'. ERC721Interface is not compatible to ERC721.
  contract ERC721Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



.. _erc827-contract-standard-checker:

ERC827 Contract Standard Checker
================================

Make sure you are in the folder where lityc is installed. 

For example your location should be "~/lity/build" if you followed `this tutorial <http://lity.readthedocs.io/en/latest/download.html>`_.

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC827 <contract file>


Examples
--------

- ERC827 Interface (erc827_standard_no_inheritance.sol)

.. code:: ts

  pragma solidity ^0.4.23;

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

  $ ./lityc/lityc --contract-standard ERC827 erc827_standard_no_inheritance.sol

  erc827_standard_no_inheritance.sol:3:1: Info: ERC827Interface is compatible to ERC827.
  contract ERC827Interface {
  ^ (Relevant source part starts here and spans across multiple lines).


- ERC827 Interface inheritance (erc827_inheritance.sol)

.. code:: ts

  pragma solidity ^0.4.23;

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

  $ ./lityc/lityc --contract-standard ERC827 erc827_standard.sol

  erc827_standard.sol:3:1: Info: Missing 'transferAndCall' with type signature 'function (address,uint256,bytes memory) payable external returns (bool)'. ERC827Base is not compatible to ERC827.
  contract ERC827Base {
  ^ (Relevant source part starts here and spans across multiple lines).
  erc827_standard.sol:13:1: Info: ERC827Interface is compatible to ERC827.
  contract ERC827Interface is ERC827Base {
  ^ (Relevant source part starts here and spans across multiple lines).



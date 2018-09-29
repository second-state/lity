.. _erc20-contract-standard-checker:

ERC20 Contract Standard Checker
===============================

Make sure you are in the folder where lityc is installed. 

For example your location should be "~/lity/build" if you followed `this tutorial <http://lity.readthedocs.io/en/latest/download.html>`_.

Enable contract standard checker with specific ERC
--------------------------------------------------

.. code:: bash

  $ ./lityc/lityc --contract-standard ERC20 <contract file>


Examples
--------

- ERC20 Interface (erc20_interface.sol)

.. code:: ts

  //pragma solidity ^0.4.23;
  pragma lity ^1.2.0;

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

  $ ./lityc/lityc --contract-standard ERC20 erc20_interface.sol

  erc20_standard.sol:3:1: Info: ERC20Interface is compatible to ERC20.
  contract ERC20Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



- ERC20 Interface with wrong mutability (wrong_mutability.sol)

.. code:: ts

  //pragma solidity ^0.4.23;
  pragma lity ^1.2.0;

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

  $ ./lityc/lityc --contract-standard ERC20 wrong_mutability.sol

  wrong_mutability.sol:3:1: Info: Missing 'totalSupply' with type signature 'function () view external returns (uint256)'. ERC20Interface is not compatible to ERC20.
  contract ERC20Interface {
  ^ (Relevant source part starts here and spans across multiple lines).



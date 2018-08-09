================================================================
New Type for Secure Transaction
================================================================

.. _token-type:

At the begin of 2018, BEC smart contract were hacked because a human error. There is one and only mathematical operation forgot to use SafeMath Library and overflowed. In the Lity, we introduce a new type `token_t` in the language to help the Smart Contract developer keep the risk of overflow away.

Token Type
----------

Lity provides a new type `token_t` which extends from `uint256`, `uint`. `token_t` is designed for calculating balance of token or any other unsigned integer with overflow protection by default. With this feature, developers don't need to care about how and where to use SafeMath library, because `token_t` will handle all of the conditions automatically.

Because of the security issue, `token_t` is not allowed implicit conversion to the other types. If you want to convert, tying to use explicit conversion like the example below:

.. code:: bash

  token_t a = 2000;
  token_t b = a * 12345;
  // uint c = b; // Error: Implicit conversion is Not Allowed
  uint c = uint(a); // OK: Explicit conversion uses strict type conversion

In addition, `token_t` will automatically check overflow during runtime on those operations: add, sub, mul. The behavior just like the SafeMath, but the developer could not to add it manually.

Example
-------

.. code::

  pragma solidity ^0.4.22;

  contract C{
      function c() public pure returns (token_t) {
          token_t a = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
          token_t b = 0x8fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
          return (a*b);
      }
  }

Let compile it with `lityc` and run the bytecode on evm. The evm will raise a INVALID and halt the buggy contract.

.. code::

  Missing opcode 0xfepc=00000123 gas=9999999917 cost=10 ERROR: invalid opcode 0xfe
  Stack:
  00000000  7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  00000001  8fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  00000002  8fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  00000003  7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  00000004  0000000000000000000000000000000000000000000000000000000000000000

  #### LOGS ####


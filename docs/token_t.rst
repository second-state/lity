================================================================
New Type for Secure Transaction
================================================================

At the begin of 2018, BEC smart contract were hacked because a human error. There is one and only mathematical operation forgot to use SafeMath Library and overflowed. In the Lity, we introduce a new type `token_t` in the language to help the Smart Contract developer keep the risk of overflow away.

Token Type
`````````````````
Token type `token_t` is a new type extend from `uint256`, `uint`. `token_t` type is not allowed implicit conversion to the other types.

.. code:: bash

  token_t a = 2000;
  token_t b = a * 12345;
  //uint c = b; // Not Allowed
  uint c = uint(a); // OK, use strict type conversion

Otherwise, `token_t` will automatically check overflow during runtime on those operations : add, sub, mul. The behavior just like the SafeMath, but the developer could not to add it manually.

Example
````````````````````

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



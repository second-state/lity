================================================================
FreeGas - Let contract owner pay the tx fee for you
================================================================

.. _freegas:

Lity provides a new modifier `freegas`. With `freegas`, developers can mark specific functions to be `freegas`, which means the transaction fee will be paid by the callee contract and caller user doesn't need to care about the gas fee.

You can find the usage below.

Example 1
---------

.. code-block:: Lity

  pragma lity ^1.2.7;

  contract FreeGas {
      uint x;
      function c(uint a) public freegas returns (uint) {
          x = a;
          return x;
      }
  }


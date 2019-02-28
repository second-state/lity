================================================================
FreeGas - Let contract owner pay the tx fee for you
================================================================

.. _freegas:

.. WARNING::
   Lity supports built-in random function after lity v1.2.7.



Built-it FreeGas Modifier
-------------------------

Lity provides a new modifier `freegas`. With `freegas`, developers can mark specific functions to be `freegas`, which means the transaction fee will be paid by the callee contract and caller user doesn't need to care about the gas fee.

Because `freegas` will consume the contract balance for the transaction fee, Dapp developers should make sure that the deployed contract must have a fallback payable function and should send some money into the contract.


How Virtual Machine handles the freegas function
------------------------------------------------

When ``freegas`` is triggered, VM will follow these rules to calculate
transaction fee:
    if Balance(Callee contract) >= transaction fee,
        then Use Balance(Calle contract) to pay the fee
            and refund all of the reserved balance
    else
        then Use origin tx gas to pay the fee
            and refund the remaining balance

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
      function () public payable {}
  }


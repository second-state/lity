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

The blockchain could pass ``gasPrice=0`` transactions to the VM to signal that the caller has requested a ``freegas`` function call. Learn more about `free transactions <https://travis.readthedocs.io/en/latest/transactions.html#free-transactions>`_ The VM now performs the following actions.

* If ``gasLimit < 5000000``, it will just process it normally. The caller's gas fee will be 0
* If ``gasLimit > 5000000``, it will require the contract function to be ``freegas``
    * The call consumes a max of ``(default gasPrice) * gasLimit`` amount of gas
    * the TX fails if the function is not ``freegas`` 
    * the TX fails if the contract does not have enough fund to cover gas
    
The gas fee for caller is ALWAYS 0 in this case.


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


Schedule Transaction
====================

.. _schedule-tx:

Warning: Schedule Transaction is under development.

Schedule Transaction is not fully supported by Lity yet.

Draft Grammar
-------------

.. code:: ts

  schedule(<External Function Call>, <Timestamp>);

  // <External Function Call>:
  //    Some examples:
  //      this.deposit(1000, 0x95...185)
  //      erc223receiver.tokenFallBack(...)
  // <Timestamp>:
  //    Unix timestamp
  // The <External Function Call> will be executed at <Timestamp>.


Examples
--------

.. code:: ts

  // pragma lity ^1.3.0;

  contract Schedule {
    function deposit(uint amount, address to) private {
        to.transfer(amount);
    }
    function setTimer(uint amount, address to, uint timestamp) public {
        schedule(this.deposit(amount, to), timestamp);
    }
  }

Validator Only Contract
=======================

.. _validator-only-contract:

Warning: Validator only contract is under development.

Validator only contract is not fully supported by Lity yet.

Draft Grammar
-------------

.. code:: ts

  isValidator(<address>) returns (bool returnValue);

  // isValidator is a built-in function provided by Lity.
  // isValidator only takes one parameter, an address, to check this address is a validtor or not.
  // If the address is in validator list => return true
  // Otherwise => return false


Examples
--------

.. code:: ts

  // pragma lity ^1.3.0;

  contract BTERelay {
    uint[] BTCHeaders;
    modifier ValidatorOnly() {
        require(isValidator(msg.sender));
        _;
    }

    function saveBTCHeader(uint blockHash) ValidatorOnly {
      BTCHeaders.append(headerHash);
    }

    function getBTCHeader(uint blockNum) pure public returns (uint) {
      return BTCHeaders[blockNum];
    }
  }


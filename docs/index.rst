Lity
====

This documentation for `Lity <https://github.com/CyberMiles/lity>`_
is still work-in-progress.

Overview
--------

Lity is an open-source language enhanced from Solidity. With new grammar extension, it is highly optimized for enterprise and easy to use for financial industry developers. The flexible interface and unlimited built-in functions can reduce gas cost and accelerate executing time. You can now encrypt large-sized data in the blink of an eye without running out of gas anymore.

How Lity Enhances Security
``````````````````````````

With `ERC Checker <features#erc-checker>`_ and
`Overflow Protection <features#overflow-protection>`_.

How Lity Improves Performance
`````````````````````````````

With `Ethereum Native Interface (ENI) <features#ethereum-native-interface>`_.

Lity by Example
---------------

Here we will provide a contract example and explain a little bit.

.. code:: C++

  pragma solidity ^0.4.23;
  
  contract ReverseContract {
    function reverse(string input) public returns(string) {
      string memory output = eni("reverse", input);
      return output;
    }
  }


Contents
--------

.. toctree::
   :maxdepth: 2

   getting-started
   download
   features
   documentation
   contributing
   news
   events

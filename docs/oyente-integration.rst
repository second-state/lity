Oyente Integration
==================

.. _oyente-integration:

Lity now integrate analysis tool Oyente and generates analysis report after compiling contract if Oyente installed.

Requirements
------------

Install our `lityc <https://github.com/CyberMiles/lity>`_ and `evm <https://github.com/CyberMiles/go-ethereum>`_.

Installation
------------

Execute a python virtualenv

.. code:: bash

  virtualenv -p python3 env
  source env/bin/activate

Download Oyente

.. code:: bash

  git clone https://github.com/CyberMiles/oyente.git
  cd oyente

Install Oyente requirements

.. code:: bash

  pip install -r requirements.txt

Install Oyente

.. code:: bash

  pip install .


Usage
-----

After running :code:`lityc`, it will automatically invoke :code:`oyente` command if installed.

.. code::

  $ cat StringReverse.sol

  pragma lity ^1.2.4;
  contract StringReverse {
    function test() public pure returns (string) {
      string memory ret;
      ret = eni("reverse", "Hello, world");
      return ret;
    }
    function reverse(string src) public pure returns (string) {
      string memory ret;
      ret = eni("reverse", src);
      return ret;
    }
  }

.. code::

  $ lityc --abi StringReverse.sol

  ======= StringReverse.sol:StringReverse =======
  Contract JSON ABI
  [{"constant":true,"inputs":[{"name":"src","type":"string"}],"name":"reverse","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"pure","type":"function"},{"constant":true,"inputs":[],"name":"test","outputs":[{"name":"","type":"string"}],"payable":false,"stateMutability":"pure","type":"function"}]

  INFO:root:contract StringReverse.sol:StringReverse:
  INFO:oyente.symExec:    ============ Results ===========
  INFO:oyente.symExec:      EVM Code Coverage:                     29.8%
  INFO:oyente.symExec:      Parity Multisig Bug 2:                 False
  INFO:oyente.symExec:      Callstack Depth Attack Vulnerability:  False
  INFO:oyente.symExec:      Transaction-Ordering Dependence (TOD): False
  INFO:oyente.symExec:      Timestamp Dependency:                  False
  INFO:oyente.symExec:      Re-Entrancy Vulnerability:             False
  INFO:oyente.symExec:    ====== Analysis Completed ======

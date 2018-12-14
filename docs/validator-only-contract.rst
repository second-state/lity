Validator Only Contract
=======================

.. _validator-only-contract:


.. WARNING::
   Lity supports Validator Only Contracts after lity v1.2.2.


Lity supports Validator Only Contracts after v1.2.2. By providing the built-in function called `isValidator`
and special opcode `OpIsValidator`, Lity can check whether an address is a validator of CyberMiles blockchain.
With this feature, smart contract developers can create trusted smart contracts that can only be modified by the validators.

A common use case is to serve as oracles to provide trusted states.

isValidator Grammar
-------------------

.. code::

  isValidator(<address>) returns (bool returnValue);

  // isValidator is a built-in function provided by Lity.
  // isValidator only takes one parameter, an address, to check whether this address is a validtor.
  // If the address is a validator => return true
  // Otherwise => return false

Examples
--------

BTCRelay
````````

Here we use `BTCRelay <https://github.com/CyberMiles/smart_contracts/tree/master/BTCRelay>`_ contract as an example to show how validator only contract works.
BTCRelay is a contract that store BTC headers and verify BTC transactions.

Download and compile it using ``lityc`` (version >= 1.2.2):

.. code:: bash

  $ wget https://raw.githubusercontent.com/CyberMiles/smart_contracts/master/BTCRelay/BTCRelay.sol
  $ lityc --abi --bin -o output BTCRelay.sol

and then we could deploy it to Travis chain:

.. code:: bash

  $ travis attach http://localhost:8545

  // Prepare accounts

  > validatorAccount = web3.cmt.accounts[0]
  "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc"
  > normalAccount = personal.newAccount('test')
  "0x34bb135d77771038a2fda3aa7cd5dc3fb4cc6abd"
  > personal.unlockAccount(validatorAccount, '1234', 86400)
  true
  > personal.unlockAccount(normalAccount, 'test', 86400)
  true
  > cmt.sendTransaction({from: validatorAccount, to: normalAccount, value: 100000000000000000})

  // Deploy contract

  > abi = <...>
  > bytecode = <...>
  > contract = web3.cmt.contract(abi);
  > btcrelay = contract.new(
    {
      from: web3.cmt.accounts[0],
      data: bytecode,
      gas: "4700000"
    },
    function(e, contract) {
      if (typeof contract.address !== 'undefined') {
        console.log('Contract mined! address: ' + contract.address + ' transactionHash: ' + contract.transactionHash);
      }
    }
  );

  // Contract mined! address: 0x05ea0f73204ea39d7231706a49c174a20380cfec transactionHash: 0x1acbd651d91f1edd3520da59dfda077c9f2ca3bbd9aa09bbc3304ec76a8b41ef

Now we try to call ``storeHeader`` function using both ``validatorAccount`` and ``normalAccount``

.. code:: bash

  > height = 125552
  > header = '0x0x0100000081cd02ab7e569e8bcd9317e2fe99f2de44d49ab2b8851ba4a308000000000000e320b6c2fffc8d750423db8b1eb942ae710e951ed797f7affc8892b0f1fc122bc7f5d74df2b9441a42a14695'

  // Send transaction with validator account

  > tx = btcrelay.storeHeader.sendTransaction(header, height, {from: validatorAccount, gas: "300000"})
  "0xe5d83d5ca82a00b6311ec9b8c0bab4df3f7a62971231791182686ba8f594b7eb"
  > cmt.getTransactionReceipt(tx)
  {
    blockHash: "0x91186a59e8fb19c1db440ab0697266fb3407a10844342278a55e1dcf0eb501e1",
    blockNumber: 72145,
    contractAddress: null,
    cumulativeGasUsed: 140024,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 140024,
    logs: [{
        address: "0x05ea0f73204ea39d7231706a49c174a20380cfec",
        blockHash: "0x91186a59e8fb19c1db440ab0697266fb3407a10844342278a55e1dcf0eb501e1",
        blockNumber: 72145,
        data: "0x00000000000000001e8d6829a8a21adc5d38d0a473b144b6765798e61f98bd1d",
        logIndex: 0,
        removed: false,
        topics: ["0xf82c50f1848136e6c140b186ea0c768b7deda5efffe42c25e96336a90b26c744"],
        transactionHash: "0xe5d83d5ca82a00b6311ec9b8c0bab4df3f7a62971231791182686ba8f594b7eb",
        transactionIndex: 0
    }],
    logsBloom: "0x0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000800000
  00000000000000000000000000000000000000000000001000000000000000100020000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  00000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  000000000000000000000000000000000000000000000000000000",
    status: "0x1",
    to: "0x05ea0f73204ea39d7231706a49c174a20380cfec",
    transactionHash: "0xe5d83d5ca82a00b6311ec9b8c0bab4df3f7a62971231791182686ba8f594b7eb",
    transactionIndex: 0
  }

  // Send transaction with normal account

  > tx = btcrelay.storeHeader.sendTransaction(header, height, {from: normalAccount, gas: "300000"})
  "0xed05e610feae32bd51931b9d0068104dc60a9595590d17b73e1916c2288b0cf9"
  > cmt.getTransactionReceipt(tx)
  {
    blockHash: "0x611671eff48e414706daa66f1fcc404428832dd79c2b6a9481a28f46ef93a430",
    blockNumber: 72173,
    contractAddress: null,
    cumulativeGasUsed: 27466,
    from: "0x34bb135d77771038a2fda3aa7cd5dc3fb4cc6abd",
    gasUsed: 27466,
    logs: [],
    logsBloom: "0x0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  000000000000000000000000000000000000000000000000000000",
    status: "0x0",
    to: "0x05ea0f73204ea39d7231706a49c174a20380cfec",
    transactionHash: "0xed05e610feae32bd51931b9d0068104dc60a9595590d17b73e1916c2288b0cf9",
    transactionIndex: 0
  }

We could see when sending transaction using ``validatorAccount``, the transactions triggered ``storeHeader`` function successfully and emit log.
Log data ``0x00000000000000001e8d6829a8a21adc5d38d0a473b144b6765798e61f98bd1d`` is returned hash of block header.
When sending transaction using ``normalAccount``, it could not pass ``validatorOnly()`` check and failed.

If you want to interact with BTCRelay contract, you could check our
`fetchd <https://github.com/CyberMiles/smart_contracts/tree/master/BTCRelay/fetchd>`_
as an example to fetch and store BTC headers.

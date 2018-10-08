================================================================
New Type for Secure Transaction
================================================================

.. _safeuint-type:

At the begin of 2018, BEC smart contract were hacked because a human error. There is one and only mathematical operation forgot to use SafeMath Library and overflowed. In the Lity, we introduce a new type `safeuint` in the language to help the Smart Contract developer keep the risk of overflow away.

SafeUint Type
-------------

Lity provides a new type `safeuint` which extends from `uint256`, `uint`. `safeuint` is designed for calculating balance of token or any other unsigned integer with overflow protection by default. With this feature, developers don't need to care about how and where to use SafeMath library, because `safeuint` will handle all of the conditions automatically.

Because of the security issue, `safeuint` is not allowed implicit conversion to the other types. If you want to convert, tying to use explicit conversion like the example below:

.. code:: bash

  safeuint a = 2000; // OK: safeuint = 2000
  safeuint b = a * 12345; // OK: safeuint * uint
  // uint c = b; // Error: Implicit conversion is Not Allowed
  uint c = uint(a); // OK: Explicit conversion uses strict type conversion

In addition, `safeuint` will automatically check overflow during runtime on those operations: add, sub, mul. The behavior just like the SafeMath, but the developer could not to add it manually.

Example 1
---------

.. code::

  pragma lity ^1.2.3;

  contract TestContract {
      function c() public pure returns (safeuint) {
          safeuint a = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
          safeuint b = 0x8fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
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

We can compile and deploy it:

.. code::

  # compile

  $ lityc --abi --bin safeuint.sol

  ======= safeuint.sol:C =======
  Binary:
  608060405234801561001057600080fd5b5061011e806100206000396000f300608060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff168063c3da42b8146044575b600080fd5b348015604f57600080fd5b506056606c565b6040518082815260200191505060405180910390f35b60008060007f7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff91507f8fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff90508082801560ea5781817fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff04101560e957fe5b5b0292505050905600a165627a7a7230582017271d4cc5b2d560c9ad2c74bef545907de9008f50d1e660b12f1d2bc32d6fdc0029

  Contract JSON ABI
  [{"constant":true,"inputs":[],"name":"c","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"pure","type":"function"}]

  # prepare deploy.js

  $ cat deploy.js

  bytecode = "0x608060405234801561001057600080fd5b5061011e806100206000396000f300608060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff168063c3da42b8146044575b600080fd5b348015604f57600080fd5b506056606c565b6040518082815260200191505060405180910390f35b60008060007f7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff91507f8fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff90508082801560ea5781817fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff04101560e957fe5b5b0292505050905600a165627a7a7230582017271d4cc5b2d560c9ad2c74bef545907de9008f50d1e660b12f1d2bc32d6fdc0029"
  abi = [{"constant":true,"inputs":[],"name":"c","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"pure","type":"function"}]
  var testContract = web3.cmt.contract(abi);
  var test = testContract.new({
    from: web3.cmt.accounts[0],
    data: bytecode,
    gas: '4700000'
  }, function (e, contract){
    if (typeof contract.address !== 'undefined') {
      console.log('Contract mined! address: ' + contract.address + ' transactionHash: ' + contract.transactionHash);
    }
  })

  # deploy

  $ travis attach http://localhost:8545

  > personal.unlockAccount(cmt.accounts[0], '<YOUR_PASSWORD>')
  true
  > loadScript('deploy.js')
  true
  Contract mined! address: 0xab119259ff325f845f8ce59de8ccf63e597a74cd transactionHash: 0x2c0efff8fa628dfb3f90132d54c1326059d0ad65d938ffd97155282ae94f6d10

  # Overflow
  > test.c()
  0

  # The failed transaction consumes all gas
  > test.c.sendTransaction({from:cmt.accounts[0], gas:12345678})
  "0x8aaef895147ac9d31058eadbff7ee1a65e8adebb626073cd4ed4755b9feb0682"
  > cmt.getTransactionReceipt("0x8aaef895147ac9d31058eadbff7ee1a65e8adebb626073cd4ed4755b9feb0682")
  {
    blockHash: "0xff98005eed6c33698954b7de1df12fdae56670eff913e182df223daad9769a99",
    blockNumber: 90,
    contractAddress: null,
    cumulativeGasUsed: 12345678,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 12345678,
    logs: [],
    logsBloom: "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    status: "0x0",
    to: "0xb6b29ef90120bec597939e0eda6b8a9164f75deb",
    transactionHash: "0x8aaef895147ac9d31058eadbff7ee1a65e8adebb626073cd4ed4755b9feb0682",
    transactionIndex: 0
  }

Example 2
---------

Here is another ``safeuint`` example to multiply two numbers.

.. code::

  pragma lity ^1.2.3;

  contract TestContract {
      function useSafeuint(safeuint _amountA, safeuint _amountB) public pure returns (safeuint) {
          return _amountA * _amountB;
      }
  }

Then compile and deploy it:

.. code::

  $ lityc --abi --bin safeuint.sol

  Compile it and deploy it:

  ======= safeuint.sol:TestContract =======
  Binary:
  608060405234801561001057600080fd5b5060f68061001f6000396000f300608060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680634eea8a05146044575b600080fd5b348015604f57600080fd5b5060766004803603810190808035906020019092919080359060200190929190505050608c565b6040518082815260200191505060405180910390f35b60008183801560c15781817fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff04101560c057fe5b5b029050929150505600a165627a7a72305820066af7ab07adfbcc343ad93808581baa29a4b4b6523d7117ae3321f5f3ee82e60029

  Contract JSON ABI
  [{"constant":true,"inputs":[{"name":"_amountA","type":"uint256"},{"name":"_amountB","type":"uint256"}],"name":"useSafeuint","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"pure","type":"function"}]

  $ travis attach http://localhost:8545

  > personal.unlockAccount(cmt.accounts[0], '<YOUR_PASSWORD>')
  true

  > bytecode = "0x608060405234801561001057600080fd5b5060f68061001f6000396000f300608060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680634eea8a05146044575b600080fd5b348015604f57600080fd5b5060766004803603810190808035906020019092919080359060200190929190505050608c565b6040518082815260200191505060405180910390f35b60008183801560c15781817fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff04101560c057fe5b5b029050929150505600a165627a7a72305820066af7ab07adfbcc343ad93808581baa29a4b4b6523d7117ae3321f5f3ee82e60029"
  > abi = [{"constant":true,"inputs":[{"name":"_amountA","type":"uint256"},{"name":"_amountB","type":"uint256"}],"name":"useSafeuint","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"pure","type":"function"}]
  > var testContract = web3.cmt.contract(abi);
  > var test = testContract.new({
  ..   from: web3.cmt.accounts[0],
  ..   data: bytecode,
  ..   gas: '4700000'
  .. }, function (e, contract){
  ..   if (typeof contract.address !== 'undefined') {
  ..     console.log('Contract mined! address: ' + contract.address + ' transactionHash: ' + contract.transactionHash);
  ..    }
  .. })

  > test.useSafeuint(2, 2)
  4

  # The normal transaction with normal gas usage
  > tx = test.useSafeuint.sendTransaction(2, 2, {from: cmt.accounts[0], gas:12345678})
  "0xd65ddf51d6ad0b83b2fd15816e06d97196bd22afa93371771c497191a82d03ef"
  > cmt.getTransactionReceipt(tx)
  {
    blockHash: "0xf9736e603d27aedd5abfa1eddd12a3d6676103b2c143c7cc074bc1815ffe221c",
    blockNumber: 310,
    contractAddress: null,
    cumulativeGasUsed: 22010,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 22010,
    logs: [],
    logsBloom: "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    status: "0x1",
    to: "0xab119259ff325f845f8ce59de8ccf63e597a74cd",
    transactionHash: "0xd65ddf51d6ad0b83b2fd15816e06d97196bd22afa93371771c497191a82d03ef",
    transactionIndex: 0
  }

  # Overflow
  > test.useSafeuint(2, 0x8000000000000000000000000000000000000000000000000000000000000000)
  0

  # The failed transaction consumes all gas
  > tx = test.useSafeuint.sendTransaction(2, 0x8000000000000000000000000000000000000000000000000000000000000000, {from: cmt.accounts[0], gas:12345678})
  "0xdc50aac3b91ed6b69df2ec4f891866b9dbe73c361f1e00887c45f9cc2f00caf9"
  > cmt.getTransactionReceipt(tx)
  {
    blockHash: "0x090c97f1bec942caf803a3b5cbc4723df5f6c0c669f468f69367093de73e1140",
    blockNumber: 315,
    contractAddress: null,
    cumulativeGasUsed: 12345678,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 12345678,
    logs: [],
    logsBloom: "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    status: "0x0",
    to: "0xab119259ff325f845f8ce59de8ccf63e597a74cd",
    transactionHash: "0xdc50aac3b91ed6b69df2ec4f891866b9dbe73c361f1e00887c45f9cc2f00caf9",
    transactionIndex: 0
  }

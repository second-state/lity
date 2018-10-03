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

Example
-------

.. code::

  //pragma solidity ^0.4.22;
  pragma lity ^1.2.0;

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
  > test.c()
  0

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

.. code:: ts

  isValidator(<address>) returns (bool returnValue);

  // isValidator is a built-in function provided by Lity.
  // isValidator only takes one parameter, an address, to check whether this address is a validtor.
  // If the address is a validator => return true
  // Otherwise => return false


Compile isValidator Contract
----------------------------

There is a simple contract that leverage isValidator to grant validators to modify states.

.. code:: ts

  // pragma lity ^1.2.2;
  //pragma solidity ^0.4.24;
  pragma lity ^1.2.0;

  contract ValidatorOnlyContractExample {
    uint[] BTCHeaders;
    modifier ValidatorOnly() {
      require(
        isValidator(msg.sender),
        "Only validator can use this function."
      );
      _;
    }

    function pushBTCHeader(uint blockHash) public ValidatorOnly returns (uint headersLength) {
      BTCHeaders.push(blockHash);
      headersLength = BTCHeaders.length;
    }

    function getBTCHeader(uint blockNum) view public returns (uint) {
      return BTCHeaders[blockNum];
    }

    function getBTCHeaders() view public returns (uint[]) {
      return BTCHeaders;
    }
  }


and we could compile it using `lityc` (version >= 1.2.2):

.. code:: ts

  $ mkdir output
  $ ./lityc --abi --bin -o output ValidatorOnlyContractExample.sol
  $ cat output/ValidatorOnlyContractExample.abi
  [{"constant":false,"inputs":[{"name":"blockHash","type":"uint256"}],"name":"pushBTCHeader","outputs":[{"name":"headersLength","type":"uint256"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"getBTCHeaders","outputs":[{"name":"","type":"uint256[]"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[{"name":"blockNum","type":"uint256"}],"name":"getBTCHeader","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"}]
  $ cat output/ValidatorOnlyContractExample.bin
  608060405234801561001057600080fd5b506102dd806100206000396000f300608060405260043610610057576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680636537eff61461005c578063bae9203d1461009d578063e415df9e14610109575b600080fd5b34801561006857600080fd5b506100876004803603810190808035906020019092919050505061014a565b6040518082815260200191505060405180910390f35b3480156100a957600080fd5b506100b2610237565b6040518080602001828103825283818151815260200191508051906020019060200280838360005b838110156100f55780820151818401526020810190506100da565b505050509050019250505060405180910390f35b34801561011557600080fd5b506101346004803603810190808035906020019092919050505061028f565b6040518082815260200191505060405180910390f35b60003373ffffffffffffffffffffffffffffffffffffffff16f615156101fe576040517f08c379a00000000000000000000000000000000000000000000000000000000081526004018080602001828103825260258152602001807f4f6e6c792076616c696461746f722063616e2075736520746869732066756e6381526020017f74696f6e2e00000000000000000000000000000000000000000000000000000081525060400191505060405180910390fd5b60008290806001815401808255809150509060018203906000526020600020016000909192909190915055506000805490509050919050565b6060600080548060200260200160405190810160405280929190818152602001828054801561028557602002820191906000526020600020905b815481526020019060010190808311610271575b5050505050905090565b6000808281548110151561029f57fe5b906000526020600020015490509190505600a165627a7a723058200785a80032fa88b662ace5fdae019671d7b1807c904adf4500b68af6cc1edaaf0029


Deploy contract to Travis
-------------------------

After we get contract ABI and bytecode, we could deploy it to Travis chain.


.. code:: bash

  # Get Travis console
  travis attach http://127.0.0.1:8545

  # Deploy contract (in Travis console)
  personal.unlockAccount(cmt.accounts[0], '1234');
  abi = [{"constant":false,"inputs":[{"name":"blockHash","type":"uint256"}],"name":"pushBTCHeader","outputs":[{"name":"headersLength","type":"uint256"}],"payable":false,"stateMutability":"nonpayable","type":"function"},{"constant":true,"inputs":[],"name":"getBTCHeaders","outputs":[{"name":"","type":"uint256[]"}],"payable":false,"stateMutability":"view","type":"function"},{"constant":true,"inputs":[{"name":"blockNum","type":"uint256"}],"name":"getBTCHeader","outputs":[{"name":"","type":"uint256"}],"payable":false,"stateMutability":"view","type":"function"}]
  bytecode = "0x608060405234801561001057600080fd5b506102dd806100206000396000f300608060405260043610610057576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680636537eff61461005c578063bae9203d1461009d578063e415df9e14610109575b600080fd5b34801561006857600080fd5b506100876004803603810190808035906020019092919050505061014a565b6040518082815260200191505060405180910390f35b3480156100a957600080fd5b506100b2610237565b6040518080602001828103825283818151815260200191508051906020019060200280838360005b838110156100f55780820151818401526020810190506100da565b505050509050019250505060405180910390f35b34801561011557600080fd5b506101346004803603810190808035906020019092919050505061028f565b6040518082815260200191505060405180910390f35b60003373ffffffffffffffffffffffffffffffffffffffff16f615156101fe576040517f08c379a00000000000000000000000000000000000000000000000000000000081526004018080602001828103825260258152602001807f4f6e6c792076616c696461746f722063616e2075736520746869732066756e6381526020017f74696f6e2e00000000000000000000000000000000000000000000000000000081525060400191505060405180910390fd5b60008290806001815401808255809150509060018203906000526020600020016000909192909190915055506000805490509050919050565b6060600080548060200260200160405190810160405280929190818152602001828054801561028557602002820191906000526020600020905b815481526020019060010190808311610271575b5050505050905090565b6000808281548110151561029f57fe5b906000526020600020015490509190505600a165627a7a723058200785a80032fa88b662ace5fdae019671d7b1807c904adf4500b68af6cc1edaaf0029"
  contract = web3.cmt.contract(abi);
  c = contract.new(
    {
      from: web3.cmt.accounts[0],
      data: bytecode,
      gas: "4700000"
    },
    function(e, contract) {
      if (contract.address) {
        console.log("contract address: " + contract.address);
        console.log("transactionHash: " + contract.transactionHash);
      }
    }
  );

Use contract to test the functionality of isValidator
-----------------------------------------------------

.. code:: bash

  # Create a normal account, the return address will be different in your environment. Don't worry.
  > newAccount = personal.newAccount("6666")
  "0x52c3b25530e7c14b9c19eefc757fcfce3ef48316"

  # Validator sends some cmt to the new account
  > cmt.sendTransaction({from: cmt.account[0], to: newAccount, value: web3.toWei(1, "cmt")})

  # Check whether the new account receives 1 cmt.
  > cmt.getBalance(newAccount)
  1000000000000000000

  # At first, the BTCHeaders is empty
  > c.getBTCHeaders.call()
  []

  # newAccount sends pushBTCHeader()
  > tx_push_should_not_pass = c.pushBTCHeader.sendTransaction(0x0000000000000000000d87d1114bd39151486cdbfa2635ba8645791df0a0afd1, {from: newAccount, gas: "4700000"})
  "0x8c3dc24ac03bdfbf0529a7a3a98cb7e598c7061062fd1688f099a944f8badb68"
  > cmt.getTransactionReceipt(tx_push_should_not_pass)
  {
    blockHash: "0x1dd34d641b0c6355cef9f3ba6ec9e14520fd069c60fdd3afa37fcbb09d9a1420",
    blockNumber: 575,
    contractAddress: null,
    cumulativeGasUsed: 22906,
    from: "0xd6a7662eb9a49f2ad96c4f82f526d49fb988ce68",
    gasUsed: 22906,
    logs: [],
    logsBloom: "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    status: "0x0",
    to: "0xccad3cb4b2a2cb917b2c6eb4025c78d4cadeb804",
    transactionHash: "0x8c3dc24ac03bdfbf0529a7a3a98cb7e598c7061062fd1688f099a944f8badb68",
    transactionIndex: 0
  }
  # Check the BTCHeaders is empty
  > c.getBTCHeaders.call()
  []

  # Validator sends pushBTCHeader()
  # BTC Block #542210 (hash = 0x0000000000000000000d87d1114bd39151486cdbfa2635ba8645791df0a0afd1)
  > tx_push_should_pass = c.pushBTCHeader.sendTransaction(0x0000000000000000000d87d1114bd39151486cdbfa2635ba8645791df0a0afd1, {from: cmt.accounts[0], gas: "4700000"})
  "0x0bbe6ace1c23d13fb8b4604398c793591c62fd68e38f1752d1ad38153f939e5c"
  > cmt.getTransactionReceipt(tx_push_should_pass)
  {
    blockHash: "0x08aef08ca19b97b4cfe0fae32ac9bf4369a7290d4e60691ee69f800cec055784",
    blockNumber: 586,
    contractAddress: null,
    cumulativeGasUsed: 63385,
    from: "0x7eff122b94897ea5b0e2a9abf47b86337fafebdc",
    gasUsed: 63385,
    logs: [],
    logsBloom: "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    status: "0x1",
    to: "0xccad3cb4b2a2cb917b2c6eb4025c78d4cadeb804",
    transactionHash: "0x0bbe6ace1c23d13fb8b4604398c793591c62fd68e38f1752d1ad38153f939e5c",
    transactionIndex: 0
  }
  # Check the BTCHeaders has one entry
  > c.getBTCHeaders.call()
  [1.295967676125722e+54]
  > c.getBTCHeader.call(0)
  1.295967676125722e+54

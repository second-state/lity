Features Overview
=================

Better Security
---------------

ERC Checker
```````````

Lity can check if your contract's interface is compatible to specific ERC standards, please refer to :doc:`ERC contract checker guide <erc-contract-checker-guide>`


Overflow Protection
```````````````````

In order to prevent crypto token leak incident like `BeautyChain(BEC) <https://medium.com/cybermiles/27c96a7e78fd>`_,
Lity offers automatic overflow free guarantee by Lity compiler or Lity's Ethereum virtual machine(EVM).
In either solution, an integer overflow would cause the contract execution fail.
So contract programmers do not need to worry about overflow detection anymore.

See :ref:`here <overflow-protection>` for more details.

More Flexibility
----------------

Provide more flexibility for your EVM with

* `Ethereum Native Interface`_.
* :ref:`Ethereum Virtual Machine C Interface <vm-wrapper>`.

Ethereum Native Interface
`````````````````````````
Conventional Smart contracts are compiled to Ethereum byte codes and then executed on EVM.
This works well for simple smart contracts.
However, for complex contracts such as RSA encrytion/decryption,
it is too slow and costs too much gas so that many applications are not feasible.
Therefore, we develop a native interface for EVM, called Ethereum Native Interface(ENI).

Smart contracts can call native libraries to speed up execution and save much gas.
So performance bottleneck due to EVM can be resolved.
Native library are installed with EVM and updated over the air(OTA).
See here for more details.

Note that new ENI operations are added without recompiling EVM, rather than pre-compiled contracts.

Business Rule Engine
--------------------

Lity Rule Language
``````````````````

Lity incorporates rule language(like Drools and Jess) into smart contract.
The rules engine shall match patterns(facts) to rules, and resolve potential conflicts.
See Lity rule language for reference. Lity's rule grammar is very similar with Drools.

This feature is compatible with official Ethereum Virtual Machine.
Because, Lity directly compiles rule language into official EVM byte code.

Check :doc:`the rule engine guide <rule-engine-guide>` for how to use Lity's rule engine.


What's Special About Lity
=========================

Better Security
---------------

Make your contract more secure with

* `ERC Checker`_, and
* `Overflow Protection`_.

Check `here <https://medium.com/cybermiles/27c96a7e78fd>`_ for a real-world
security case happened on the BeautyChain (BEC).

ERC Checker
```````````

.. TODO

Overflow Protection
```````````````````

In order to prevent crypto token leak incident like `this <https://medium.com/cybermiles/27c96a7e78fd>`_, 
Lity offers automatic overflow free guarantee by Lity compiler or Lity's Ethereum virtual machine(EVM).
In either solution, an integer overflow would cause the contract execution fail.
So contract programmers do not need to worry about overflow detection anymore.

See here for more details.

More Flexibility
----------------

Provide more flexibility for your EVM with

* `Ethereum Native Interface`_.

Ethereum Native Interface
`````````````````````````
Conventional Smart contracts are compiled to Ethereum byte codes and then executed on EVM.
This works well for simple smart contracts.
However, for complex contracts such as RSA encrytion/decryption, it is too slow and costs too much gas so that many applications are not feasible.
Therefore, we develop a native interface for EVM, called Ethereum Native Interface(ENI).

Smart contracts can call native libraries to speed up execution and save much gas.
So performance bottleneck due to EVM can be resolved.
Native library are installed with EVM and updated over the air(OTA).
See here for more details.

Note that new ENI operations are added without recompiling EVM, rather than pre-compiled contracts.

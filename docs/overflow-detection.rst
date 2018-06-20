===============
Overflow Detection
===============

In order to prevent crypto token leak incident like `this <https://medium.com/cybermiles/27c96a7e78fd>`_, 
Lity provides two approaches: improving compiler or Ethereum virtual machine(EVM). 
The first approach converts every integer operation to SafeMath operations at compile time. 
The Second approach modifies EVM so that overflow is detected at runtime.

Lity Compiler
---------------
Lity automatically converts integer operations to SafeMath operations, 
so any overflow would cause a contract execution fail.
Compiler options could be set mannualy to enable/disable this feature.

Lity's Ethereum Virtual machine
---------------
While executes a smart contract, Lity's EVM analyzes each integer operation.
When an overflow occurs, it will terminate the contract execution and report an error.

Currently, three arithmetic operations are overflow checked: ADD, SUB and MUL.
And overflow are checked only for 32-byte integers.

Since signed and unsigned overflow must checked in a different method, 
each of those integer operations(ADD, SUB and MUL) divides into a signed operation and an unsigned operation.
We see original operations(ADD, SUB and MUL) as unsigned ones and new operations(SADD, SSUB and SMUL) as signed one.

Note that this approach only applies for Lity's version of compiler and EVM.

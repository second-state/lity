===============
Overflow Detection
===============

In order to prevent crypto token leak incident like this, Lity adds runtime overflow detection to Ethereum virtual machine.(EVM)
Lity's EVM analyzes each integer operation. When overflow occurs, it will terminate the contract execution and report an error.
Currently, three arithmetic operations are overflow checked: ADD, SUB and MUL.
Since signed and unsigned overflow must checked in a different method, 
we see original ADD, SUB and MUL as unsigned ones and add new SADD, SSUB and SMUL as signed one.
These operations with overflow detection applies for 32-byte integers.

Note that this feature only applies with Lity's version of compiler and Ethereum virtual machine.

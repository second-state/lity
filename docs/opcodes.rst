VM Opcodes
==========


Overview
--------
Lity's EVM is a stack-based, big-endian VM with a word size of 256-bits and is used to run the smart contracts on the blockchain.
Each opcode is encoded as one byte.


Arithmetic
----------
* integer : ADD, MUL, SUB
* fixed-point : UFMUL, SFMUL, UFDIV, SFDIV

The values are popped from the operand stack, 
and the result will be pushed back to the stack.
Opcode starts with :code:`S` is a signed operation, and :code:`U` is an unsigned operation.


All arithmetic operations are overflow-checked.
An overflow would cause the contract execution fail. 
See :doc:`overflow protection <overflow-protection>` for more details.


ISVALIDATOR
-----------
This operation will check whether an address popped from the stack is a validator of CyberMiles blockchain,
and the result will be pushed back to the stack.
With this feature, smart contract developers can make a contract modifiable only by the validators.

See :doc:`Validator Only Contract <validator-only-contract>` for more details.


FREEGAS
-------
This operation allows developers to use contract balance to pay the transaction fee for the users.
See :doc:`Freegas-Let contract owner pay the tx fee for you <freegas>` for more details.


RAND
----
This operation will use random seed, nonce, code hash, and a counter number to generate a random number. 
After :code:`RAND` is called, the counter will automaticlly increase. The counter number depends on the the called time of :code:`RAND`. 
Even if transations are in the same code block, we can still receive different results. 

See :doc:`Rand-Get random number on the chain <rand>` for more details.


Opcodes
-------

+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| OpCode              | uint8        | Stack Input      | Stack Output               | Expression                | Notes                                                  |
+=====================+==============+==================+============================+===========================+========================================================+
| :code:`ADD`        | :code:`0x01` | :code:`[a|b|$`   | :code:`[a+b|$`             | a + b                     | unsigned integer addition                              |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`MUL`        | :code:`0x02` | :code:`[a|b|$`   | :code:`[a*b|$`             | a * b                     | unsigned integer multiplication                        |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`SUB`        | :code:`0x03` | :code:`[a|b|$`   | :code:`[a-b|$`             | a - b                     | unsigned integer substraction                          |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`UFMUL`       | :code:`0x2a` | :code:`[a|b|N|$` | :code:`[a*b/pow(10,N)|$`   | A [1]_ * B [2]_           | unsigned fixed-point multiplication                    |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`SFMUL`       | :code:`0x2b` | :code:`[a|b|N|$` | :code:`[a*b/pow(10,N)|$`   | A [1]_ * B [2]_           | signed fixed-point multiplication                      |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`UFDIV`       | :code:`0x2c` | :code:`[a|b|N|$` | :code:`[a*pow(10,N)/b|$`   | A [1]_ / B [2]_           | unsigned fixed-point division                          |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`SFDIV`       | :code:`0x2d` | :code:`[a|b|N|$` | :code:`[a*pow(10,N)/b|$`   | A [1]_ / B [2]_           | signed fixed-point division                            |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`ENI`         | :code:`0xf5` |                  |                            |                           |                                                        |  
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`ISVALIDATOR` | :code:`0xf6` | :code:`[addr|$`  | :code:`[boolean|$`         | isValidator(addr)         | make a contract modifiable only by the validators      |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`SCHEDULE`    | :code:`0xf7` |                  |                            |                           |                                                        |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`FREEGAS`     | :code:`0xf8` | :code:`[|$`      | :code:`[|$`                | function modifier freegas | let contract owner pay the tx fee for you              |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+
| :code:`RAND`        | :code:`0xf9` | :code:`[|$`      | :code:`[keccak256|$`       | rand()                    | get the random number on the chain                     |
+---------------------+--------------+------------------+----------------------------+---------------------------+--------------------------------------------------------+

.. [1] A = a * pow(10,N) 
.. [2] B = b * pow(10,N)
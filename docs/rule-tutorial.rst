Business Rule Engine
====================

.. _rule-tutorial:

Lity incorporates rule language(like Drools and Jess) into smart contract.
The rules engine shall match patterns(facts) to rules, and resolve potential conflicts.
See Lity rule language for reference. Lity's rule grammar is very similar with Drools.

This feature is compatible with official Ethereum Virtual Machine.
Because, Lity directly compiles rule language into official EVM byte code.


Rule Grammar
------------

Rule
````

An rule statement contains three parts:

1. Rule Name: a string literal to represent rule declaration.
2. Filter Statements (a.k.a. *when block*): one or more statements describe what kind of object should be captured and apply action in the *then block*.
3. Action Statements (a.k.a. *then block*): one or more statements to apply action on these objects which are captured in *when block*.


.. code:: ts

    rule "ruleName" when {
        // Filter Statement
    } then {
        // Action Statement
    }


Fact
````

And we have three operators to handle facts:

1. factInsert: add current object as a new fact to the monitor list.
2. factDelete: remove current object from the monitor list.
3. fireAllRules: emit all rules and apply them on the relative facts.

.. code:: ts

    lvalue = factInsert object;
    factDelete lvalue;
    fireAllRules;


Example
```````

Here we start from a contract for age pension as first example.

.. code:: ts

    // pragma lity ^1.2.0;

    contract AgePension {
        struct Person {
            int age;
            bool eligible;
            address addr;
        }

        mapping (address => uint256) addr2idx;
        Person[] ps;

        function addPerson(int age) public {
            ps.push(Person(age, true, msg.sender));
            addr2idx[msg.sender] = factInsert ps[ps.length-1];
        }

        function deletePerson() public {
            factDelete addr2idx[msg.sender];
        }
    }

Whenever a user add themselves to ``AgePension`` with ``addPerson``,
they are also recorded to the set of fact by ``factInsert``.
The operator ``factInsert`` will return an ``uint256`` as the storage location
for the fact in the working memory.
The user will be able to remove themselves from ``AgePension`` with
``deletePerson`` by passing the storage location to ``factDelete`` operator.

.. code:: ts

    // pragma lity ^1.2.0;

    contract AgePension {
        struct Person {
            int age;
            bool eligible;
            address addr;
        }

        // ...

        rule "oldAgePension" when {
            p: Person(age > 65, eligible);
        } then {
            p.addr.transfer(1 ether);
            p.eligible = false;
        }
    }

Next, we add a ``rule "oldAgePension"`` that gives everyone more than age 65
one ether if they haven't received age pension yet.

.. code:: ts

    // pragma lity ^1.2.0;

    contract AgePension {
        function pay() public {
            fireAllRules;
        }

        function () public payable { }
    }

The age pension is paid when ``fireAllRules`` is executed.

Lity Rule by Examples
=====================

Here we start from a contract for age pension.

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

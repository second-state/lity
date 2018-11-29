Rule Engine Guide
=================

.. _rule-engine-guide:

Introduction
------------

In industry, people often use `business rule engine <https://en.wikipedia.org/wiki/Business_rules_engine>`_ to maintain their business logics and knowledges. Rule engine can detect sepicific conditions and contradiction and execute predefined rules to fix them. Some examples of rule engines are `Jess <https://www.jessrules.com>`_ and `Drools <https://www.drools.org/>`_.

Despite the wide adoption, it is hard to run the rule engine in a publicly verifiable way (which means to run it on Blockchain). To eliminate the gap between traditional business rule engine and Blockchain technology, we decide to implement part of traditional rule engine features (to make it part of Lity). Rule engine related codes are directly compiled into EVM byte codes, so it can also be executed on Ethereum public chain.

Our rule engine's syntax and semantics are directly borrowed from Drools, so it might be a good idea to look through `Drools documentation <https://www.drools.org/learn/documentation.html>`_ before you use Lity's rule engine.
Specifically, chapter `Rule Engines and Production Rule Systems (PRS) <https://docs.jboss.org/drools/release/7.1.0.Final/drools-docs/html_single/#_rule_engines_and_production_rule_systems_prs>`_ introduces the basic concept.

Rule Engine Overview
--------------------

Facts and Working Memory
""""""""""""""""""""""""
Facts are data that shall be matched and modified by the rule engine.
In Lity, a fact must be a struct stored in storage.

Working memory is a container that stores facts hides behind a contract. To insert/remove facts to/from working memory, we can use ``factInsert`` and ``factDelete`` operators.

Rules
"""""

A rule defines that ``when`` certain conditions occur, ``then`` certain actions should be executed.

In Lity, rules are written in the contract and the syntax is very similar with Drools's.

A rule statement consists of four parts:

1. Rule Name: a string literal which served as the identifier of rule.
2. Rule attributes: optional hints that describe activation behavior of this rule.
3. Filter Statements (a.k.a. *when block*): one or more conditional statements describe which set of facts should be captured.
4. Action Statements (a.k.a. *then block*): one or more statements to execute on matched objects (which are captured in *when block*.)

A contract with a rule definition looks like this:

.. code:: ts

    contract C {
        rule "ruleName"
        // Rule attributes
        when {
            // Filter Statements
        } then {
            // Action Statements
        }
    }

Rule Attributes
~~~~~~~~~~~~~~~

salience
********
default: 0

type: integer literal

Salience specifies the priority of rules in the Activation queue.
Higher salience indicates higher priority.
Activation order of rules with the same salience would be arbitrary.

In constrast to Drools, Lity does not support dynamic salience.

Due to Solidity parser issue, current salience value cannot be negative, but this shall be resolved in the future.

no_loop
*******
default: false

type: bool literal

``no_loop`` forbids a rule to activate itself with the same set of facts.
This is for the purpose of preventing infinite loop.

lock_on_active
**************
default: false

type: bool literal

``lock_on_active`` forbids a rule to be activated more than once with the same set of facts.
This is stronger than ``no_loop`` because it also prevent the reactivation of the rule even if it is caused by other rule's then-part.

Filter Statements(when)
~~~~~~~~~~~~~~~~~~~~~~~
When part is composed of patterns (which are explained later).
When part specifies conditions which set of facts should be activated.
If all pattern conditions are met, then part shall be executed for this set of facts.

Pattern
*******
A pattern describe a fact (struct) with a set of conditions.
It start with pattern binding, which specifies fact identifier refered in this rule scope.
After binding, pattern type specifies the type (struct name) of the fact.
Then, a set of constraints is descibe conditions of this fact.
Constraints must be boolean expressions.
See the example below or refer rule grammar for details.

.. code:: ts

    p: Person(age >= 65, eligible == true);

Above pattern describe that a fact ``p`` with Person type, and its constraints are its ``age`` must be greater or equal to ``65`` and its ``eligible`` must be ``true``.

Action Statements(then)
~~~~~~~~~~~~~~~~~~~~~~~
Then part is composed of normal statements.
However, there is a special operator, ``update`` (explained later), which might be useful in this part.

Due to Solidity compiler issue, variable declaration statement is not supported yet in then block.
But this shall be resolved in the future.

The update operator
*******************
``update object`` will inform the rule engine that this object may be modified and rules may need to be reevaluated.
In current implementation, all rules and facts are reevaluated even for the objects that was not updated.
So conditions should be taken care when ``update`` is used in any rule.

A simple Example
~~~~~~~~~~~~~~~~
Let's start with a simple example to explain how a rule works.
This example pays Ether to old people.

.. code:: ts

   rule "payPension" when {
     p: Person(age >= 65, eligible == true);
     b: Budget(amount >= 10);
   } then {
     p.addr.transfer(10);
     p.eligible = false;
     b.amount -= 10;
   }

Above is a rule which pays money to old people if the budget is still enough.
``payPension`` is the identifier of the rule.
There are two patterns in this rule: ``p`` and ``b`` .
``Person(age >= 65, eligible == true)`` describes a person who is at least 65 years old and is eligible for receiving the pension.
The ``p:`` syntax means to bind the qualified person to identifier ``p``, so we can refer the person in then-block.
``b: Budget(amount >= 10)`` describes the budget must have enough amount. (``10`` in this case)

If the rule engine found a person and a budget satisfies above requirements, the then part will be executed.
In then part, we modify eligiblity of the person to prevent this rule being applied for the same person again.
In addition, pension is sent to the person from the budget.

For full source code of this example, refer the section Rule Examples.

Rule Inheritance
~~~~~~~~~~~~~~~~
Rules can also be inherited with attributes.
This only applies for when part and only supports single inheritance.
Refer to section Rule Examples for details.

Rule Examples
-------------
This section illustrates more use cases for Lity rule engine.

Pay Pension
"""""""""""

This example has already been described in section Rules.
The complete contract is below.

.. code:: ts

    contract AgePension {
        struct Person {
            int age;
            bool eligible;
            address addr;
        }

        struct Budget {
            int amount;
        }

        mapping (address => uint256) addr2idx;
        Person[] ps;
        Budget budget;

        constructor () public {
            factInsert budget;
            budget.amount = 100;
        }

        function addPerson(int age) public {
            ps.push(Person(age, true, msg.sender));
            addr2idx[msg.sender] = factInsert ps[ps.length-1];
        }

        function deletePerson() public {
            factDelete addr2idx[msg.sender];
        }

        function pay() public {
            fireAllRules;
        }

        function () public payable { }

        rule "payPension" when {
            p: Person(age >= 65, eligible == true);
            b: Budget(amount >= 10);
        } then {
            p.addr.transfer(10);
            p.eligible = false;
            b.amount -= 10;
        }
    }

A user must use ``factInsert`` add himself (an instance of ``Person``) in order to make the rule engine aware of this data. (written in function ``addPerson``)
The operator ``factInsert`` returns an ``uint256``.
This is where the fact resides in the storage, and this address is recorded in mapping ``addr2idx``.
The user will be able to remove himself from the engine by ``factDelete`` with the fact storage address. (written in function ``deletePerson``)

The rule ``payPension`` decribes that gives everyone more than age 65. (already explained in section Rules)

The age pension is paid when ``fireAllRules`` is executed. (written in function ``pay``)
``fireAllRules`` triggers the rules to find matched rules and apply then part for the corresponding facts.

Fibonacci numbers
"""""""""""""""""

Here we demostrate how to use rule engine to calculate fibonacci numbers.

First, we define a struct to represent a fibonacci number:

.. code:: ts

  struct E {
      int256 index;
      int256 value;
  }


The struct has two members. ``index`` records the index of this fibonacci number, and ``value`` records the value of the fibonacci number. If the ``value`` is unknown, we set it to ``-1``.

We can now define a rule representing fibonacci number's recurrence relation: :math:`f_n = f_{n-1} + f_{n-2}`.

.. code:: ts

    rule "buildFibonacci" when {
        x1: E(value != -1, i1: index);
        x2: E(value != -1, index == i1+1, i2: index);
        x3: E(value == -1, index == i2+1);
    } then {
        x3.value = x1.value+x2.value;
        update x3;
    }

Note that the ``update x3;`` inside rule's RHS is essential; the update statement informs the rule engine that the value of ``x3`` has been updated, and all future rule match should not depend on the old value of it.

Let's insert initial terms and unknown fibonacci numbers into working memory

.. code:: ts

   // es is a storage array storing `E`
   es.push(E(0, 0));
   factInsert es[es.length - 1];
   es.push(E(1, 1));
   factInsert es[es.length - 1];
   for (int i = 2 ; i < 10 ; i++) {
       es.push(E(i, -1));
       factInsert es[es.length - 1];
   }

Working memory now contains :math:`f_0`, :math:`f_1`, ... , :math:`f_{10}`. And only :math:`f_0` and :math:`f_1`'s value are known. We can now use ``fireAllRules`` statement to start the rule engine, and all fibonacci numbers should be calculated accordingly.

Complete source of the contract:

.. code:: ts

  contract C {
      struct E {
          int256 index;
          int256 value;
      }

      rule "buildFibonacci" when {
          x1: E(value != -1);
          x2: E(value != -1, index == x1.index+1);
          x3: E(value == -1, index == x2.index+1);
      } then {
          x3.value = x1.value+x2.value;
          update x3;
      }

      E[] es;

      constructor() public {
          es.push(E(0, 0));
          factInsert es[es.length - 1];
          es.push(E(1, 1));
          factInsert es[es.length - 1];
          for (int i = 2 ; i < 10 ; i++) {
              es.push(E(i, -1));
              factInsert es[es.length - 1];
          }
      }

      function calc() public returns (bool) {
          fireAllRules;
          return true;
      }

      function get(uint256 x) public view returns (int256) {
          return es[x].value;
      }

      function () public payable { }
  }



Examples of salience
""""""""""""""""""""

If you want some rules to be processed first than other rules (i.e higher priority), ``salience`` keyword can be used. The bigger the number specified, the higher the priority it have.

.. code:: ts

   rule "test1" salience 20 when {
     p: Person(val >= 10);
   } then {
     p.addr.send(1);
     p.val--;
     update p;
   }

   rule "test2" salience 30 when {
     p: Person(val >= 20);
   } then {
     p.addr.send(2);
     p.val--;
     update p;
   }

In the above example, the second rule will have higher priority.

Examples of no_Loop and lock_on_active
""""""""""""""""""""""""""""""""""""""
Sometimes you may want to update a fact but the activation of the same rule by the same set of fact is not desired.

.. code:: ts

   rule "test" when {
     p: Person(age >= 20);
   } then {
     p.age++;
     p.addr.send(1);
     update p;
   }

If you tried to ``fireAllRules``, the above rule may keep firing (until ``p.age`` overflows). To make it fire only once for each ``fireAllRules``, we can use ``no_loop`` keyword.

.. code:: ts

   rule "test" no_loop true when {
     p: Person(age >= 20);
   } then {
     p.age++;
     p.addr.send(1);
     update p;
   }

Example of rule inheritance
"""""""""""""""""""""""""""
Sometimes constraints of a rule is based on constraints of another rule.
In this case, this rule can ``extends`` another rule.

For example, a department store wants to give elder customers 10 percent discount and their cars free parking.
The discount rule is described as below.

.. code:: ts

    rule "Give 10% discount to customers older than 60"
    when {
        $customer : Customer( age > 60 );
    } then {
        $customer.discount = 10;
    }

The free parking rule can ``extends`` the constraint of elder customers (older then 60).
Then this rule can be written as below.

.. code:: ts

    rule "Give free parking to customers older than 60"
        extends "Give 10% discount to customers older than 60"
    when {
        $car : Car ( ownerID == $customer.id );
    } then {
        $car.freeParking = true ;
    }

The rule above (with ``extends``) is equivalent to the rule written without ``extends``.

.. code:: ts

    rule "Give free parking to customers older than 60"
    when {
        $customer : Customer( age > 60 );
        $car : Car ( ownerID == $customer.id );
    } then {
        $car.freeParking = true ;
    }

Tax caculation
""""""""""""""
In this example, we illustrate how to caculate tax by rule engine.
In most countries, tax rates are divides into brackets.
That is, certain income range is taxed for corresponding rates.
Often, more income indicates higher tax rates.

Take `this region <https://www.ntbt.gov.tw/etwmain/web/ETW118W/CON/2074/5702201758651492080>`_ for example
(For simplicity, we ignore deductions and exemptions for real tax rules. Thus, actual tax rates would be lower.),
the corresponding rate table is below.

=====================  ==============
Net income             Tax rate
=====================  ==============
        0 ~   540,000   5%
  540,000 ~ 1,210,000  12%
1,210,001 ~ 2,420,000  20%
2,420,001 ~ 4,530,000  30%
4,530,001 ~ ∞          40%
=====================  ==============

For the first tax bracket, net income from 0 to 540000 is taxed for 5%.
This is represented as below.

.. code:: ts

    rule "first bracket" when{
        p: Person(salary > 0)
    } then {
        p.tax += min(540000, p.salary) * 5 / 100;
    }

Similarly, net income from 540001 to 1210000 is taxed for 12% in the second tax bracket.
Note that income 540000 has already been taxed in the first tax bracket, so the amount taxed here should minus 540000.

.. code:: ts

    rule "second bracket" when{
        p: Person(salary > 540000)
    } then {
        p.tax += (min(1210000, p.salary) - 540000) * 12 / 100;
    }

In the same way, rest brackets are represented as below.

.. code:: ts

    rule "third bracket" when{
        p: Person(salary > 1210000)
    } then {
        p.tax += (min(2420000, p.salary) - 1210000) * 20 / 100;
    }

    rule "fourth bracket" when{
        p: Person(salary > 2420000)
    } then {
        p.tax += (min(4530000, p.salary) - 2420000) * 30 / 100;
    }

    rule "fifth bracket" when{
        p: Person(salary > 4530000)
    } then {
        p.tax += (p.salary - 4530000) * 40 / 100;
    }

Cats
""""

A cat is walking on a number line. Initially it is so hungry that it can't even move.
Fortunately, there are some cat foods scattered on the number line. And each cat food can provide some energy to the cat.
Whenever the cat's location equal to cat food's location, the cat will immediately eat all the cat foods on that location and gain energy to move forward.

First, we define our fact types:

.. code:: ts

    struct Cat {
        uint256 id;
        uint256 energy;
    }
    struct CatLocation {
        uint256 id;
        uint256 value;
    }
    struct Food {
        uint256 location;
        uint256 energy;
        bool eaten;
    }

Here we model the problem in a way similiar to entity-relationship model. ``Cat`` and ``CatLocation`` has an one-to-one relationship.
Food represents a cat food on the number line, ``location`` represents its location, ``energy`` represents how much energy it can provide to Cat. Each unit of energy provides power for the cat to move one unit forward.

Now we can define 2 rules to solve the problem.

.. code:: ts

    rule "catEatFood" salience 10
    when {
        c1: Cat();
        cl1: CatLocation(id == c1.id);
        f1: Food(location == cl1.value, !eaten);
    } then {
        c1.energy += f1.energy;
        update c1;
        f1.eaten = true;
        update f1;
    }

In the above rule, we first match ``Cat`` and ``CatLocation`` using ``id``, then match all not yet eaten food that have the same location.
If we successfully found a cat whose location equal to the food's location, we let the cat eat the food and tell rule engine that ``c1`` and ``f1``'s value have been modified, so that no food will be eaten twice, for example.

The second rule:

.. code:: ts

    rule "catMoves" salience 0
    when {
        c1: Cat(energy > 0);
        cl1: CatLocation(id == c1.id);
    } then {
        c1.energy--;
        update c1;
        cl1.value++;
        update cl1;
    }

This rule states that if the cat have positive energy, it can move one unit forward.

``salience`` is set so that the cat eat the food whenever its location overlaps with food's location.


Complete source code of the contract:

.. code:: ts

    contract C {
        struct Cat {
            uint256 id;
            uint256 energy;
        }
        struct CatLocation {
            uint256 id;
            uint256 value;
        }
        struct Food {
            uint256 location;
            uint256 energy;
            bool eaten;
        }

        // Note that rules appear first have higher priority,
        // so cats won't go through a food without eating it.
        rule "catEatFood" salience 10
        when {
            c1: Cat();
            cl1: CatLocation(id == c1.id);
            f1: Food(location == cl1.value, !eaten);
        } then {
            c1.energy += f1.energy;
            update c1;
            f1.eaten = true;
            update f1;
        }

        rule "catMoves" salience 0
        when {
            c1: Cat(energy > 0);
            cl1: CatLocation(id == c1.id);
        } then {
            c1.energy--;
            update c1;
            cl1.value++;
            update cl1;
        }

        Cat[] cats;
        CatLocation[] catLocations;
        uint256[] factIDs;
        Food[] foods;

        function addCat(uint256 initialLocation) public returns (bool) {
            uint256 newId = cats.length;
            cats.push(Cat(newId, 0));
            catLocations.push(CatLocation(newId, initialLocation));
            factIDs.push(factInsert cats[newId]);
            factIDs.push(factInsert catLocations[newId]);
            return true;
        }

        function addFood(uint256 location, uint256 energy) public returns (bool) {
            foods.push(Food(location, energy, false));
            factIDs.push(factInsert foods[foods.length-1]);
            return true;
        }

        function queryCatCoord(uint256 catId) public view returns (uint256) {
            assert(catLocations[catId].id == catId);
            return catLocations[catId].value;
        }

        function run() public returns (bool) {
            fireAllRules;
            return true;
        }

        function reset() public returns (bool) {
            for (uint256 i = 0; i < factIDs.length; i++)
                factDelete factIDs[i];
            delete cats;
            delete catLocations;
            delete factIDs;
            return true;
        }

        function () public payable { }
    }

Specifications
--------------
Rule Engine Operators
"""""""""""""""""""""

We have three operators to handle facts and working memory:

1. factInsert: add current object as a new fact to working memory.
2. factDelete: remove current object from the working memory.
3. fireAllRules: apply all rules on all facts in working memory.

factInsert
~~~~~~~~~~

This operator takes a struct with storage data location, evaluates to fact handle, which has type ``uint256``. Insert the reference to the storage struct into working memory.

For example:

.. code-block:: ts

   contract C {
     struct fact { int x; }
     fact[] facts;
     constructor() public {
        facts.push(fact(0));
        factInsert facts[facts.length-1]; // insert the fact into working memory
     }
   }

And note that the following statement cannot be compiled:

.. code-block:: ts

   factInsert fact(0);

The reason is that ``fact(0)`` is a reference with memory data location, which is not persistant thus cannot be inserted into working memory.

For more information about data location mechanism, please refer to `solidity's documentation <https://solidity.readthedocs.io/en/v0.4.25/types.html#data-location>`_

factDelete
~~~~~~~~~~

This operator takes a fact handle (uint256) and evaluates to void. Removes the reference of the fact from working memory.

fireAllRules
~~~~~~~~~~~~

``fireAllRules`` is a special statement that launches lity rule engine execution, it works like drools' ``ksession.fireAllRules()`` API.

Grammar
"""""""

Grammar of rule definition:

.. code-block:: bnf

   Rule = 'rule' StringLiteral RuleAttributes 'when' '{' RuleLHS '}' 'then' '{' RuleRHS '}'
   RuleLHS = ( ( Identifier ':' )? FactMatchExpr ';' )*
   FactMatchExpr = Identifier '(' ( FieldExpr ( ',' FieldExpr )* )? ')'
   FieldExpr = Expression
   RuleRHS = ( Statement | 'update' Identifier ';' )*
   RuleAttributes = ( 'no_loop true' | 'lock_on_active true' ( 'salience' DecimalNumber ) )*

Note that some nonterminal symbols are defined in solidity's grammar, including ``StringLiteral``, ``Identifier``, ``Expression``, ``Statement``, and ``DecimalNumber``.

Rete Network Generation
"""""""""""""""""""""""

* Each ``FieldExpr`` involve more than 1 facts creates a beta node. Otherwise, it creates an alpha node.
* Each nodes corresponding to a dynamic memory array (a data structure which supports lity rule engine runtime execution), these dynamic memory array contains matched fact sets of each node.
* All dynamic memory arrays are reevaluated when ``fireAllRules`` is called.


Rule Engine Examples
====================

.. _rule-examples:

Fibonacci numbers
------------------

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


Cats
------------------

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

Now we can define 2 rules to solve the problem (Note that the order of definition is important!)

.. code:: ts

    rule "catEatFood"
    when {
        c1: Cat(true);
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

    rule "catMoves"
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

The order of rules is important because we want the cat eat the food whenever its location overlaps with food's location. If the order is reversed, the cat will keep moving forward and ignore the food, which is not what we want.


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
        rule "catEatFood"
        when {
            c1: Cat(true);
            cl1: CatLocation(id == c1.id);
            f1: Food(location == cl1.value, !eaten);
        } then {
            c1.energy += f1.energy;
            update c1;
            f1.eaten = true;
            update f1;
        }

        rule "catMoves"
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


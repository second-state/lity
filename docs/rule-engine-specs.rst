Rule Engine Implementation Specification
====================

.. _rule-engine-specs:

Design Principle
-----------

The semantics of Lity rule engine should be close to Drools's semantics.

Efficiency and complexity of our algorithm may differ from traditional BRMS systems (due to EVM's limitations), but the logic of rule engine should be the same.

Terms Definitions
-----------

Fact
"""""""
A struct that can be pattern matched when Rete match algorithm is executed. The Fact has to be a struct stored in storage.

Working Memory
"""""""
A set of storage pointers, which points to facts. It can be manipulated using factInsert and factDelete operators.

Grammar
------------
Grammar of current rule definition:

.. code-block:: abnf
   :linenos:

   Rule = 'rule' StringLiteral 'when' '{' RuleLHS '}' 'then' '{' RuleRHS '}'
   RuleLHS = ( ( Identifier ':' )? FactMatchExpr ';' )
   FactMatchExpr = Identifier '(' ( FieldExpr (',' FieldExpr)* )? ')'
   FieldExpr = ( Identifier ':' Identifier ) | Expression
   RuleRHS = Statement*

Note that some nonterminal symbols are defined in solidity's grammar, including ``StringLiteral``, ``Identifier``, ``Expression``, and ``Statement``.

Working memory manipulation operators
-------

factInsert
""""""

This operator takes a struct with storage data location, evaluates to fact handle, which has type ``uint256``. Insert the reference to the storage struct into working memory.

factDelete
""""""

This operator takes a fact handle (uint256) and evaluates to void. Removes the reference of the fact from working memory.

Rule engine execution instructions
-------

fireAllRules
""""""
Fire all rules, just like Drools' ``fireAllRules()``

Rete Network Generation
-------

* Each ``FieldExpr`` involve more than 1 facts creates a beta node. Otherwise, it creates an alpha node.
* Each nodes corresponding to a dynamic memory array (a data structure which supports lity rule engine runtime execution), these dynamic memory array contains matched fact sets of each node.
* All dynamic memory arrays are reevaluated when ``fireAllRules`` is called.


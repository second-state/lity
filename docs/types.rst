=====
Types

.. _types-fixed-point-numbers:

Fixed Point Numbers
-------------------

.. WARNING::
   Lity supports fixed point numbers after lity v1.2.6.

Keywords are ``fixedMxN`` for signed fixed point numbers, and ``ufixedMxN``
for unsigned fixed point numbers.
``M`` is the number of bits used by the type, and ``N`` is the number of
fractional points.
``M`` should be a number in ``[8, 256]`` which is divisible by 8, and ``N``
is a non-negative integer that fits in an ``uint32``.

``ufixed`` and ``fixed`` are aliases to ``ufixed128x18`` and ``fixed128x18``
respectively.

Grammar
```````

.. code::

    SignedFixed   = 'fixed' ( [0-9]+ 'x' [0-9]+ )?
    UnsignedFixed = 'ufixed' ( [0-9]+ 'x' [0-9]+ )?

Operators
`````````

- Arithmetics: binary ``+``, binary ``-``, unary ``+``, unary ``-``, ``*``, ``/``, ``%``
- Comparisons: ``<=``, ``<``, ``==``, ``!=``, ``>=``, ``>``
- Conversions: see :ref:`types-fixed-point-numbers-conversions`.

Arithmetic operators may cause implicit
:ref:`truncation <types-fixed-point-numbers-truncations>`.

Definition
``````````

Take a look at `Wikipedia's explanation <wiki_>`__ for the definition of
fixed point numbers.

.. _wiki: https://en.wikipedia.org/wiki/Fixed-point_arithmetic

You may check the ranges of some fixed point types listed below
to better understand the concept of fixed point numbers.

+---------------+------------------------------+
| Type          | Range                        |
+===============+==============================+
| ``ufixed8x1`` | ``[0.0, 25.5]``              |
+---------------+------------------------------+
| ``ufixed8x8`` | ``[0.00000000, 0.00000255]`` |
+---------------+------------------------------+
| ``fixed16x2`` | ``[-327.68, 32767]``         |
+---------------+------------------------------+
| ``fixed16x7`` | ``[-0.0032768, 0.0032767]``  |
+---------------+------------------------------+

.. _types-fixed-point-numbers-conversions:

Conversions between Literals and Fixed Point Types
``````````````````````````````````````````````````

Decimal literals can be converted to the target fixed point type
only if it fits into ``N`` bits.

Implcit Conversion
''''''''''''''''''

Decimal literals can be converted to fixed point types
implicitly if no :ref:`truncation <types-fixed-point-numbers-truncations>`
occurred.

.. code-block:: Lity

    ufixed8x2 a = 1.1; // rational 11/10 to ufixed8x2, ok
    fixed8x1 sa = 1.1; // rational 11/10 to fixed8x1, ok

    ufixed8x2 b = 9.9; // rational 99/10 to ufixed8x2 [0.00, 2.55], fail

Explcit Conversion
''''''''''''''''''

When :ref:`truncation <types-fixed-point-numbers-truncations>` will occur,
explicit conversion is required.

.. code-block:: Lity

    ufixed16x2 pi = ufixed16x2(3.1415926535); // truncated to 3.14

Not Convertible
'''''''''''''''

A ``TypeError`` is raised when the literal does not fit into ``N`` bits
even if using explcit conversion.

.. code-block:: Lity

    // store 2.56 into ufixed8x1 [0.0, 25.5] results in TypeError
    ufixed8x1 a = 25.6;
    ufixed8x1 b = ufixed8x1(25.6);

Conversions between Different Fixed Point Types
```````````````````````````````````````````````

Explicit conversion is also required between different fixed point types
if it might cause :ref:`truncation <types-fixed-point-numbers-truncations>`.

.. code-block:: Lity

    ufixed16x2 pi2 = 3.14;
    ufixed16x4 pie = ufixed16x4(pi2); // still 3.14 but need explicit conversion

.. WARNING::
   Conversions that causes overflow or underflow of fixed point types
   is currently undefined.

.. code-block:: Lity

    fixed8x1 a = -1.0;
    fixed8x1 b = -0.1;
    ufixed8x1 ua = ufixed8x1(a); // undefined
    ufixed8x1 ub = ufixed8x1(b); // undefined

.. _types-fixed-point-numbers-truncations:

Truncations
```````````

If a fixed point number has more than ``N`` digits after its decimal point,
all digits after the ``N``-th digit are truncated.

.. TODO::
   Add some examples here.

ABI Types
`````````

``fixedMxN`` for signed fixed point numbers, and
``ufixedMxN`` for unsigned fixed point numbers.
The definition of ``M`` and ``N`` are the same as those
in the definition of Lity fixed point numbers.

Compatibility with Solidity
```````````````````````````

Fixed point numbers are not fully supported in Solidity yet.
The range of fractional points ``N`` in Solidity is currently ``[0, 80]`` (see
`Solidity's documentation for fixed point numbers <soldoc_>`__).
There's a discussion to change it to ``[0, 77]`` `here at #4061 <i4061_>`__.

.. _i4061: https://github.com/ethereum/solidity/issues/4061
.. _soldoc: https://solidity.readthedocs.io/en/latest/types.html#fixed-point-numbers

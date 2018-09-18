=====
Types
=====

.. _types-fixed-point-numbers:

Fixed Point Numbers
-------------------

Keywords are ``fixedMxN`` for signed fixed point numbers, and ``ufixedMxN``
for unsigned fixed point numbers.
``M`` is the number of bits used by the type, and ``N`` is the number of
fractional points.
``M`` should be a number in ``[8, 256]`` which is divisible by 8, and ``N``
is a positive integer that fits in an ``uint32``.

``ufixed`` and ``fixed`` are aliases to ``ufixed128x18`` and ``fixed128x18``
respectively.

Grammar
```````

.. code::

    SignedFixed   = 'fixed' ( [0-9]+ 'x' [0-9]+ )?
    UnsignedFixed = 'ufixed' ( [0-9]+ 'x' [0-9]+ )?

Examples
````````

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

Take a look at `Wikipedia's explanation <wiki_>`__ for the definition of
fixed point numbers.

.. _wiki: https://en.wikipedia.org/wiki/Fixed-point_arithmetic

Compatibility with Solidity
```````````````````````````

Fixed point numbers are not fully supported in Solidity yet.
The range of fractional points ``N`` in Solidity is currently ``[0, 80]`` (see
`Solidity's documentation for fixed point numbers <soldoc_>`__).
There's a discussion to change it to ``[0, 77]`` `here at #4061 <i4061_>`__.

.. _i4061: https://github.com/ethereum/solidity/issues/4061
.. _soldoc: https://solidity.readthedocs.io/en/latest/types.html#fixed-point-numbers

Operators
`````````

- Comparisons: ``<=``, ``<``, ``==``, ``!=``, ``>=``, ``>``
- Arithmetics: binary ``+``, binary ``-``, unary ``+``, unary ``-``, ``*``, ``/``, ``%``

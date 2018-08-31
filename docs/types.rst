=====
Types
=====

.. _types-fixed-point-numbers:

Fixed Point Numbers
-------------------

Keywords are ``fixedMxN`` for signed fixed point numbers, and ``ufixedMxN``
for unsigned fixed point numbers.
``M`` is the number of bits used by the type, and ``N`` is the number of
decimal points.
``M`` should be a number in ``[8, 256]`` which is divisible by 8, and ``N``
is a number in ``[1, 76]``.

``ufixed`` and ``fixed`` are aliases to ``ufixed128x18`` and ``fixed128x18``
respectively.

Grammar
```````

.. code::

    SignedFixed   = 'fixed' ( [0-9]+ 'x' [0-9]+ )?
    UnsignedFixed = 'ufixed' ( [0-9]+ 'x' [0-9]+ )?

How is it different from Solidity's fixed point numbers?
````````````````````````````````````````````````````````

Theoretically at most 78 decimal digits could be stored in 256 bits, so we
limit ``N`` to be less than 77 so that all signed and unsigned cases could
be handled properly.
We also limit ``N`` to be at least 1, since fixed point numbers without any
decimal points are integers.

You may like to check `Solidity's documentations for fixed point numbers <https://solidity.readthedocs.io/en/latest/types.html#fixed-point-numbers>`_.

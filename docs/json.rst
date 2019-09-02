JSON
====

.. _eni_json:

This example shows how to use our :code:`eni_json.so` to parse JSON.

:code:`eni_json.so` is relatively low-level, so a :code:`json.lity` library is provided.

Usage
-----

`json.lity <https://github.com/second-state/json.lity/blob/master/lity/json.lity>`_ defines the functions to interface with the low level eni library.

Here's an example that is equivalent to ``string = data[1][0]`` in python or javascript:

.. code-block:: Lity
  :linenos:

  pragma lity ^1.2.4;

  import "./json.lity";

  contract Test {
    using JSON for JSON.ValueRef;
    function main(string raw) pure public returns(string) {
      JSON.ValueRef memory root = JSON.newValueRef(raw);
      JSON.ValueRef memory a1 = root.arrayGet(raw, 1);
      string memory a1_0 = a1.arrayGet(raw, 0).asString(raw);
      return a1_0;
    }
  }

* Line 3: ``import`` to use
* Line 6: the ``using`` keyword can be used to ease programming: we can write ``a1_0.asString(raw)`` instead of ``JSON.asString(a1_0, raw)``
* Line 8: the JSON library does not store copies of parsed objects in memory, instead, they store references (``byte`` ranges) to the raw JSON string. ``JSON.newValueRef`` helps initializing a ``JSON.ValueRef`` for the entire raw JSON string.
* Line 9: ``arrayGet`` can be used to get a value by index  get in an array.
* Line 10: ``asString`` can be used to parse a JSON value as a string.

ValueRef
--------

.. note::

  The definition of the ``ValueRef`` struct should be treated as an implementation detail.
  Users should only use the ``asType()`` and ``containerGet()`` methods on ``ValueRef``.

A ValueRef currently stores two units, which are the first byte and the last+1 byte of the JSON value.

.. code-block:: Lity

  struct ValueRef {
    uint begin;
    uint end;
  }

API Reference
-------------

.. function:: JSON.newValueRef(rawJSON)

  :returns: JSON.ValueRef

  Creates a new ``JSON.ValueRef`` that references the root JSON value of ``rawJSON``.

  The behavior is undefined if the rawJSON is not a valid JSON value.

.. function:: valueRef.getType(rawJSON)

  :returns: JSON.Type (enum)

  Returns type type of the value referenced by ``valueRef``.

  +-----------------------+
  | Supported types       |
  +=======================+
  | ``JSON.Type.BOOL``    |
  +-----------------------+
  | ``JSON.Type.NUMBER``  |
  +-----------------------+
  | ``JSON.Type.STRING``  |
  +-----------------------+
  | ``JSON.Type.ARRAY``   |
  +-----------------------+
  | ``JSON.Type.OBJECT``  |
  +-----------------------+
  | ``JSON.Type.INVALID`` |
  +-----------------------+

.. function:: valueRef.asBool(rawJSON)

  :returns: bool

  Returns the ``bool`` value referenced by ``valueRef``.

  The behavior is undefined if the value is not a ``JSON.Type.BOOL``.

.. function:: valueRef.asInt(rawJSON)

  :returns: int

  Returns the ``int`` value referenced by ``valueRef``.

  The behavior is undefined if the value is not a ``JSON.Type.NUMBER``.

  If the value is a floating point number, it is rounded down.

.. function:: valueRef.asString(rawJSON)

  :returns: string

  Returns the ``string`` value referenced by ``valueRef``.

  The behavior is undefined if the value is not a ``JSON.Type.STRING``.

.. function:: valueRef.arrayGet(rawJSON, index)

  :returns: JSON.ValueRef

  Returns the reference to ``valueRef[index]``.

  The behavior is undefined if the index is out of the bounds of the array referenced by ``valueRef`` or if ``valueRef`` is not a ``JSON.Type.ARRAY``.

.. function:: valueRef.objectGet(rawJSON, key)

  :returns: JSON.ValueRef

  Returns the reference to ``valueRef[key]``.

  The behavior is undefined if the key does not exist on the object referenced by ``valueRef`` or if ``valueRef`` is not a ``JSON.Type.OBJECT``.

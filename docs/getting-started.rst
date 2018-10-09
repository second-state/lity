Getting Started
===============

Download the Prebuilt lityc
---------------------------

See `Lity releases <https://github.com/CyberMiles/lity/releases>`_
for the latest release.

Dependencies
````````````

- Ubuntu 16.04: :code:`apt install libz3-dev`

Releases
````````

+---------+--------------------------------------+
| Version | Platform                             |
+=========+======================================+
| v1.2.3_ | | `Ubuntu 16.04 <u123_>`__           |
|         | | `Static Link ELF Binary <s123_>`__ |
+---------+--------------------------------------+
| v1.2.2_ | | `Ubuntu 16.04 <u122_>`__           |
|         | | `Static Link ELF Binary <s122_>`__ |
+---------+--------------------------------------+
| v1.1.1_ | | `Ubuntu 16.04 <u111_>`__           |
|         | | `macOS <m111_>`__                  |
|         | | `Static Link ELF Binary <s111_>`__ |
+---------+--------------------------------------+

.. _v1.2.3: https://github.com/CyberMiles/lity/releases/tag/v1.2.3
.. _u123: https://github.com/CyberMiles/lity/releases/download/v1.2.3/lity-v1.2.3-ubuntu-xenial.zip
.. _s123: https://github.com/CyberMiles/lity/releases/download/v1.2.3/lity-v1.2.3-static

.. _v1.2.2: https://github.com/CyberMiles/lity/releases/tag/v1.2.2
.. _u122: https://github.com/CyberMiles/lity/releases/download/v1.2.2/lity-v1.2.2-ubuntu-xenial.zip
.. _s122: https://github.com/CyberMiles/lity/releases/download/v1.2.2/lity-v1.2.2-static

.. _v1.1.1: https://github.com/CyberMiles/lity/releases/tag/v1.1.1
.. _u111: https://github.com/CyberMiles/lity/releases/download/v1.1.1/lity-v1.1.1-ubuntu-xenial.zip
.. _m111: https://github.com/CyberMiles/lity/releases/download/v1.1.1/lity-v1.1.1-macos.zip
.. _s111: https://github.com/CyberMiles/lity/releases/download/v1.1.1/lity-v1.1.1-static

Build from source
-----------------

Download Source Code
````````````````````

.. code:: bash

  git clone https://github.com/CyberMiles/lity.git

Install Dependencies
````````````````````

In your lity directory:

.. code:: bash

  ./scripts/install_deps.sh

Build with CMake
````````````````

In your lity directory:

.. code:: bash

  mkdir build
  cd build
  cmake ..
  make
  ./lityc/lityc --version

Run Tests
`````````

In your lity directory:

.. code:: bash

  ./build/test/soltest -- --testpath ./test --no-ipc --show_progress
  ./test/cmdlineTests.sh

See `the documentation <download.html>`_ for how to download and install Lity.

Use Lityc to Secure Your Contract
---------------------------------

Some instructions to use ERC checker and overflow protection.

Use ENI in Your Contract
------------------------

:ref:`ENI Tutorial <eni-tutorial>`

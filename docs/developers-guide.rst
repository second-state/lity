Developer's Guide
=================

.. _developers-guide:

Build Lity from source
----------------------

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


How to Download Lity
====================

Build from source
-----------------

.. code:: bash

  git clone https://github.com/CyberMiles/lity.git
  cd lity
  mkdir build
  cd build
  sudo apt-get install cmake libblkid-dev e2fslibs-dev libboost-all-dev libaudit-dev
  cmake ..
  make
  ./lityc/lityc --help

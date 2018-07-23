How to Download Lity
====================

Build from source
-----------------

.. code:: bash

  git clone https://github.com/CyberMiles/lity.git
  cd lity
  mkdir build
  cd build
  sudo apt-get install software-properties-common
  sudo add-apt-repository ppa:george-edison55/cmake-3.x
  sudo apt-get update
  sudo apt-get install cmake
  sudo apt-get upgrade
  cmake ..
  make
  ./lityc/lityc --help

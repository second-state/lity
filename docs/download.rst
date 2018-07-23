How to Download Lity
====================

Build from source
-----------------

.. code:: bash

  git clone https://github.com/CyberMiles/lity.git
  cd lity
  mkdir build
  cd build
  sudo apt-get install build-essential
  wget http://www.cmake.org/files/v3.2/cmake-3.2.2.tar.gz
  tar xf cmake-3.2.2.tar.gz
  cd cmake-3.2.2
  ./configure
  make
  sudo make install
  cmake ..
  make
  ./lityc/lityc --help

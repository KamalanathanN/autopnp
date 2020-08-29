# autopnp

Requirements for compiling `ipa_room_segmentation` on melodic (I didn't try the other packages):

- Compile [OpenBLAS](https://github.com/xianyi/OpenBLAS/wiki/User-Manual) and install somewhere where cmake will find it,
e.g. `/usr/local` (by default gets installed in `/opt`). With `libblas-dev` I always got undefined reference to symbol 'cblas_ddot`.
  ### Follow these steps to compile OpenBLAS
  
     ```
     tar zxvf OpenBLAS-0.2.18.tar.gz
     cd OpenBLAS*
     make FC=gfortran
     sudo make install
     ```
- Compile [dlib](https://github.com/davisking/dlib) and install somewhere where cmake will find it,
(by default gets installed in `/usr/local`). Not 100% sure if it's not possible to make it work with `libdlib-dev.
  ### Davis King, creator of Dlib, recommends using CMake for using Dlib in your code.so to use Dlib as a library follow these steps:

  ```
  tar xvf dlib-19.6.tar.bz2
  cd dlib-19.6/
  mkdir build
  cd build
  cmake ..
  cmake --build . --config Release
  sudo make install
  sudo ldconfig
  cd ..
  ```
Now you can use pkg-config to provide path to Dlib’s include directory and link Dlib library file.
    ```
    pkg-config --libs --cflags dlib-1
    ```


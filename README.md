# autopnp

Requirements for compiling `ipa_room_segmentation` on melodic (I didn't try the other packages):

- Compile [OpenBLAS](https://github.com/xianyi/OpenBLAS/wiki/User-Manual) and install somewhere where cmake will find it,
e.g. `/usr/local` (by default gets installed in `/opt`). With `libblas-dev` I always got undefined reference to symbol 'cblas_ddot`.
- Compile [dlib](https://github.com/davisking/dlib) and install somewhere where cmake will find it,
(by default gets installed in `/usr/local`). Not 100% sure if it's not possible to make it work with `libdlib-dev`, but I gave up.

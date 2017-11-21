
=================
**Building**
=================

Project uses CMake (version 2.8 is minimal supported) as its buildsystem, so it should work everywhere where CMake is supported and all required libraries are available.
Simply invoke `cmake .` in the main directory or - in case of out-of-source (recommended) builds, invoke it from a different directory. Just like any other CMake invocation.

If you wish to use provided libraries on MSVS, make sure you have installed at least MSVS 2013 Update 4, as some stuff in ChaiScript library requires it.

Additionally, an experimental support for building with Premake5 is (shall be) supported. Please note that this will most likely remain experimental, as Premake5 itself is in alpha phase.

# logreader
A fast GUI-mostly application for analyzing logfiles

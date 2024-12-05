#!/bin/sh

COMPILER_OPTIONS="-std=c++11 -shared-libgcc -pedantic -Wall -Wextra -Werror -O3";
COMPILER_DEFINES="-D DEBUG";
PATH_INCLUDE="";
PATH_LIBRARY="";
LINKED_LIBRARIES="-l stdc++ -l OpenCL";

mkdir -p build
gcc $COMPILER_OPTIONS $COMPILER_DEFINES -I "$PATH_INCLUDE" -c ./source/dct_denoise.cpp -o ./build/dct_denoise.o -L "$PATH_LIBRARY" $LINKED_LIBRARIES;
gcc $COMPILER_OPTIONS $COMPILER_DEFINES -I "$PATH_INCLUDE" ./build/dct_denoise.o source/denice.cpp -o ./build/denice -L "$PATH_LIBRARY" $LINKED_LIBRARIES;

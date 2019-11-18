#!/bin/sh

COMPILER_OPTIONS="-std=c++11 -shared-libgcc -pedantic -Wall -Wextra -Werror -O3";
COMPILER_DEFINES="-D DEBUG";
PATH_INCLUDE="";
PATH_LIBRARY="";
LINKED_LIBRARIES="-l stdc++ -l OpenCL";

gcc $COMPILER_OPTIONS $COMPILER_DEFINES $PATH_INCLUDE -c ./dct_denoise.cpp -o ./build/dct_denoise.o $PATH_LIBRARY $LINKED_LIBRARIES
gcc $COMPILER_OPTIONS $COMPILER_DEFINES $PATH_INCLUDE ./build/dct_denoise.o denice.cpp -o ./build/denice $PATH_LIBRARY $LINKED_LIBRARIES;

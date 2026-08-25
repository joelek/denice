#!/bin/sh

COMPILER_OPTIONS="-std=c++20 -static -pedantic -Wall -Wextra -O3";
COMPILER_DEFINES="-D DEBUG";
PATH_INCLUDE="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.2\include";
PATH_LIBRARY="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.2\lib\x64";
LINKED_LIBRARIES="-l stdc++ -l OpenCL";

./externalize.sh dct_denoise source/dct_denoise.opencl.c source/dct_denoise.cpp

mkdir -p build
gcc $COMPILER_OPTIONS $COMPILER_DEFINES -I "$PATH_INCLUDE" -c ./source/dct_denoise.cpp -o ./build/dct_denoise.o -L "$PATH_LIBRARY" $LINKED_LIBRARIES;
gcc $COMPILER_OPTIONS $COMPILER_DEFINES -I "$PATH_INCLUDE" ./build/dct_denoise.o source/denice.cpp -o ./build/denice -L "$PATH_LIBRARY" $LINKED_LIBRARIES;

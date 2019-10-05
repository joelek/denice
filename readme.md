# @joelek/denice

Hardware-accelerated efficient video denoising.

## Technology

This project uses a denoising technique called DCT-denoising. The technique exploits the fact that uniform noise is uniformly distributed in the frequency domain.

## Configure

Set up the OpenCL SDK for your platform target and make sure that the compiler finds the required include files and libraries. You can choose between either configuring the compiler or editing the paths in the make script `make.sh`.

```
PATH_INCLUDE="-I <path>"
PATH_LIBRARY="-L <path>"
```

## Build

Build the project using the make script `make.sh`. This will produce a single binary with the name `denice` in the `build/` folder.

```
./make.sh
```

## Use

Launch the binary with the correct arguments.

```
denice <format> <width> <height> <strength>
```

Write frames, channel by channel, to the standard input. Read frames, channel by channel, from the standard output. When the last frame has been written, write `EOF` to the standard input.

# @joelek/denice

Hardware-accelerated detail-preserving video denoising.

## Technology

This project uses a denoising technique called [DCT-denoising](http://www.ipol.im/pub/art/2011/ys-dct/revisions/2011-10-24/ys_dct.html). The technique exploits the fact that uniform noise is uniformly distributed in the frequency domain.

## Configure

Set up the OpenCL SDK for your platform target and make sure that the compiler finds the required include files and libraries. You can choose between either configuring the compiler or editing the paths in the make script `make.sh`.

```
PATH_INCLUDE="<path>"
PATH_LIBRARY="<path>"
```

## Build

Build the project using the make script `make.sh`. This will produce a single binary with the name `denice` in the `build/` folder.

```
./make.sh
```

## Use

Launch the binary with the correct arguments.

```
denice <format> <width> <height> <min_strength> [max_strength] [block_size_log_2] [block_step]
```

Write frames, channel by channel, to the standard input. Read frames, channel by channel, from the standard output. When the last frame has been written, write `EOF` to the standard input.

Strength should be somewhere between 0.00 and 0.05.

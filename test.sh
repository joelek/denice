#!/bin/sh

ffmpeg -hide_banner -loglevel panic -i ./samples/uniform.png -vf "format=yuv420p16le" -f rawvideo pipe: |
	./build/denice yuv420p16le 300 300 0.1 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv420p16le -s 300:300 -i pipe: -vf "format=rgb24" -y ./samples/uniform.010.png;

#!/bin/sh

ffmpeg -hide_banner -loglevel panic -i ./samples/simpsons.png -vf "format=yuv420p16le" -f rawvideo pipe: |
	./build/denice yuv420p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv420p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./samples/simpsons.010.yuv420p16le.png;

ffmpeg -hide_banner -loglevel panic -i ./samples/simpsons.png -vf "format=yuv422p16le" -f rawvideo pipe: |
	./build/denice yuv422p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv422p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./samples/simpsons.010.yuv422p16le.png;

ffmpeg -hide_banner -loglevel panic -i ./samples/simpsons.png -vf "format=yuv444p16le" -f rawvideo pipe: |
	./build/denice yuv444p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv444p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./samples/simpsons.010.yuv444p16le.png;

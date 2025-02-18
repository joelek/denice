#!/bin/sh

ffmpeg -hide_banner -loglevel panic -i ./public/samples/simpsons.png -vf "format=yuv420p16le" -f rawvideo pipe: |
	./build/denice yuv420p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv420p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./public/samples/simpsons.010.yuv420p16le.png;

ffmpeg -hide_banner -loglevel panic -i ./public/samples/simpsons.png -vf "format=yuv422p16le" -f rawvideo pipe: |
	./build/denice yuv422p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv422p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./public/samples/simpsons.010.yuv422p16le.png;

ffmpeg -hide_banner -loglevel panic -i ./public/samples/simpsons.png -vf "format=yuv444p16le" -f rawvideo pipe: |
	./build/denice yuv444p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv444p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./public/samples/simpsons.010.yuv444p16le.png;

ffmpeg -hide_banner -loglevel panic -i ./public/samples/the_oc.png -vf "format=yuv420p16le" -f rawvideo pipe: |
	./build/denice yuv420p16le 720 576 0.02 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv420p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./public/samples/the_oc.002.png;

ffmpeg -hide_banner -loglevel panic -i ./public/samples/the_oc.png -vf "format=yuv420p16le" -f rawvideo pipe: |
	./build/denice yuv420p16le 720 576 0.05 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv420p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./public/samples/the_oc.005.png;

ffmpeg -hide_banner -loglevel panic -i ./public/samples/uniform.png -vf "format=yuv420p16le" -f rawvideo pipe: |
	./build/denice yuv420p16le 720 576 0.10 |
	ffmpeg -hide_banner -loglevel panic -f rawvideo -pix_fmt yuv420p16le -s 720:576 -i pipe: -vf "format=rgb24" -y ./public/samples/uniform.010.png;

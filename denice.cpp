#include <fstream>
#include <iostream>
#include <cmath>
#include <streambuf>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <CL/cl.hpp>
#include "dct_denoise.hpp"

#define BLOCK_SIZE 8

#ifdef DEBUG
	#define OPENCL_CHECK_STATUS() \
		if (status != CL_SUCCESS) { \
			fprintf(stderr, "[%s:%u]: Unexpected OpenCL status \"%s\" (%i)!\n", __FILE__, __LINE__, get_opencl_status(status), status); \
			throw EXIT_FAILURE; \
		}
#else
	#define OPENCL_CHECK_STATUS() \
		if (status != CL_SUCCESS) { \
			fprintf(stderr, "Unexpected OpenCL status \"%s\" (%i)!\n", get_opencl_status(status), status); \
			throw EXIT_FAILURE; \
		}
#endif

auto compute_modulus(int x, int m)
-> int {
	if (m <= 0) {
		fprintf(stderr, "Modulus must be a positive integer!\n");
		throw EXIT_FAILURE;
	}
	return ((x % m) + m) % m;
}

auto compute_global_size_ceil(unsigned int data_size, unsigned int local_size)
-> unsigned int {
	if (local_size == 0) {
		fprintf(stderr, "Unsupported local size!\n");
		throw EXIT_FAILURE;
	}
	return ((data_size + local_size - 1) / local_size) * local_size;
}

auto compute_global_size_floor(unsigned int data_size, unsigned int local_size)
-> unsigned int {
	if (local_size == 0) {
		fprintf(stderr, "Unsupported local size!\n");
		throw EXIT_FAILURE;
	}
	return ((data_size) / local_size) * local_size;
}

struct channel_t {
	int w;
	int h;
};

struct format_t {
	std::vector<channel_t> channels;
	bool two_bytes_per_pixel;
};

struct data_channel_t {
	channel_t& channel;
	cl::Buffer buffer;
	cl::Image2D source;
	cl::Image2D middle;
	cl::Image2D target;
};

struct frame_t {
	std::vector<data_channel_t> data_channels;
	std::vector<unsigned char> buffer;
};

auto parse_format(const char* raw_format, int arg_width, int arg_height)
-> format_t {
	if (false) {
	} else if (strcmp(raw_format, "yuv420p16le") == 0) {
		if ((arg_width & 1) == 1) {
			fprintf(stderr, "Frame format requires an even frame width!\n");
			throw EXIT_FAILURE;
		}
		if ((arg_height & 1) == 1) {
			fprintf(stderr, "Frame format requires an even frame height!\n");
			throw EXIT_FAILURE;
		}
		auto fw = arg_width;
		auto fh = arg_height;
		auto hw = (fw >> 1);
		auto hh = (fh >> 1);
		auto channels = std::vector<channel_t>();
		channels.push_back({ fw, fh });
		channels.push_back({ hw, hh });
		channels.push_back({ hw, hh });
		return { channels, true };
	}
	fprintf(stderr, "Unsupported frame format!\n");
	throw EXIT_FAILURE;
}

auto parse_width(const char* raw_width)
-> int {
	auto value = atoi(raw_width);
	if (value <= 0) {
		fprintf(stderr, "Frame width may not be negative or zero!\n");
		throw EXIT_FAILURE;
	}
	return value;
}

auto parse_height(const char* raw_height)
-> int {
	auto value = atoi(raw_height);
	if (value <= 0) {
		fprintf(stderr, "Frame height may not be negative or zero!\n");
		throw EXIT_FAILURE;
	}
	return value;
}

auto get_opencl_status(int code)
-> const char* {
	switch (code) {
		case 0: return "CL_SUCCESS";
		case -1: return "CL_DEVICE_NOT_FOUND";
		case -2: return "CL_DEVICE_NOT_AVAILABLE";
		case -3: return "CL_COMPILER_NOT_AVAILABLE";
		case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case -5: return "CL_OUT_OF_RESOURCES";
		case -6: return "CL_OUT_OF_HOST_MEMORY";
		case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case -8: return "CL_MEM_COPY_OVERLAP";
		case -9: return "CL_IMAGE_FORMAT_MISMATCH";
		case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case -11: return "CL_BUILD_PROGRAM_FAILURE";
		case -12: return "CL_MAP_FAILURE";
		case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		case -15: return "CL_COMPILE_PROGRAM_FAILURE";
		case -16: return "CL_LINKER_NOT_AVAILABLE";
		case -17: return "CL_LINK_PROGRAM_FAILURE";
		case -18: return "CL_DEVICE_PARTITION_FAILED";
		case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
		case -30: return "CL_INVALID_VALUE";
		case -31: return "CL_INVALID_DEVICE_TYPE";
		case -32: return "CL_INVALID_PLATFORM";
		case -33: return "CL_INVALID_DEVICE";
		case -34: return "CL_INVALID_CONTEXT";
		case -35: return "CL_INVALID_QUEUE_PROPERTIES";
		case -36: return "CL_INVALID_COMMAND_QUEUE";
		case -37: return "CL_INVALID_HOST_PTR";
		case -38: return "CL_INVALID_MEM_OBJECT";
		case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case -40: return "CL_INVALID_IMAGE_SIZE";
		case -41: return "CL_INVALID_SAMPLER";
		case -42: return "CL_INVALID_BINARY";
		case -43: return "CL_INVALID_BUILD_OPTIONS";
		case -44: return "CL_INVALID_PROGRAM";
		case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
		case -46: return "CL_INVALID_KERNEL_NAME";
		case -47: return "CL_INVALID_KERNEL_DEFINITION";
		case -48: return "CL_INVALID_KERNEL";
		case -49: return "CL_INVALID_ARG_INDEX";
		case -50: return "CL_INVALID_ARG_VALUE";
		case -51: return "CL_INVALID_ARG_SIZE";
		case -52: return "CL_INVALID_KERNEL_ARGS";
		case -53: return "CL_INVALID_WORK_DIMENSION";
		case -54: return "CL_INVALID_WORK_GROUP_SIZE";
		case -55: return "CL_INVALID_WORK_ITEM_SIZE";
		case -56: return "CL_INVALID_GLOBAL_OFFSET";
		case -57: return "CL_INVALID_EVENT_WAIT_LIST";
		case -58: return "CL_INVALID_EVENT";
		case -59: return "CL_INVALID_OPERATION";
		case -60: return "CL_INVALID_GL_OBJECT";
		case -61: return "CL_INVALID_BUFFER_SIZE";
		case -62: return "CL_INVALID_MIP_LEVEL";
		case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
		case -64: return "CL_INVALID_PROPERTY";
		case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
		case -66: return "CL_INVALID_COMPILER_OPTIONS";
		case -67: return "CL_INVALID_LINKER_OPTIONS";
		case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
		case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
		case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
		case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
		case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
		case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
		case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
		default: return "";
	}
}

auto get_opencl_platform()
-> cl::Platform {
	auto status = CL_SUCCESS;
	auto platforms = std::vector<cl::Platform>();
	status = cl::Platform::get(&platforms);
	OPENCL_CHECK_STATUS();
	if (platforms.size() == 0) {
		fprintf(stderr, "No platforms found!\n");
		throw EXIT_FAILURE;
	}
	fprintf(stderr, "Found platforms:\n");
	for (auto& platform : platforms) {
		auto name = platform.getInfo<CL_PLATFORM_NAME>(&status);
		OPENCL_CHECK_STATUS();
		auto version = platform.getInfo<CL_PLATFORM_VERSION>(&status);
		OPENCL_CHECK_STATUS();
		fprintf(stderr, "\t\"%s\" (%s)\n", name.c_str(), version.c_str());
	}
	auto platform = platforms[0];
	auto name = platform.getInfo<CL_PLATFORM_NAME>(&status);
	OPENCL_CHECK_STATUS();
	auto version = platform.getInfo<CL_PLATFORM_VERSION>(&status);
	OPENCL_CHECK_STATUS();
	fprintf(stderr, "Selected platform \"%s\" (%s)\n", name.c_str(), version.c_str());
	return platform;
}

auto get_opencl_device(const cl::Platform& platform)
-> cl::Device {
	auto status = CL_SUCCESS;
	auto devices = std::vector<cl::Device>();
	status = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	OPENCL_CHECK_STATUS();
	if (devices.size() == 0) {
		fprintf(stderr, "No devices found!\n");
		throw EXIT_FAILURE;
	}
	fprintf(stderr, "Found devices:\n");
	for (auto& device : devices) {
		auto name = device.getInfo<CL_DEVICE_NAME>(&status);
		OPENCL_CHECK_STATUS();
		fprintf(stderr, "\t\"%s\"\n", name.c_str());
	}
	auto device = devices[0];
	auto name = device.getInfo<CL_DEVICE_NAME>(&status);
	OPENCL_CHECK_STATUS();
	fprintf(stderr, "Selected device \"%s\"\n", name.c_str());
	return device;
}

auto get_opencl_context(const cl::Device& device)
-> cl::Context {
	auto status = CL_SUCCESS;
	auto context = cl::Context(device, nullptr, nullptr, nullptr, &status);
	OPENCL_CHECK_STATUS();
	return context;
}

auto get_opencl_queue(const cl::Context& context, const cl::Device& device)
-> cl::CommandQueue {
	auto status = CL_SUCCESS;
	auto queue = cl::CommandQueue(context, device, 0, &status);
	OPENCL_CHECK_STATUS();
	return queue;
}

auto get_opencl_program(const cl::Context& context, const cl::Device& device, const std::string& source)
-> cl::Program {
	auto status = CL_SUCCESS;
	auto sources = cl::Program::Sources();
	sources.push_back({ source.c_str(), source.length() });
	auto program = cl::Program(context, sources, &status);
	OPENCL_CHECK_STATUS();
	status = program.build({ device });
	try {
		OPENCL_CHECK_STATUS();
	} catch (...) {
		auto log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		fprintf(stderr, "%s", log.c_str());
		throw EXIT_FAILURE;
	}
	return program;
}

auto get_opencl_kernel(const cl::Program& program, const std::string& name)
-> cl::Kernel {
	auto status = CL_SUCCESS;
	fprintf(stderr, "Looking for kernel \"%s\"...\n", name.c_str());
	auto kernel = cl::Kernel(program, name.c_str(), &status);
	OPENCL_CHECK_STATUS();
	return kernel;
}

auto set_binary_input_output()
-> void {
	if (setmode(STDIN_FILENO, O_BINARY) == -1) {
		fprintf(stderr, "Failed setting binary mode for stdin!\n");
		throw EXIT_FAILURE;
	}
	if (setmode(STDOUT_FILENO, O_BINARY) == -1) {
		fprintf(stderr, "Failed setting binary mode for stdout!\n");
		throw EXIT_FAILURE;
	}
}

auto copy_channel_to_device(const cl::CommandQueue& queue, const data_channel_t& data_channel, unsigned char* buffer)
-> void {
	auto origin = cl::size_t<3>();
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	auto region = cl::size_t<3>();
	region[0] = data_channel.channel.w;
	region[1] = data_channel.channel.h;
	region[2] = 1;
	auto status = CL_SUCCESS;
	status = queue.enqueueWriteImage(data_channel.source, CL_TRUE, origin, region, 0, 0, buffer);
	OPENCL_CHECK_STATUS();
	status = queue.enqueueCopyImage(data_channel.source, data_channel.target, origin, origin, region);
	OPENCL_CHECK_STATUS();
}

auto copy_frame_to_device(const cl::CommandQueue& queue, frame_t& frame, bool two_bytes_per_pixel)
-> void {
	auto status = CL_SUCCESS;
	auto frame_buffer_offset = 0;
	for (auto& data_channel : frame.data_channels) {
		copy_channel_to_device(queue, data_channel, &frame.buffer.data()[frame_buffer_offset]);
		auto pixels_in_channel = (data_channel.channel.w * data_channel.channel.h);
		if (two_bytes_per_pixel) {
			pixels_in_channel *= 2;
		}
		frame_buffer_offset += pixels_in_channel;
	}
	status = queue.finish();
	OPENCL_CHECK_STATUS();
}

auto copy_channel_to_host(const cl::CommandQueue& queue, const data_channel_t& data_channel, unsigned char* buffer)
-> void {
	auto origin = cl::size_t<3>();
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	auto region = cl::size_t<3>();
	region[0] = data_channel.channel.w;
	region[1] = data_channel.channel.h;
	region[2] = 1;
	auto status = CL_SUCCESS;
	status = queue.enqueueReadImage(data_channel.target, CL_TRUE, origin, region, 0, 0, buffer);
	OPENCL_CHECK_STATUS();
}

auto copy_frame_to_host(const cl::CommandQueue& queue, frame_t& frame, bool two_bytes_per_pixel)
-> void {
	auto status = CL_SUCCESS;
	auto frame_buffer_offset = 0;
	for (auto& data_channel : frame.data_channels) {
		copy_channel_to_host(queue, data_channel, &frame.buffer.data()[frame_buffer_offset]);
		auto pixels_in_channel = (data_channel.channel.w * data_channel.channel.h);
		if (two_bytes_per_pixel) {
			pixels_in_channel *= 2;
		}
		frame_buffer_offset += pixels_in_channel;
	}
	status = queue.finish();
	OPENCL_CHECK_STATUS();
}

auto filter_channel(const cl::CommandQueue& queue, cl::Kernel& filter_kernel, cl::Kernel& normalize_kernel, const data_channel_t& data_channelp, const data_channel_t& data_channelc, const data_channel_t& data_channeln)
-> void {
	auto status = CL_SUCCESS;
	auto origin = cl::size_t<3>();
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	auto region = cl::size_t<3>();
	region[0] = data_channelc.channel.w;
	region[1] = data_channelc.channel.h;
	region[2] = 1;
	status = queue.enqueueCopyImage(data_channelc.target, data_channelc.middle, origin, origin, region);
	OPENCL_CHECK_STATUS();
	status = filter_kernel.setArg(0, data_channelc.buffer);
	OPENCL_CHECK_STATUS();
	status = filter_kernel.setArg(1, data_channelp.source);
	OPENCL_CHECK_STATUS();
	status = filter_kernel.setArg(2, data_channelc.middle);
	OPENCL_CHECK_STATUS();
	status = filter_kernel.setArg(3, data_channeln.source);
	OPENCL_CHECK_STATUS();
	auto zero = 0.0f;
	queue.enqueueFillBuffer(data_channelc.buffer, &zero, 0, (data_channelc.channel.w * data_channelc.channel.h * sizeof(float)));
	OPENCL_CHECK_STATUS();
	for (auto y = 0; y < BLOCK_SIZE; y += 2) {
		for (auto x = 0; x < BLOCK_SIZE; x += 2) {
			status = filter_kernel.setArg(4, x);
			OPENCL_CHECK_STATUS();
			status = filter_kernel.setArg(5, y);
			OPENCL_CHECK_STATUS();
			auto local_w = BLOCK_SIZE;
			auto local_h = BLOCK_SIZE;
			auto global_w = compute_global_size_floor((data_channelc.channel.w - x), local_w);
			auto global_h = compute_global_size_floor((data_channelc.channel.h - y), local_h);
			status = queue.enqueueNDRangeKernel(filter_kernel, cl::NDRange(0, 0, 0), cl::NDRange(global_w, global_h, 1), cl::NDRange(local_w, local_h, 1));
			OPENCL_CHECK_STATUS();
		}
	}
	status = normalize_kernel.setArg(0, data_channelc.target);
	OPENCL_CHECK_STATUS();
	status = normalize_kernel.setArg(1, data_channelc.buffer);
	OPENCL_CHECK_STATUS();
	auto local_w = BLOCK_SIZE;
	auto local_h = BLOCK_SIZE;
	auto global_w = compute_global_size_ceil(data_channelc.channel.w, local_w);
	auto global_h = compute_global_size_ceil(data_channelc.channel.h, local_h);
	status = queue.enqueueNDRangeKernel(normalize_kernel, cl::NDRange(0, 0, 0), cl::NDRange(global_w, global_h, 1), cl::NDRange(local_w, local_h, 1));
	OPENCL_CHECK_STATUS();
	status = queue.finish();
	OPENCL_CHECK_STATUS();
}

auto filter_frame(const cl::CommandQueue& queue, cl::Kernel& filter_kernel, cl::Kernel& normalize_kernel, frame_t& framep, frame_t& framec, frame_t& framen)
-> void {
	for (auto i = 0; i < 4; i++) {
		for (auto j = 0; j < (int)framec.data_channels.size(); j++) {
			filter_channel(queue, filter_kernel, normalize_kernel, framep.data_channels.at(j), framec.data_channels.at(j), framen.data_channels.at(j));
		}
	}
}

auto difference_frame(const cl::CommandQueue& queue, cl::Kernel& difference_kernel, frame_t& frame)
-> void {
	auto status = CL_SUCCESS;
	for (auto& data_channel : frame.data_channels) {
		auto origin = cl::size_t<3>();
		origin[0] = 0;
		origin[1] = 0;
		origin[2] = 0;
		auto region = cl::size_t<3>();
		region[0] = data_channel.channel.w;
		region[1] = data_channel.channel.h;
		region[2] = 1;
		status = queue.enqueueCopyImage(data_channel.target, data_channel.middle, origin, origin, region);
		OPENCL_CHECK_STATUS();
		status = difference_kernel.setArg(0, data_channel.target);
		OPENCL_CHECK_STATUS();
		status = difference_kernel.setArg(1, data_channel.middle);
		OPENCL_CHECK_STATUS();
		status = difference_kernel.setArg(2, data_channel.source);
		OPENCL_CHECK_STATUS();
		auto local_w = 8;
		auto local_h = 8;
		auto global_w = compute_global_size_ceil(data_channel.channel.w, local_w);
		auto global_h = compute_global_size_ceil(data_channel.channel.h, local_h);
		status = queue.enqueueNDRangeKernel(difference_kernel, cl::NDRange(0, 0, 0), cl::NDRange(global_w, global_h, 1), cl::NDRange(local_w, local_h, 1));
		status = queue.finish();
		OPENCL_CHECK_STATUS();
	}
}


auto is_system_little_endian()
-> bool {
	auto value = "\0\1\0\0\0\0\0\0";
	return *(unsigned int*)(value) == 256;
}

auto main(int argc, char** argv)
-> int {
	try {
		auto status = CL_SUCCESS;
		if (argc < 4) {
			fprintf(stderr, "Please supply program arguments in the following order.\n");
			fprintf(stderr, "\tEnum describing frame format.\n");
			fprintf(stderr, "\tInteger describing frame width.\n");
			fprintf(stderr, "\tInteger describing frame height.\n");
			throw EXIT_FAILURE;
		}
		auto raw_format = argv[1];
		auto raw_width = argv[2];
		auto raw_height = argv[3];
		auto arg_width = parse_width(raw_width);
		auto arg_height = parse_height(raw_height);
		auto arg_format = parse_format(raw_format, arg_width, arg_height);
		fprintf(stderr, "Frame format set to \"%s\".\n", raw_format);
		fprintf(stderr, "Frame width set to %i.\n", arg_width);
		fprintf(stderr, "Frame height set to %i.\n", arg_height);
		set_binary_input_output();
		auto platform = get_opencl_platform();
		auto device = get_opencl_device(platform);
		auto context = get_opencl_context(device);
		auto queue = get_opencl_queue(context, device);
		auto program = get_opencl_program(context, device, dct_denoise);
		auto filter_kernel = get_opencl_kernel(program, "filter_kernel");
		auto normalize_kernel = get_opencl_kernel(program, "normalize_kernel");
		auto difference_kernel = get_opencl_kernel(program, "difference_kernel");
		auto frame_buffer_capacity = 3;
		auto image_format = cl::ImageFormat(CL_LUMINANCE, CL_UNORM_INT8);
		if (arg_format.two_bytes_per_pixel) {
			image_format = cl::ImageFormat(CL_LUMINANCE, CL_UNORM_INT16);
		}
		auto frames = std::vector<frame_t>();
		for (auto i = 0; i < frame_buffer_capacity; i++) {
			auto data_channels = std::vector<data_channel_t>();
			auto bytes_per_frame = 0;
			for (auto& channel : arg_format.channels) {
				auto buffer = cl::Buffer(context, CL_MEM_READ_WRITE, channel.w * channel.h * sizeof(float), nullptr, &status);
				OPENCL_CHECK_STATUS();
				auto source = cl::Image2D(context, CL_MEM_READ_WRITE, image_format, channel.w, channel.h, 0, nullptr, &status);
				OPENCL_CHECK_STATUS();
				auto middle = cl::Image2D(context, CL_MEM_READ_WRITE, image_format, channel.w, channel.h, 0, nullptr, &status);
				OPENCL_CHECK_STATUS();
				auto target = cl::Image2D(context, CL_MEM_READ_WRITE, image_format, channel.w, channel.h, 0, nullptr, &status);
				OPENCL_CHECK_STATUS();
				bytes_per_frame += (channel.w * channel.h);
				data_channels.push_back({ channel, buffer, source, middle, target });
			}
			if (arg_format.two_bytes_per_pixel) {
				bytes_per_frame *= 2;
			}
			auto buffer = std::vector<unsigned char>(bytes_per_frame);
			frames.push_back({ data_channels, buffer });
		}
		auto frames_read = 0;
		auto frames_filtered = 0;
		auto frames_written = 0;
		while (!feof(stdin) || (frames_written < frames_read)) {
			for (auto i = frames_read; i < frames_written + frame_buffer_capacity; i++) {
				auto& frame = frames.at(compute_modulus(i, frame_buffer_capacity));
				auto new_frames_read = fread(frame.buffer.data(), frame.buffer.size(), 1, stdin);
				if (new_frames_read == 0) {
					break;
				}
				copy_frame_to_device(queue, frame, arg_format.two_bytes_per_pixel);
				frames_read += new_frames_read;
			}
			for (auto i = frames_filtered; i < frames_read; i++) {
				if (!feof(stdin) && (i >= frames_read - frame_buffer_capacity + 1)) {
					break;
				}
				auto& framep = frames.at(compute_modulus(i - 1, frame_buffer_capacity));
				auto& framec = frames.at(compute_modulus(i, frame_buffer_capacity));
				auto& framen = frames.at(compute_modulus(i + 1, frame_buffer_capacity));
				filter_frame(queue, filter_kernel, normalize_kernel, framep, framec, framen);
				//difference_frame(queue, difference_kernel, framec);
				copy_frame_to_host(queue, framec, arg_format.two_bytes_per_pixel);
				frames_filtered += 1;
			}
			for (auto i = frames_written; i < frames_filtered; i++) {
				auto& frame = frames.at(compute_modulus(i, frame_buffer_capacity));
				auto new_frames_written = fwrite(frame.buffer.data(), frame.buffer.size(), 1, stdout);
				if (new_frames_written == 0) {
					break;
				}
				frames_written += new_frames_written;
			}
			usleep(1000);
		}
		fprintf(stderr, "A total of %i frames were read.\n", frames_read);
		fprintf(stderr, "A total of %i frames were filtered.\n", frames_filtered);
		fprintf(stderr, "A total of %i frames were written.\n", frames_written);
		fprintf(stderr, "Program completed successfully.\n");
		return EXIT_SUCCESS;
	} catch (...) {
		fprintf(stderr, "Program did not complete successfully!\n");
		return EXIT_FAILURE;
	}
}

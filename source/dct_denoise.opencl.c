#define BLOCK_SIZE_LOG2 3
#define BLOCK_SIZE (1 << BLOCK_SIZE_LOG2)
#define BLOCK_MAX (BLOCK_SIZE - 1)

__constant float dct_coefficients[64] = {
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.35355339059327378637f,
	0.49039264020161521529f,
	0.41573480615127261784f,
	0.27778511650980114434f,
	0.09754516100806416568f,
	-0.09754516100806409629f,
	-0.27778511650980097780f,
	-0.41573480615127267335f,
	-0.49039264020161521529f,
	0.46193976625564336924f,
	0.19134171618254491865f,
	-0.19134171618254486313f,
	-0.46193976625564336924f,
	-0.46193976625564342475f,
	-0.19134171618254516845f,
	0.19134171618254500191f,
	0.46193976625564325822f,
	0.41573480615127261784f,
	-0.09754516100806409629f,
	-0.49039264020161521529f,
	-0.27778511650980108882f,
	0.27778511650980092229f,
	0.49039264020161521529f,
	0.09754516100806438772f,
	-0.41573480615127256232f,
	0.35355339059327378637f,
	-0.35355339059327373086f,
	-0.35355339059327384188f,
	0.35355339059327367535f,
	0.35355339059327384188f,
	-0.35355339059327334228f,
	-0.35355339059327356432f,
	0.35355339059327328677f,
	0.27778511650980114434f,
	-0.49039264020161521529f,
	0.09754516100806415180f,
	0.41573480615127278437f,
	-0.41573480615127256232f,
	-0.09754516100806401302f,
	0.49039264020161532631f,
	-0.27778511650980075576f,
	0.19134171618254491865f,
	-0.46193976625564342475f,
	0.46193976625564325822f,
	-0.19134171618254494640f,
	-0.19134171618254527947f,
	0.46193976625564336924f,
	-0.46193976625564320271f,
	0.19134171618254477987f,
	0.09754516100806416568f,
	-0.27778511650980108882f,
	0.41573480615127278437f,
	-0.49039264020161532631f,
	0.49039264020161521529f,
	-0.41573480615127250681f,
	0.27778511650980075576f,
	-0.09754516100806429058f
};

float assess_denoising_strength1(__local float* block) {
	float x_factor = 0.0f;
	float x_weight_total = 0.0f;
	float y_factor = 0.0f;
	float y_weight_total = 0.0f;
	float factor = 0.0f;
	float weight_total = 0.0f;
	float power = 1.0f;
	int offset = 0;
	for (int y = offset; y < BLOCK_SIZE; y++) {
		for (int x = offset; x < BLOCK_SIZE; x++) {
			float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
			float x_weight = ((float)(x - offset)) * value;
			float y_weight = ((float)(y - offset)) * value;
			float weight = ((float)(x - offset + y - offset)) * value;
			x_factor += x_weight * (float)(x - offset) / (BLOCK_MAX - offset);
			x_weight_total += x_weight;
			y_factor += y_weight * (float)(y - offset) / (BLOCK_MAX - offset);
			y_weight_total += y_weight;
			factor += weight * (float)(x - offset + y - offset) / (BLOCK_MAX - offset + BLOCK_MAX - offset);
			weight_total += weight;
		}
	}
	x_factor /= x_weight_total;
	y_factor /= y_weight_total;
	factor /= weight_total;
	float strength = pow(x_factor * y_factor, 1.0f);
	return strength;
}

int get_scaling_factor(int coordinate, int size) {
	if (coordinate < 0) {
		return 1;
	} else if (coordinate < BLOCK_SIZE) {
		return coordinate + 1;
	} else if (coordinate <= size - BLOCK_SIZE) {
		return BLOCK_SIZE;
	} else if (coordinate < size) {
		return size - coordinate;
	} else {
		return 1;
	}
}

void convert_to_relative_range(__local float* block, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	float s = block[offset];
	s = (s * 2.0f) - 1.0f;
	block[offset] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void convert_to_absolute_range(__local float* block, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	float s = block[offset];
	s = (s + 1.0f) * 0.5f;
	block[offset] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void compute_dct(__local float* block, int offset, int stride_log2, int k) {
	float s = 0.0f;
	for (int n = 0; n < BLOCK_SIZE; n++) {
		float v = block[offset + (n << stride_log2)];
		float c = dct_coefficients[(k << BLOCK_SIZE_LOG2) + n];
		s += (v * c);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	block[offset + (k << stride_log2)] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void compute_dct_x(__local float* block, int x, int y) {
	compute_dct(block, y << BLOCK_SIZE_LOG2, 0, x);
}

void compute_dct_y(__local float* block, int x, int y) {
	compute_dct(block, x, BLOCK_SIZE_LOG2, y);
}

void compute_dct_xy(__local float* block, int x, int y) {
	compute_dct_x(block, x, y);
	compute_dct_y(block, x, y);
}

void compute_idct(__local float* block, int offset, int stride_log2, int k) {
	float s = 0.0f;
	for (int n = 0; n < BLOCK_SIZE; n++) {
		float v = block[offset + (n << stride_log2)];
		float c = dct_coefficients[(n << BLOCK_SIZE_LOG2) + k];
		s += (v * c);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	block[offset + (k << stride_log2)] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void compute_idct_x(__local float* block, int x, int y) {
	compute_idct(block, y << BLOCK_SIZE_LOG2, 0, x);
}

void compute_idct_y(__local float* block, int x, int y) {
	compute_idct(block, x, BLOCK_SIZE_LOG2, y);
}

void compute_idct_xy(__local float* block, int x, int y) {
	compute_idct_y(block, x, y);
	compute_idct_x(block, x, y);
}

void copy_to_block(__local float* block, int x, int y, float s) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	block[offset] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

float block_avg(__local float* block) {
	float sum = 0.0f;
	for (int i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); i++) {
		sum += block[i];
	}
	sum /= (BLOCK_SIZE * BLOCK_SIZE);
	barrier(CLK_LOCAL_MEM_FENCE);
	return sum;
}

float block_avgspread(__local float* block) {
	float avg = block_avg(block);
	float sum = 0.0f;
	for (int i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); i++) {
		float x = block[i] - avg;
		sum += fabs(x);
	}
	sum /= (BLOCK_SIZE * BLOCK_SIZE);
	barrier(CLK_LOCAL_MEM_FENCE);
	return sum * 2.0f;
}

float block_mse(__local float* block) {
	float avg = block_avg(block);
	float sum = 0.0f;
	for (int i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); i++) {
		float x = (block[i] - avg);
		sum += (x * x);
	}
	sum /= (BLOCK_SIZE * BLOCK_SIZE);
	barrier(CLK_LOCAL_MEM_FENCE);
	return sum;
}

void block_abs(__local float* target, __local float* lhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = fabs(lhs[offset]);
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_add(__local float* target, __local float* lhs, __local float* rhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset] + rhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_addf(__local float* target, __local float* lhs, float rhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset] + rhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_sub(__local float* target, __local float* lhs, __local float* rhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset] - rhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_subf(__local float* target, __local float* lhs, float rhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset] - rhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_mul(__local float* target, __local float* lhs, __local float* rhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset] * rhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_mulf(__local float* target, __local float* lhs, float rhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset] * rhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void filter_block(__local float* block, int x, int y, float threshold) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	float s = block[offset];
	if (fabs(s) < threshold) {
		block[offset] = 0.0f;
	}
	barrier(CLK_LOCAL_MEM_FENCE);
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv2(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	int2 coords = { gid.x + x, gid.y + y };
	if (coords.x >= ss.x) {
		return;
	}
	if (coords.y >= ss.y) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;



	float s_prev = read_imagef(source_prev, sampler, coords).s0;
	__local float block_prev[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_prev, lid.x, lid.y, s_prev);

	float s = read_imagef(source, sampler, coords).s0;
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block, lid.x, lid.y, s);

	float s_next = read_imagef(source_next, sampler, coords).s0;
	__local float block_next[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_next, lid.x, lid.y, s_next);






	float s_prev_prefiltered = read_imagef(source_prev_prefiltered, sampler, coords).s0;
	__local float block_prev_prefiltered[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_prev_prefiltered, lid.x, lid.y, s_prev_prefiltered);

	float s_prefiltered = read_imagef(source_prefiltered, sampler, coords).s0;
	__local float block_prefiltered[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_prefiltered, lid.x, lid.y, s_prefiltered);

	float s_next_prefiltered = read_imagef(source_next_prefiltered, sampler, coords).s0;
	__local float block_next_prefiltered[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_next_prefiltered, lid.x, lid.y, s_next_prefiltered);





	__local float delta_prev[BLOCK_SIZE * BLOCK_SIZE];
	block_sub(delta_prev, block, block_prev, lid.x, lid.y);
	convert_to_absolute_range(delta_prev, lid.x, lid.y);
	float difference_delta_prev = block_avgspread(delta_prev); // 0 (identical) to 1 (inverted)


	__local float delta_next[BLOCK_SIZE * BLOCK_SIZE];
	block_sub(delta_next, block, block_next, lid.x, lid.y);
	convert_to_absolute_range(delta_next, lid.x, lid.y);
	float difference_delta_next = block_avgspread(delta_next); // 0 (identical) to 1 (inverted)





	compute_dct_xy(block_prev, lid.x, lid.y);
	compute_dct_xy(block, lid.x, lid.y);
	compute_dct_xy(block_next, lid.x, lid.y);
	__local float final_block[BLOCK_SIZE * BLOCK_SIZE];
	int offset = (lid.y << BLOCK_SIZE_LOG2) + lid.x;
	float scaler = (lid.x + lid.y) / 14.0f;
	float f0 = (1.0f - difference_delta_prev) * (1.0f - difference_delta_prev);
	float f1 = 1.0f;
	float f2 = (1.0f - difference_delta_next) * (1.0f - difference_delta_next);
	final_block[offset] = block[offset] * (1.0f - scaler) + (f0 * block_prev[offset] + f1 * block[offset] + f2 * block_next[offset]) / (f0 + f1 + f2) * scaler;
	barrier(CLK_LOCAL_MEM_FENCE);

	float threshold = assess_denoising_strength1(block);

	filter_block(final_block, lid.x, lid.y, max(0.01f, strength * threshold));
	compute_idct_xy(final_block, lid.x, lid.y);
	float t = final_block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}


__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv1(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	int2 coords = { gid.x + x, gid.y + y };
	if (coords.x >= ss.x) {
		return;
	}
	if (coords.y >= ss.y) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	float s_prev_prefiltered = read_imagef(source_prev_prefiltered, sampler, coords).s0;
	__local float block_prev_prefiltered[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_prev_prefiltered, lid.x, lid.y, s_prev_prefiltered);

	float s_prefiltered = read_imagef(source_prefiltered, sampler, coords).s0;
	__local float block_prefiltered[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_prefiltered, lid.x, lid.y, s_prefiltered);

	float s_next_prefiltered = read_imagef(source_next_prefiltered, sampler, coords).s0;
	__local float block_next_prefiltered[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_next_prefiltered, lid.x, lid.y, s_next_prefiltered);


	float s_prev = read_imagef(source_prev, sampler, coords).s0;
	__local float block_prev[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_prev, lid.x, lid.y, s_prev);

	float s = read_imagef(source, sampler, coords).s0;
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block, lid.x, lid.y, s);

	float s_next = read_imagef(source_next, sampler, coords).s0;
	__local float block_next[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_next, lid.x, lid.y, s_next);



	__local float delta_prev[BLOCK_SIZE * BLOCK_SIZE];
	block_sub(delta_prev, block, block_prev, lid.x, lid.y);
	convert_to_absolute_range(delta_prev, lid.x, lid.y);


	__local float delta_next[BLOCK_SIZE * BLOCK_SIZE];
	block_sub(delta_next, block, block_next, lid.x, lid.y);
	convert_to_absolute_range(delta_next, lid.x, lid.y);

	__local float delta_prefilter[BLOCK_SIZE * BLOCK_SIZE];
	block_sub(delta_prefilter, block, block_prefiltered, lid.x, lid.y);
	convert_to_absolute_range(delta_prefilter, lid.x, lid.y);





	compute_dct_xy(block, lid.x, lid.y);




	float factor = assess_denoising_strength1(block);


	filter_block(block, lid.x, lid.y, strength * factor);
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv0(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	int2 coords = { gid.x + x, gid.y + y };
	if (coords.x >= ss.x) {
		return;
	}
	if (coords.y >= ss.y) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	float s = read_imagef(source, sampler, coords).s0;
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block, lid.x, lid.y, s);
	compute_dct_xy(block, lid.x, lid.y);
	filter_block(block, lid.x, lid.y, strength);
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
normalize_kernel(__write_only image2d_t target, __global float* buffer) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	int2 ts = { get_image_width(target), get_image_height(target) };
	int2 coords = { gid.x, gid.y };
	if (coords.x >= ts.x) {
		return;
	}
	if (coords.y >= ts.y) {
		return;
	}
	float s = buffer[(coords.y * ts.x) + coords.x];
	int xf = get_scaling_factor(coords.x, ts.x);
	int yf = get_scaling_factor(coords.y, ts.y);
	float t = s / (float)(xf * yf);
	write_imagef(target, coords, (float4)(t));
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
dct_transform(__write_only image2d_t target, __read_only image2d_t source) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	if (gid.x >= get_image_width(source)) {
		return;
	}
	if (gid.y >= get_image_height(source)) {
		return;
	}
	if (gid.x >= get_image_width(target)) {
		return;
	}
	if (gid.y >= get_image_height(target)) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	float s = read_imagef(source, sampler, gid).s0;
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block, lid.x, lid.y, s);
	convert_to_relative_range(block, lid.x, lid.y);
	compute_dct_xy(block, lid.x, lid.y);
	block_mulf(block, block, 1.0f / (float)(BLOCK_SIZE * BLOCK_SIZE), lid.x, lid.y);
	convert_to_absolute_range(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	write_imagef(target, gid, (float4)(t));
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
idct_transform(__write_only image2d_t target, __read_only image2d_t source) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	if (gid.x >= get_image_width(source)) {
		return;
	}
	if (gid.y >= get_image_height(source)) {
		return;
	}
	if (gid.x >= get_image_width(target)) {
		return;
	}
	if (gid.y >= get_image_height(target)) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	float s = read_imagef(source, sampler, gid).s0;
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block, lid.x, lid.y, s);
	convert_to_relative_range(block, lid.x, lid.y);
	compute_idct_xy(block, lid.x, lid.y);
	block_mulf(block, block, 1.0f * (float)(BLOCK_SIZE * BLOCK_SIZE), lid.x, lid.y);
	convert_to_absolute_range(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	write_imagef(target, gid, (float4)(t));
}

__kernel void
lowpass_x_kernel(__write_only image2d_t target, __read_only image2d_t source) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	if (gid.x >= ss.x) {
		return;
	}
	if (gid.y >= ss.y) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	float s1 = read_imagef(source, sampler, (int2){ gid.x - 1, gid.y}).s0;
	float s2 = read_imagef(source, sampler, (int2){ gid.x + 0, gid.y}).s0;
	float s3 = read_imagef(source, sampler, (int2){ gid.x + 1, gid.y}).s0;
	float t = (s1 + s2 + s3) / 3.0f;
	write_imagef(target, gid, (float4)(t));
}

__kernel void
lowpass_y_kernel(__write_only image2d_t target, __read_only image2d_t source) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	if (gid.x >= ss.x) {
		return;
	}
	if (gid.y >= ss.y) {
		return;
	}
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	float s1 = read_imagef(source, sampler, (int2){ gid.x, gid.y - 1}).s0;
	float s2 = read_imagef(source, sampler, (int2){ gid.x, gid.y + 0}).s0;
	float s3 = read_imagef(source, sampler, (int2){ gid.x, gid.y + 1}).s0;
	float t = (s1 + s2 + s3) / 3.0f;
	write_imagef(target, gid, (float4)(t));
}

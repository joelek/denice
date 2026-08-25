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

void block_copy(__local float* target, __local float* lhs, int x, int y) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	target[offset] = lhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void filter_block(__local float* block, int x, int y, float threshold) {
	int offset = (y << BLOCK_SIZE_LOG2) + x;
	float s = block[offset];
	if (offset > 0 && fabs(s) < fabs(threshold)) {
		block[offset] = 0.0f;
	}
	barrier(CLK_LOCAL_MEM_FENCE);
}

float determine_strength_filter_kernelv7(__local float* block, int start, int stop) {
	float acs[BLOCK_MAX + BLOCK_MAX + 1];
	float max_value = 0.0f;
	float total_ac = 0.0f;
	int count_ac = 0;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		acs[i] = 0.0f;
	}
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				acs[x + y] += value;
				total_ac += value;
				count_ac += 1;
				if (value > max_value) {
					max_value = value;
				}
			}
		}
	}
	float sum = 0.0f;
	float sum_weights = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		sum += acs[i] * i;
		sum_weights += acs[i];
	}
	float index = sum / sum_weights;
	float avg_ac = total_ac / count_ac;
	float max_ac_diag = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		float value = acs[i] / (8 - abs(7 - i));
		if (value > max_ac_diag) {
			max_ac_diag = value;
		}
	}
	return avg_ac / max_ac_diag;
};

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv7(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block, lid.x, lid.y, read_imagef(source, sampler, coords).s0);
	__local float block_target_prev[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(block_target_prev, lid.x, lid.y, read_imagef(target_prev, sampler, coords).s0);



	compute_dct_xy(block, lid.x, lid.y);
	compute_dct_xy(block_target_prev, lid.x, lid.y);

	__local float block2[BLOCK_SIZE * BLOCK_SIZE];
	block_copy(block2, block, lid.x, lid.y);

	float flatness = determine_strength_filter_kernelv7(block, 1, 14);
	filter_block(block, lid.x, lid.y, min_strength * 0.5f);
	filter_block(block2, lid.x, lid.y, min_strength * 5.0f);

	block_mulf(block, block, 1.0f - flatness, lid.x, lid.y);
	block_mulf(block2, block2, flatness, lid.x, lid.y);
	block_add(block, block, block2, lid.x, lid.y);



	int offset = (lid.y << BLOCK_SIZE_LOG2) + lid.x;
	float factor = min((float)(lid.x + lid.y) / (BLOCK_MAX + BLOCK_MAX), 1.0f) * min(0.25f + flatness, 1.0f);
	block[offset] = block_target_prev[offset] * factor + block[offset] * (1.0f - factor);
	barrier(CLK_LOCAL_MEM_FENCE);



	compute_idct_xy(block, lid.x, lid.y);


	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

float determine_strength_filter_kernelv6(__local float* block, int start, int stop) {
	float acs[BLOCK_MAX + BLOCK_MAX + 1];
	float max_value = 0.0f;
	float total_ac = 0.0f;
	int count_ac = 0;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		acs[i] = 0.0f;
	}
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				acs[x + y] += value;
				total_ac += value;
				count_ac += 1;
				if (value > max_value) {
					max_value = value;
				}
			}
		}
	}
	float sum = 0.0f;
	float sum_weights = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		sum += acs[i] * i;
		sum_weights += acs[i];
	}
	float index = sum / sum_weights;
	float avg_ac = total_ac / count_ac;
	float max_ac_diag = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		float value = acs[i] / (8 - abs(7 - i));
		if (value > max_ac_diag) {
			max_ac_diag = value;
		}
	}
	return avg_ac / max_ac_diag * 5.0f;
};

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv6(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	float strength = determine_strength_filter_kernelv6(block, 1, 14) * min_strength;
	filter_block(block, lid.x, lid.y, strength);
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

float determine_strength_filter_kernelv5(__local float* block, int start, int stop) {
	float acs[BLOCK_MAX + BLOCK_MAX + 1];
	float max_value = 0.0f;
	float total_ac = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		acs[i] = 0.0f;
	}
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				acs[x + y] += value;
				total_ac += value;
				if (value > max_value) {
					max_value = value;
				}
			}
		}
	}
	float sum = 0.0f;
	float sum_weights = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		sum += acs[i] * i;
		sum_weights += acs[i];
	}
	float index = sum / sum_weights;
	return max_value / acs[start];
};

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv5(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	float strength = determine_strength_filter_kernelv5(block, 1, 14) * min_strength;
	filter_block(block, lid.x, lid.y, max(0.0f, min(strength, min_strength * 2.0f)));
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

float determine_strength_filter_kernelv4(__local float* block, int start, int stop) {
	float acs[BLOCK_MAX + BLOCK_MAX + 1];
	float max_value = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		acs[i] = 0.0f;
	}
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				acs[x + y] += value;
				if (value > max_value) {
					max_value = value;
				}
			}
		}
	}
	float sum = 0.0f;
	float sum_weights = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		sum += acs[i] * i;
		sum_weights += acs[i];
	}
	float index = sum / sum_weights;
	return (float)(index - start) / (stop - start + 1) * max_value / acs[start] * 5.0f;
};

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv4(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	float strength = determine_strength_filter_kernelv4(block, 1, 14) * min_strength;
	barrier(CLK_LOCAL_MEM_FENCE);
	filter_block(block, lid.x, lid.y, max(0.0f, strength));
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

float determine_strength_filter_kernelv3(__local float* block, int start, int stop) {
	float x_acs[BLOCK_SIZE];
	float y_acs[BLOCK_SIZE];
	for (int i = 0; i < BLOCK_SIZE; i++) {
		x_acs[i] = 0.0f;
		y_acs[i] = 0.0f;
	}
	float total_ac = 0.0;
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				x_acs[x] += value;
				y_acs[y] += value;
				total_ac += value;
			}
		}
	}
	int x_range_start = 0;
	int x_range_end = BLOCK_MAX;
	float x_ac_energy_outside_range = 0.0f;
	float x_ac_energy_inside_range = total_ac;
	while (x_range_start + 1 < x_range_end && x_ac_energy_inside_range > total_ac * 0.5f) {
		float lf = x_acs[x_range_start];
		float hf = x_acs[x_range_end];
		if (lf <= hf) {
			x_range_start += 1;
			x_ac_energy_outside_range += lf;
			x_ac_energy_inside_range -= lf;
		}
		if (hf <= lf) {
			x_range_end -= 1;
			x_ac_energy_outside_range += hf;
			x_ac_energy_inside_range -= hf;
		}
	}
	float x_sum = 0.0f;
	float x_sum_weights = 0.0f;
	for (int i = x_range_start; i <= x_range_end; i++) {
		x_sum += x_acs[i] * i;
		x_sum_weights += x_acs[i];
	}
	float x_index = x_sum / x_sum_weights;
	int y_range_start = 0;
	int y_range_end = BLOCK_MAX;
	float y_ac_energy_outside_range = 0.0f;
	float y_ac_energy_inside_range = total_ac;
	while (y_range_start + 1 < y_range_end && y_ac_energy_inside_range > total_ac * 0.5f) {
		float lf = y_acs[y_range_start];
		float hf = y_acs[y_range_end];
		if (lf <= hf) {
			y_range_start += 1;
			y_ac_energy_outside_range += lf;
			y_ac_energy_inside_range -= lf;
		}
		if (hf <= lf) {
			y_range_end -= 1;
			y_ac_energy_outside_range += hf;
			y_ac_energy_inside_range -= hf;
		}
	}
	float y_sum = 0.0f;
	float y_sum_weights = 0.0f;
	for (int i = y_range_start; i <= y_range_end; i++) {
		y_sum += y_acs[i] * i;
		y_sum_weights += y_acs[i];
	}
	float y_index = y_sum / y_sum_weights;
	return pow((1 + x_index + y_index - start) / (stop - start + 1), 2.0f);
};

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv3(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	float strength = determine_strength_filter_kernelv3(block, 1, 14);
	filter_block(block, lid.x, lid.y, max(min_strength, strength));
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

float determine_strength_filter_kernelv2(__local float* block, int start, int stop) {
	float acs[BLOCK_MAX + BLOCK_MAX + 1];
	float total_ac = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		acs[i] = 0.0f;
	}
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				acs[x + y] += value;
				total_ac += value;
			}
		}
	}
	int range_start = 0;
	int range_end = BLOCK_MAX + BLOCK_MAX;
	float ac_energy_outside_range = 0.0f;
	float ac_energy_inside_range = total_ac;
	while (range_start + 1 < range_end && ac_energy_inside_range > total_ac * 0.5f) {
		float lf = acs[range_start];
		float hf = acs[range_end];
		if (lf <= hf) {
			range_start += 1;
			ac_energy_outside_range += lf;
			ac_energy_inside_range -= lf;
		}
		if (hf <= lf) {
			range_end -= 1;
			ac_energy_outside_range += hf;
			ac_energy_inside_range -= hf;
		}
	}
	float sum = 0.0f;
	float sum_weights = 0.0f;
	for (int i = range_start; i <= range_end; i++) {
		sum += acs[i] * i;
		sum_weights += acs[i];
	}
	float index = sum / sum_weights;
	return pow((index - start) / (stop - start + 1), 2.0f) / total_ac;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv2(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	float strength = determine_strength_filter_kernelv2(block, 1, 14);
	filter_block(block, lid.x, lid.y, max(min_strength, strength));
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

float determine_strength_filter_kernelv1(__local float* block, int start, int stop) {
	float acs[BLOCK_MAX + BLOCK_MAX + 1];
	float total_ac = 0.0f;
	for (int i = 0; i < BLOCK_MAX + BLOCK_MAX + 1; i++) {
		acs[i] = 0.0f;
	}
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			if (x + y >= start && x + y <= stop) {
				float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
				acs[x + y] += value;
				total_ac += value;
			}
		}
	}
	int range_start = 0;
	int range_end = BLOCK_MAX + BLOCK_MAX;
	float ac_energy_outside_range = 0.0f;
	float ac_energy_inside_range = total_ac;
	while (range_start + 1 < range_end && ac_energy_inside_range > total_ac * 0.5f) {
		float lf = acs[range_start];
		float hf = acs[range_end];
		if (lf <= hf) {
			range_start += 1;
			ac_energy_outside_range += lf;
			ac_energy_inside_range -= lf;
		}
		if (hf <= lf) {
			range_end -= 1;
			ac_energy_outside_range += hf;
			ac_energy_inside_range -= hf;
		}
	}
	float sum = 0.0f;
	float sum_weights = 0.0f;
	for (int i = range_start; i <= range_end; i++) {
		sum += acs[i] * i;
		sum_weights += acs[i];
	}
	float index = sum / sum_weights;
	return pow((index - start) / (stop - start + 1), 2.0f);
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv1(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	float strength = determine_strength_filter_kernelv1(block, 1, 14);
	filter_block(block, lid.x, lid.y, max(min_strength, strength));
	compute_idct_xy(block, lid.x, lid.y);
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernelv0(__global float* buffer, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, int x, int y, float min_strength, __read_only image2d_t source_prev_prefiltered, __read_only image2d_t source_prefiltered, __read_only image2d_t source_next_prefiltered, __read_only image2d_t target_prev) {
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
	filter_block(block, lid.x, lid.y, min_strength);
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

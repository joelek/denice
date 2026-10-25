__constant float dct_coefficients_8x8[64] = {
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

__constant float dct_coefficients_16x16[256] = {
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.25000000000000000000f,
	 0.35185093438159564805f,
	 0.33832950029358815902f,
	 0.31180625324666783049f,
	 0.27330046675043939697f,
	 0.22429189658565909515f,
	 0.16666391461943669272f,
	 0.10263113188058933800f,
	 0.03465429229977292497f,
	-0.03465429229977288333f,
	-0.10263113188058928249f,
	-0.16666391461943666497f,
	-0.22429189658565903964f,
	-0.27330046675043939697f,
	-0.31180625324666777498f,
	-0.33832950029358815902f,
	-0.35185093438159559254f,
	 0.34675996133053688109f,
	 0.29396890060483971130f,
	 0.19642373959677558726f,
	 0.06897484482073577838f,
	-0.06897484482073573675f,
	-0.19642373959677547623f,
	-0.29396890060483971130f,
	-0.34675996133053688109f,
	-0.34675996133053688109f,
	-0.29396890060483976681f,
	-0.19642373959677553175f,
	-0.06897484482073590328f,
	 0.06897484482073576451f,
	 0.19642373959677542072f,
	 0.29396890060483971130f,
	 0.34675996133053682557f,
	 0.33832950029358815902f,
	 0.22429189658565909515f,
	 0.03465429229977292497f,
	-0.16666391461943666497f,
	-0.31180625324666777498f,
	-0.35185093438159564805f,
	-0.27330046675043945248f,
	-0.10263113188058937963f,
	 0.10263113188058924086f,
	 0.27330046675043923043f,
	 0.35185093438159564805f,
	 0.31180625324666783049f,
	 0.16666391461943674823f,
	-0.03465429229977264047f,
	-0.22429189658565884535f,
	-0.33832950029358804800f,
	 0.32664074121909414394f,
	 0.13529902503654928081f,
	-0.13529902503654925305f,
	-0.32664074121909414394f,
	-0.32664074121909419945f,
	-0.13529902503654944734f,
	 0.13529902503654933632f,
	 0.32664074121909408843f,
	 0.32664074121909414394f,
	 0.13529902503654950285f,
	-0.13529902503654930856f,
	-0.32664074121909403292f,
	-0.32664074121909414394f,
	-0.13529902503654953061f,
	 0.13529902503654925305f,
	 0.32664074121909403292f,
	 0.31180625324666783049f,
	 0.03465429229977292497f,
	-0.27330046675043939697f,
	-0.33832950029358821453f,
	-0.10263113188058937963f,
	 0.22429189658565912291f,
	 0.35185093438159564805f,
	 0.16666391461943674823f,
	-0.16666391461943658170f,
	-0.35185093438159564805f,
	-0.22429189658565903964f,
	 0.10263113188058946290f,
	 0.33832950029358804800f,
	 0.27330046675043961901f,
	-0.03465429229977255721f,
	-0.31180625324666771947f,
	 0.29396890060483971130f,
	-0.06897484482073573675f,
	-0.34675996133053688109f,
	-0.19642373959677553175f,
	 0.19642373959677542072f,
	 0.34675996133053688109f,
	 0.06897484482073593104f,
	-0.29396890060483965579f,
	-0.29396890060483982232f,
	 0.06897484482073537593f,
	 0.34675996133053688109f,
	 0.19642373959677567052f,
	-0.19642373959677530970f,
	-0.34675996133053693660f,
	-0.06897484482073637513f,
	 0.29396890060483926721f,
	 0.27330046675043939697f,
	-0.16666391461943666497f,
	-0.33832950029358821453f,
	 0.03465429229977268905f,
	 0.35185093438159564805f,
	 0.10263113188058942127f,
	-0.31180625324666777498f,
	-0.22429189658565903964f,
	 0.22429189658565881760f,
	 0.31180625324666794151f,
	-0.10263113188058942127f,
	-0.35185093438159564805f,
	-0.03465429229977361192f,
	 0.33832950029358821453f,
	 0.16666391461943691477f,
	-0.27330046675043895288f,
	 0.25000000000000005551f,
	-0.25000000000000000000f,
	-0.25000000000000005551f,
	 0.24999999999999994449f,
	 0.25000000000000005551f,
	-0.24999999999999972244f,
	-0.24999999999999988898f,
	 0.24999999999999969469f,
	 0.24999999999999991673f,
	-0.24999999999999963918f,
	-0.24999999999999994449f,
	 0.24999999999999963918f,
	 0.25000000000000000000f,
	-0.24999999999999961142f,
	-0.25000000000000005551f,
	 0.24999999999999955591f,
	 0.22429189658565909515f,
	-0.31180625324666777498f,
	-0.10263113188058937963f,
	 0.35185093438159564805f,
	-0.03465429229977264047f,
	-0.33832950029358815902f,
	 0.16666391461943652619f,
	 0.27330046675043961901f,
	-0.27330046675043934146f,
	-0.16666391461943685925f,
	 0.33832950029358821453f,
	 0.03465429229977365355f,
	-0.35185093438159570356f,
	 0.10263113188058929637f,
	 0.31180625324666827458f,
	-0.22429189658565865106f,
	 0.19642373959677558726f,
	-0.34675996133053688109f,
	 0.06897484482073576451f,
	 0.29396890060483982232f,
	-0.29396890060483965579f,
	-0.06897484482073566736f,
	 0.34675996133053693660f,
	-0.19642373959677530970f,
	-0.19642373959677569828f,
	 0.34675996133053688109f,
	-0.06897484482073586165f,
	-0.29396890060483960028f,
	 0.29396890060483921170f,
	 0.06897484482073650003f,
	-0.34675996133053699211f,
	 0.19642373959677514317f,
	 0.16666391461943669272f,
	-0.35185093438159564805f,
	 0.22429189658565887311f,
	 0.10263113188058971270f,
	-0.33832950029358832555f,
	 0.27330046675043900839f,
	 0.03465429229977357029f,
	-0.31180625324666821907f,
	 0.31180625324666738640f,
	-0.03465429229977184250f,
	-0.27330046675044011861f,
	 0.33832950029358782595f,
	-0.10263113188058806124f,
	-0.22429189658566023313f,
	 0.35185093438159548151f,
	-0.16666391461943516616f,
	 0.13529902503654928081f,
	-0.32664074121909419945f,
	 0.32664074121909408843f,
	-0.13529902503654930856f,
	-0.13529902503654953061f,
	 0.32664074121909414394f,
	-0.32664074121909403292f,
	 0.13529902503654916979f,
	 0.13529902503654964163f,
	-0.32664074121909447701f,
	 0.32664074121909419945f,
	-0.13529902503654905876f,
	-0.13529902503654978041f,
	 0.32664074121909447701f,
	-0.32664074121909369985f,
	 0.13529902503654775425f,
	 0.10263113188058933800f,
	-0.27330046675043934146f,
	 0.35185093438159559254f,
	-0.31180625324666805254f,
	 0.16666391461943652619f,
	 0.03465429229977232128f,
	-0.22429189658565912291f,
	 0.33832950029358804800f,
	-0.33832950029358821453f,
	 0.22429189658565962251f,
	-0.03465429229977300823f,
	-0.16666391461943591557f,
	 0.31180625324666771947f,
	-0.35185093438159559254f,
	 0.27330046675044039617f,
	-0.10263113188059029557f,
	 0.06897484482073577838f,
	-0.19642373959677553175f,
	 0.29396890060483982232f,
	-0.34675996133053693660f,
	 0.34675996133053688109f,
	-0.29396890060483960028f,
	 0.19642373959677530970f,
	-0.06897484482073586165f,
	-0.06897484482073645840f,
	 0.19642373959677578155f,
	-0.29396890060483960028f,
	 0.34675996133053704762f,
	-0.34675996133053660353f,
	 0.29396890060483982232f,
	-0.19642373959677503215f,
	 0.06897484482073433509f,
	 0.03465429229977292497f,
	-0.10263113188058967107f,
	 0.16666391461943674823f,
	-0.22429189658565951149f,
	 0.27330046675043961901f,
	-0.31180625324666821907f,
	 0.33832950029358838107f,
	-0.35185093438159570356f,
	 0.35185093438159548151f,
	-0.33832950029358782595f,
	 0.31180625324666733089f,
	-0.27330046675043800919f,
	 0.22429189658565854004f,
	-0.16666391461943508290f,
	 0.10263113188058664571f,
	-0.03465429229977150249f
};

void convert_to_relative_range(int block_size_log_2, __local float* target, __local float* lhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	float s = lhs[offset];
	s = (s * 2.0f) - 1.0f;
	target[offset] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void convert_to_absolute_range(int block_size_log_2, __local float* target, __local float* lhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	float s = lhs[offset];
	s = (s + 1.0f) * 0.5f;
	target[offset] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void compute_dct(int block_size_log_2, __constant float* coefficients, __local float* block, int offset, int stride_log2, int k) {
	float s = 0.0f;
	int block_size = (1 << block_size_log_2);
	for (int n = 0; n < block_size; n++) {
		float v = block[offset + (n << stride_log2)];
		float c = coefficients[(k << block_size_log_2) + n];
		s += (v * c);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	block[offset + (k << stride_log2)] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void compute_dct_x(int block_size_log_2, __constant float* coefficients, __local float* block, int x, int y) {
	compute_dct(block_size_log_2, coefficients, block, y << block_size_log_2, 0, x);
}

void compute_dct_y(int block_size_log_2, __constant float* coefficients, __local float* block, int x, int y) {
	compute_dct(block_size_log_2, coefficients, block, x, block_size_log_2, y);
}

void compute_dct_xy(int block_size_log_2, __constant float* coefficients, __local float* block, int x, int y) {
	compute_dct_x(block_size_log_2, coefficients, block, x, y);
	compute_dct_y(block_size_log_2, coefficients, block, x, y);
}

void compute_idct(int block_size_log_2, __constant float* coefficients, __local float* block, int offset, int stride_log2, int k) {
	float s = 0.0f;
	int block_size = (1 << block_size_log_2);
	for (int n = 0; n < block_size; n++) {
		float v = block[offset + (n << stride_log2)];
		float c = coefficients[(n << block_size_log_2) + k];
		s += (v * c);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	block[offset + (k << stride_log2)] = s;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void compute_idct_x(int block_size_log_2, __constant float* coefficients, __local float* block, int x, int y) {
	compute_idct(block_size_log_2, coefficients, block, y << block_size_log_2, 0, x);
}

void compute_idct_y(int block_size_log_2, __constant float* coefficients, __local float* block, int x, int y) {
	compute_idct(block_size_log_2, coefficients, block, x, block_size_log_2, y);
}

void compute_idct_xy(int block_size_log_2, __constant float* coefficients, __local float* block, int x, int y) {
	compute_idct_y(block_size_log_2, coefficients, block, x, y);
	compute_idct_x(block_size_log_2, coefficients, block, x, y);
}

void copy_to_block(int block_size_log_2, __local float* target, float lhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

float block_avg(int block_size_log_2, __local float* block) {
	float sum = 0.0f;
	int block_size = (1 << block_size_log_2);
	for (int i = 0; i < block_size * block_size; i++) {
		sum += block[i];
	}
	sum /= (block_size * block_size);
	barrier(CLK_LOCAL_MEM_FENCE);
	return sum;
}

float block_variance(int block_size_log_2, __local float* block) {
	float avg = block_avg(block_size_log_2, block);
	float sum = 0.0f;
	int block_size = (1 << block_size_log_2);
	for (int i = 0; i < block_size * block_size; i++) {
		float x = (block[i] - avg);
		sum += (x * x);
	}
	sum /= (block_size * block_size);
	barrier(CLK_LOCAL_MEM_FENCE);
	return sum;
}

float block_stddev(int block_size_log_2, __local float* block) {
	return sqrt(block_variance(block_size_log_2, block));
}

void block_abs(int block_size_log_2, __local float* target, __local float* lhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = fabs(lhs[offset]);
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_add(int block_size_log_2, __local float* target, __local float* lhs, __local float* rhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset] + rhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_addf(int block_size_log_2, __local float* target, __local float* lhs, float rhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset] + rhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_sub(int block_size_log_2, __local float* target, __local float* lhs, __local float* rhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset] - rhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_subf(int block_size_log_2, __local float* target, __local float* lhs, float rhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset] - rhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_mul(int block_size_log_2, __local float* target, __local float* lhs, __local float* rhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset] * rhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_mulf(int block_size_log_2, __local float* target, __local float* lhs, float rhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset] * rhs;
	barrier(CLK_LOCAL_MEM_FENCE);
}

void block_copy(int block_size_log_2, __local float* target, __local float* lhs, int x, int y) {
	int offset = (y << block_size_log_2) + x;
	target[offset] = lhs[offset];
	barrier(CLK_LOCAL_MEM_FENCE);
}

void filter_block(int block_size_log_2, __local float* target, __local float* lhs, int x, int y, float threshold) {
	int offset = (y << block_size_log_2) + x;
	float s = lhs[offset];
	if (offset > 0 && fabs(s) < fabs(threshold)) {
		target[offset] = 0.0f;
	}
	barrier(CLK_LOCAL_MEM_FENCE);
}

#define BLOCK_SIZE_LOG2 3
#define BLOCK_SIZE (1 << BLOCK_SIZE_LOG2)

float block_flatness_8x8(__local float* block) {
	float acs[BLOCK_SIZE];
	for (int i = 0; i < BLOCK_SIZE; i++) {
		acs[i] = 0.0f;
	}
	float total_ac = 0.0f;
	int count_ac = 0;
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
			if (x + y > 0 && x + y < BLOCK_SIZE) {
				acs[x + y] += value;
				total_ac += value;
				count_ac += 1;
			}
		}
	}
	float avg_ac = total_ac / count_ac;
	float max_diag_ac = 0.0f;
	for (int i = 0; i < BLOCK_SIZE; i++) {
		max_diag_ac = max(acs[i], max_diag_ac);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	return avg_ac / max_diag_ac;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernel_8x8(__global float* buffer, __global float* counter, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, float min_strength, float max_strength, int x, int y) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	int2 coords = { gid.x - x, gid.y - y };
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	__local float block1[BLOCK_SIZE * BLOCK_SIZE];
	__local float block2[BLOCK_SIZE * BLOCK_SIZE];
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(BLOCK_SIZE_LOG2, block1, read_imagef(source, sampler, coords).s0, lid.x, lid.y);
	compute_dct_xy(BLOCK_SIZE_LOG2, dct_coefficients_8x8, block1, lid.x, lid.y);
	block_copy(BLOCK_SIZE_LOG2, block2, block1, lid.x, lid.y);
	filter_block(BLOCK_SIZE_LOG2, block1, block1, lid.x, lid.y, min_strength);
	filter_block(BLOCK_SIZE_LOG2, block2, block2, lid.x, lid.y, max_strength);
	block_add(BLOCK_SIZE_LOG2, block, block1, block2, lid.x, lid.y);
	block_mulf(BLOCK_SIZE_LOG2, block, block, 0.5f, lid.x, lid.y);
	compute_idct_xy(BLOCK_SIZE_LOG2, dct_coefficients_8x8, block, lid.x, lid.y);
	if (coords.x < 0 || coords.x >= ss.x) {
		return;
	}
	if (coords.y < 0 || coords.y >= ss.y) {
		return;
	}
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
	counter[(coords.y * ss.x) + coords.x] += 1;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
normalize_kernel_8x8(__write_only image2d_t target, __global float* buffer, __global float* counter) {
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
	float n = counter[(coords.y * ts.x) + coords.x];
	float t = s / n;
	write_imagef(target, coords, (float4)(t));
}

#undef BLOCK_SIZE_LOG2
#undef BLOCK_SIZE

#define BLOCK_SIZE_LOG2 4
#define BLOCK_SIZE (1 << BLOCK_SIZE_LOG2)

float block_flatness_16x16(__local float* block) {
	float acs[BLOCK_SIZE];
	for (int i = 0; i < BLOCK_SIZE; i++) {
		acs[i] = 0.0f;
	}
	float total_ac = 0.0f;
	int count_ac = 0;
	for (int y = 0; y < BLOCK_SIZE; y++) {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			float value = fabs(block[(y << BLOCK_SIZE_LOG2) + x]);
			if (x + y > 0 && x + y < BLOCK_SIZE) {
				acs[x + y] += value;
				total_ac += value;
				count_ac += 1;
			}
		}
	}
	float avg_ac = total_ac / count_ac;
	float max_diag_ac = 0.0f;
	for (int i = 0; i < BLOCK_SIZE; i++) {
		max_diag_ac = max(acs[i], max_diag_ac);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	return avg_ac / max_diag_ac;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
filter_kernel_16x16(__global float* buffer, __global float* counter, __read_only image2d_t source_prev, __read_only image2d_t source, __read_only image2d_t source_next, float min_strength, float max_strength, int x, int y) {
	int2 gid = { get_global_id(0), get_global_id(1) };
	int2 lid = { get_local_id(0), get_local_id(1) };
	int2 ss = { get_image_width(source), get_image_height(source) };
	int2 coords = { gid.x - x, gid.y - y };
	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
	__local float block1[BLOCK_SIZE * BLOCK_SIZE];
	__local float block2[BLOCK_SIZE * BLOCK_SIZE];
	__local float block[BLOCK_SIZE * BLOCK_SIZE];
	copy_to_block(BLOCK_SIZE_LOG2, block1, read_imagef(source, sampler, coords).s0, lid.x, lid.y);
	compute_dct_xy(BLOCK_SIZE_LOG2, dct_coefficients_16x16, block1, lid.x, lid.y);
	block_copy(BLOCK_SIZE_LOG2, block2, block1, lid.x, lid.y);
	filter_block(BLOCK_SIZE_LOG2, block1, block1, lid.x, lid.y, min_strength);
	filter_block(BLOCK_SIZE_LOG2, block2, block2, lid.x, lid.y, max_strength);
	block_add(BLOCK_SIZE_LOG2, block, block1, block2, lid.x, lid.y);
	block_mulf(BLOCK_SIZE_LOG2, block, block, 0.5f, lid.x, lid.y);
	compute_idct_xy(BLOCK_SIZE_LOG2, dct_coefficients_16x16, block, lid.x, lid.y);
	if (coords.x < 0 || coords.x >= ss.x) {
		return;
	}
	if (coords.y < 0 || coords.y >= ss.y) {
		return;
	}
	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];
	buffer[(coords.y * ss.x) + coords.x] += t;
	counter[(coords.y * ss.x) + coords.x] += 1;
}

__kernel void
__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
normalize_kernel_16x16(__write_only image2d_t target, __global float* buffer, __global float* counter) {
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
	float n = counter[(coords.y * ts.x) + coords.x];
	float t = s / n;
	write_imagef(target, coords, (float4)(t));
}

#undef BLOCK_SIZE_LOG2
#undef BLOCK_SIZE

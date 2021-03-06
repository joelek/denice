const char* dct_denoise = ""
"#define BLOCK_SIZE_LOG2 3\n"
"#define BLOCK_SIZE (1 << BLOCK_SIZE_LOG2)\n"
"\n"
"__constant float dct_coefficients[64] = {\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.35355339059327378637f,\n"
"	 0.49039264020161521529f,\n"
"	 0.41573480615127261784f,\n"
"	 0.27778511650980114434f,\n"
"	 0.09754516100806416568f,\n"
"	-0.09754516100806409629f,\n"
"	-0.27778511650980097780f,\n"
"	-0.41573480615127267335f,\n"
"	-0.49039264020161521529f,\n"
"	 0.46193976625564336924f,\n"
"	 0.19134171618254491865f,\n"
"	-0.19134171618254486313f,\n"
"	-0.46193976625564336924f,\n"
"	-0.46193976625564342475f,\n"
"	-0.19134171618254516845f,\n"
"	 0.19134171618254500191f,\n"
"	 0.46193976625564325822f,\n"
"	 0.41573480615127261784f,\n"
"	-0.09754516100806409629f,\n"
"	-0.49039264020161521529f,\n"
"	-0.27778511650980108882f,\n"
"	 0.27778511650980092229f,\n"
"	 0.49039264020161521529f,\n"
"	 0.09754516100806438772f,\n"
"	-0.41573480615127256232f,\n"
"	 0.35355339059327378637f,\n"
"	-0.35355339059327373086f,\n"
"	-0.35355339059327384188f,\n"
"	 0.35355339059327367535f,\n"
"	 0.35355339059327384188f,\n"
"	-0.35355339059327334228f,\n"
"	-0.35355339059327356432f,\n"
"	 0.35355339059327328677f,\n"
"	 0.27778511650980114434f,\n"
"	-0.49039264020161521529f,\n"
"	 0.09754516100806415180f,\n"
"	 0.41573480615127278437f,\n"
"	-0.41573480615127256232f,\n"
"	-0.09754516100806401302f,\n"
"	 0.49039264020161532631f,\n"
"	-0.27778511650980075576f,\n"
"	 0.19134171618254491865f,\n"
"	-0.46193976625564342475f,\n"
"	 0.46193976625564325822f,\n"
"	-0.19134171618254494640f,\n"
"	-0.19134171618254527947f,\n"
"	 0.46193976625564336924f,\n"
"	-0.46193976625564320271f,\n"
"	 0.19134171618254477987f,\n"
"	 0.09754516100806416568f,\n"
"	-0.27778511650980108882f,\n"
"	 0.41573480615127278437f,\n"
"	-0.49039264020161532631f,\n"
"	 0.49039264020161521529f,\n"
"	-0.41573480615127250681f,\n"
"	 0.27778511650980075576f,\n"
"	-0.09754516100806429058f\n"
"};\n"
"\n"
"int get_scaling_factor(int coordinate, int size) {\n"
"	if (coordinate < 0) {\n"
"		return 1;\n"
"	} else if (coordinate < BLOCK_SIZE) {\n"
"		return coordinate + 1;\n"
"	} else if (coordinate <= size - BLOCK_SIZE) {\n"
"		return BLOCK_SIZE;\n"
"	} else if (coordinate < size) {\n"
"		return size - coordinate;\n"
"	} else {\n"
"		return 1;\n"
"	}\n"
"}\n"
"\n"
"void convert_to_relative_range(__local float* block, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	float s = block[offset];\n"
"	s = (s * 2.0f) - 1.0f;\n"
"	block[offset] = s;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void convert_to_absolute_range(__local float* block, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	float s = block[offset];\n"
"	s = (s + 1.0f) * 0.5f;\n"
"	block[offset] = s;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void compute_dct(__local float* block, int offset, int stride_log2, int k) {\n"
"	float s = 0.0f;\n"
"	for (int n = 0; n < BLOCK_SIZE; n++) {\n"
"		float v = block[offset + (n << stride_log2)];\n"
"		float c = dct_coefficients[(k << BLOCK_SIZE_LOG2) + n];\n"
"		s += (v * c);\n"
"	}\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"	block[offset + (k << stride_log2)] = s;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void compute_dct_x(__local float* block, int x, int y) {\n"
"	compute_dct(block, y << BLOCK_SIZE_LOG2, 0, x);\n"
"}\n"
"\n"
"void compute_dct_y(__local float* block, int x, int y) {\n"
"	compute_dct(block, x, BLOCK_SIZE_LOG2, y);\n"
"}\n"
"\n"
"void compute_dct_xy(__local float* block, int x, int y) {\n"
"	compute_dct_x(block, x, y);\n"
"	compute_dct_y(block, x, y);\n"
"}\n"
"\n"
"void compute_idct(__local float* block, int offset, int stride_log2, int k) {\n"
"	float s = 0.0f;\n"
"	for (int n = 0; n < BLOCK_SIZE; n++) {\n"
"		float v = block[offset + (n << stride_log2)];\n"
"		float c = dct_coefficients[(n << BLOCK_SIZE_LOG2) + k];\n"
"		s += (v * c);\n"
"	}\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"	block[offset + (k << stride_log2)] = s;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void compute_idct_x(__local float* block, int x, int y) {\n"
"	compute_idct(block, y << BLOCK_SIZE_LOG2, 0, x);\n"
"}\n"
"\n"
"void compute_idct_y(__local float* block, int x, int y) {\n"
"	compute_idct(block, x, BLOCK_SIZE_LOG2, y);\n"
"}\n"
"\n"
"void compute_idct_xy(__local float* block, int x, int y) {\n"
"	compute_idct_y(block, x, y);\n"
"	compute_idct_x(block, x, y);\n"
"}\n"
"\n"
"void copy_to_block(__local float* block, int x, int y, float s) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	block[offset] = s;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"float block_avg(__local float* block) {\n"
"	float sum = 0.0f;\n"
"	for (int i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); i++) {\n"
"		sum += block[i];\n"
"	}\n"
"	sum /= (BLOCK_SIZE * BLOCK_SIZE);\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"	return sum;\n"
"}\n"
"\n"
"float block_mse(__local float* block) {\n"
"	float avg = block_avg(block);\n"
"	float sum = 0.0f;\n"
"	for (int i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); i++) {\n"
"		float x = (block[i] - avg);\n"
"		sum += (x * x);\n"
"	}\n"
"	sum /= (BLOCK_SIZE * BLOCK_SIZE);\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"	return sum;\n"
"}\n"
"\n"
"void block_abs(__local float* target, __local float* lhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = fabs(lhs[offset]);\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void block_add(__local float* target, __local float* lhs, __local float* rhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = lhs[offset] + rhs[offset];\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void block_addf(__local float* target, __local float* lhs, float rhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = lhs[offset] + rhs;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void block_sub(__local float* target, __local float* lhs, __local float* rhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = lhs[offset] - rhs[offset];\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void block_subf(__local float* target, __local float* lhs, float rhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = lhs[offset] - rhs;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void block_mul(__local float* target, __local float* lhs, __local float* rhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = lhs[offset] * rhs[offset];\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void block_mulf(__local float* target, __local float* lhs, float rhs, int x, int y) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	target[offset] = lhs[offset] * rhs;\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"void filter_block(__local float* block, int x, int y, float threshold) {\n"
"	int offset = (y << BLOCK_SIZE_LOG2) + x;\n"
"	float s = block[offset];\n"
"	if (fabs(s) < threshold) {\n"
"		block[offset] = 0.0f;\n"
"	}\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"}\n"
"\n"
"__kernel void\n"
"__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))\n"
"filter_kernel(__global float* buffer, __read_only image2d_t source, int x, int y, float threshold) {\n"
"	int2 gid = { get_global_id(0), get_global_id(1) };\n"
"	int2 lid = { get_local_id(0), get_local_id(1) };\n"
"	int2 ss = { get_image_width(source), get_image_height(source) };\n"
"	int2 coords = { gid.x + x, gid.y + y };\n"
"	if (coords.x >= ss.x) {\n"
"		return;\n"
"	}\n"
"	if (coords.y >= ss.y) {\n"
"		return;\n"
"	}\n"
"	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;\n"
"	float s = read_imagef(source, sampler, coords).s0;\n"
"	__local float block[BLOCK_SIZE * BLOCK_SIZE];\n"
"	copy_to_block(block, lid.x, lid.y, s);\n"
"	compute_dct_xy(block, lid.x, lid.y);\n"
"	filter_block(block, lid.x, lid.y, threshold);\n"
"	compute_idct_xy(block, lid.x, lid.y);\n"
"	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];\n"
"	buffer[(coords.y * ss.x) + coords.x] += t;\n"
"}\n"
"\n"
"__kernel void\n"
"__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))\n"
"normalize_kernel(__write_only image2d_t target, __global float* buffer) {\n"
"	int2 gid = { get_global_id(0), get_global_id(1) };\n"
"	int2 lid = { get_local_id(0), get_local_id(1) };\n"
"	int2 ts = { get_image_width(target), get_image_height(target) };\n"
"	int2 coords = { gid.x, gid.y };\n"
"	if (coords.x >= ts.x) {\n"
"		return;\n"
"	}\n"
"	if (coords.y >= ts.y) {\n"
"		return;\n"
"	}\n"
"	float s = buffer[(coords.y * ts.x) + coords.x];\n"
"	int xf = get_scaling_factor(coords.x, ts.x);\n"
"	int yf = get_scaling_factor(coords.y, ts.y);\n"
"	float t = s / (float)(xf * yf);\n"
"	write_imagef(target, coords, (float4)(t));\n"
"}\n"
"\n"
"__kernel void\n"
"__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))\n"
"dct_transform(__write_only image2d_t target, __read_only image2d_t source) {\n"
"	int2 gid = { get_global_id(0), get_global_id(1) };\n"
"	int2 lid = { get_local_id(0), get_local_id(1) };\n"
"	if (gid.x >= get_image_width(source)) {\n"
"		return;\n"
"	}\n"
"	if (gid.y >= get_image_height(source)) {\n"
"		return;\n"
"	}\n"
"	if (gid.x >= get_image_width(target)) {\n"
"		return;\n"
"	}\n"
"	if (gid.y >= get_image_height(target)) {\n"
"		return;\n"
"	}\n"
"	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;\n"
"	float s = read_imagef(source, sampler, gid).s0;\n"
"	__local float block[BLOCK_SIZE * BLOCK_SIZE];\n"
"	copy_to_block(block, lid.x, lid.y, s);\n"
"	convert_to_relative_range(block, lid.x, lid.y);\n"
"	compute_dct_xy(block, lid.x, lid.y);\n"
"	block_mulf(block, block, 1.0f / (float)(BLOCK_SIZE * BLOCK_SIZE), lid.x, lid.y);\n"
"	convert_to_absolute_range(block, lid.x, lid.y);\n"
"	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];\n"
"	write_imagef(target, gid, (float4)(t));\n"
"}\n"
"\n"
"__kernel void\n"
"__attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))\n"
"idct_transform(__write_only image2d_t target, __read_only image2d_t source) {\n"
"	int2 gid = { get_global_id(0), get_global_id(1) };\n"
"	int2 lid = { get_local_id(0), get_local_id(1) };\n"
"	if (gid.x >= get_image_width(source)) {\n"
"		return;\n"
"	}\n"
"	if (gid.y >= get_image_height(source)) {\n"
"		return;\n"
"	}\n"
"	if (gid.x >= get_image_width(target)) {\n"
"		return;\n"
"	}\n"
"	if (gid.y >= get_image_height(target)) {\n"
"		return;\n"
"	}\n"
"	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;\n"
"	float s = read_imagef(source, sampler, gid).s0;\n"
"	__local float block[BLOCK_SIZE * BLOCK_SIZE];\n"
"	copy_to_block(block, lid.x, lid.y, s);\n"
"	convert_to_relative_range(block, lid.x, lid.y);\n"
"	compute_idct_xy(block, lid.x, lid.y);\n"
"	block_mulf(block, block, 1.0f * (float)(BLOCK_SIZE * BLOCK_SIZE), lid.x, lid.y);\n"
"	convert_to_absolute_range(block, lid.x, lid.y);\n"
"	float t = block[(lid.y << BLOCK_SIZE_LOG2) + lid.x];\n"
"	write_imagef(target, gid, (float4)(t));\n"
"}\n"
"\n"
"__kernel void\n"
"lowpass_x_kernel(__write_only image2d_t target, __read_only image2d_t source) {\n"
"	int2 gid = { get_global_id(0), get_global_id(1) };\n"
"	int2 ss = { get_image_width(source), get_image_height(source) };\n"
"	if (gid.x >= ss.x) {\n"
"		return;\n"
"	}\n"
"	if (gid.y >= ss.y) {\n"
"		return;\n"
"	}\n"
"	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;\n"
"	float s0 = read_imagef(source, sampler, (int2){ gid.x - 2, gid.y}).s0;\n"
"	float s1 = read_imagef(source, sampler, (int2){ gid.x - 1, gid.y}).s0;\n"
"	float s2 = read_imagef(source, sampler, (int2){ gid.x + 0, gid.y}).s0;\n"
"	float s3 = read_imagef(source, sampler, (int2){ gid.x + 1, gid.y}).s0;\n"
"	float s4 = read_imagef(source, sampler, (int2){ gid.x + 2, gid.y}).s0;\n"
"	float t = (s0 + s1 + s2 + s3 + s4) / 5.0f;\n"
"	write_imagef(target, gid, (float4)(t));\n"
"}\n"
"\n"
"__kernel void\n"
"lowpass_y_kernel(__write_only image2d_t target, __read_only image2d_t source) {\n"
"	int2 gid = { get_global_id(0), get_global_id(1) };\n"
"	int2 ss = { get_image_width(source), get_image_height(source) };\n"
"	if (gid.x >= ss.x) {\n"
"		return;\n"
"	}\n"
"	if (gid.y >= ss.y) {\n"
"		return;\n"
"	}\n"
"	sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;\n"
"	float s0 = read_imagef(source, sampler, (int2){ gid.x, gid.y - 2}).s0;\n"
"	float s1 = read_imagef(source, sampler, (int2){ gid.x, gid.y - 1}).s0;\n"
"	float s2 = read_imagef(source, sampler, (int2){ gid.x, gid.y + 0}).s0;\n"
"	float s3 = read_imagef(source, sampler, (int2){ gid.x, gid.y + 1}).s0;\n"
"	float s4 = read_imagef(source, sampler, (int2){ gid.x, gid.y + 2}).s0;\n"
"	float t = (s0 + s1 + s2 + s3 + s4) / 5.0f;\n"
"	write_imagef(target, gid, (float4)(t));\n"
"}\n"
"\n"
"";

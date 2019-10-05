function concat(...values: any): string {
	return Array.from(values).map((value: any) => {
		return String(value);
	}).join("");
}

function generate_coefficients(size: number): Array<number> {
	let values = new Array<number>();
	for (let k = 0; k < size; k += 1) {
		let s = (k === 0) ? Math.sqrt(1 / size) : Math.sqrt(2 / size);
		let f = Math.PI * (k / size);
		for (let n = 0; n < size; n += 1) {
			values.push(s * Math.cos(f * (n + 0.5)));
		}
	}
	return values;
};

function pad_left(padding: string, string: string, length: number): string {
	if (padding.length === 1) {
		while (string.length < length) {
			string = concat(padding, string);
		}
	}
	return string;
}

function generate_code(size: number, precision: number): string {
	let coefficients = generate_coefficients(size);
	let string = coefficients.map((coefficient) => {
		let string = pad_left(" ", coefficient.toFixed(precision), precision + 3);
		return concat("\t", string, "f");
	}).join(",\n");
	return concat("__constant float dct_coefficients[", size * size, "] = {\n", string, "\n}\n");
}

console.log(generate_code(8, 20));

import * as libfs from "fs";

libfs.readFile("./dct_denoise.cl", "utf8", (error, data) => {
	if (error === null) {
		let lines = data.split(/(?:\r\n?)|(?:\n\r?)/g)
			.map((line) => {
				return "\"" + line + "\\n\"";
			});
		let string = [ "const char* dct_denoise = \"\"", ...lines, "\"\";" ].join("\n");
		libfs.writeFile("./dct_denoise.cpp", string + "\n", "utf8", (error) => {
			if (error === null) {
				process.exit(0);
			} else {
				process.exit(1);
			}
		});
	} else {
		process.exit(1);
	}
});

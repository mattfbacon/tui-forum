const readline = require('readline');
const { encode, decode } = require("@msgpack/msgpack");
const { strict: assert } = require('assert');
const fetch = require('node-fetch');
const { promisify } = require('util');

const rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout,
});

const rl_question = (question) => {
	return new Promise((resolve, reject) => {
		rl.question(question, resolve);
	});
};

const repl = () => {
	return {
		[Symbol.asyncIterator]() {
			return {
				async next() {
					return rl_question('> ').then(value => ({ done: false, value })).catch(() => ({ done: true }));
				}
			};
		}
	}
};

(async () => {
	for await (cmd of repl()) {
		const split = cmd.split(' ');
		const method = split[0].toUpperCase();
		const url = split[1];
		const content = eval(cmd.slice(method.length + 1 + url.length + 1).join(' '));
		const fetch_response = await fetch('http://localhost:9000' + url, { method, ...(method == 'GET' || method == 'HEAD' ? {} : { body: encode(content) }) });
		if (fetch_response.status >= 200 && fetch_response.status < 300) {
			const decoded = decode(await fetch_response.arrayBuffer());
			console.info(decoded);
		} else {
			console.info(await fetch_response.text());
		}
	}
})();

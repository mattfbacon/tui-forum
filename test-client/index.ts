import {decode, encode} from '@msgpack/msgpack';
const fetch = require('node-fetch');
interface FetchResponse {
	arrayBuffer(): Promise<ArrayBuffer>;
	status: number;
	text(): Promise<string>;
}
interface FetchOptions {
	method?: 'POST'|'GET'|'HEAD'|'OPTIONS'|'PATCH'|'DELETE'|'PUT';
	body?: any;
}
import readline from 'readline';
import {promisify} from 'util';

const rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout,
});

const rl_question = (promisify(rl.question).bind(rl) as unknown as (prompt: string) => Promise<string>);

const repl = () => {
	return {
		[Symbol.asyncIterator](): AsyncIterator<string, undefined> {
			return {
				async next() {
					return rl_question('> ').then(value => ({ done: false, value } as { done: false, value: string })).catch(() => ({ done: true, value: undefined }));
				},
			};
		}
	}
};

let bearer_token: string|null = null;

interface TokenResponse {
	token: string;
}

const is_token_response = (x: unknown): x is TokenResponse => typeof x == 'object' && typeof (x as { token: unknown }).token == 'string';
const is_method = (x: unknown): x is FetchOptions['method'] => typeof x == 'string' && ['POST', 'GET', 'HEAD', 'OPTIONS', 'PATCH', 'DELETE', 'PUT'].includes(x);

async function run_command(command: string, rest: string): Promise<boolean> {
	switch (command) {
		case 'quit':
		case 'exit':
		case 'bye': {
			return false;
		}
		case 'login': {
			const data = eval(`(${rest})`);
			const fetch_response: FetchResponse
				= await fetch('http://localhost:9000/users/session', { method: 'POST', body: encode(data), headers: bearer_token ? { 'Bearer': bearer_token } : {} });
			if (fetch_response.status >= 200 && fetch_response.status < 300 && fetch_response.status !== 204) {
				const decoded = decode(await fetch_response.arrayBuffer());
				bearer_token = is_token_response(decoded) ? decoded.token : null;
			} else {
				console.info(await fetch_response.text());
			}
			return true;
		}
		default: {
			console.warn('Unknown command ' + command);
			return true;
		}
	}
}

(async () => {
	for await (const cmd of repl()) {
		const split = cmd.split(' ');
		const method = split[0].toUpperCase();
		if (method.startsWith('.')) {
			const command = method.slice(1).toLowerCase();
			if (await run_command(command, cmd.slice(method.length + 1))) {
				continue;
			} else {
				break;
			}
		} else {
			const url = decodeURIComponent(split[1]);
			const raw_content = cmd.slice(method.length + 1 + split[1].length + 1);
			const content = raw_content.length > 0 ? eval(`(${raw_content})`) : undefined;
			let fetch_response: FetchResponse;
			if (!is_method(method)) {
				console.warn("Invalid method " + method);
				continue;
			}
			try {
				fetch_response = await fetch(
					'http://localhost:9000' + url,
					{ method, ...(method == 'GET' || method == 'HEAD' ? {} : { body: encode(content) }), headers: bearer_token ? { 'Bearer': bearer_token } : {} });
			} catch (e) {
				console.error((e as { toString: () => string }).toString());
				continue;
			}
			if (fetch_response.status >= 200 && fetch_response.status < 300 && fetch_response.status !== 204) {
				const decoded = decode(await fetch_response.arrayBuffer());
				console.info(decoded);
			} else {
				console.info(await fetch_response.text());
			}
		}
	}
})();

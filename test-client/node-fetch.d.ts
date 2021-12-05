interface FetchResponse {
	arrayBuffer(): Promise<ArrayBuffer>;
	status: number;
	text(): Promise<string>;
}
interface FetchOptions {
	method?: 'POST'|'GET'|'HEAD'|'OPTIONS'|'PATCH'|'DELETE'|'PUT';
	body?: ReadableStream;
	headers?: Record<string, string>;
}

declare module 'node-fetch' {
	function fetch(url: string, options: FetchOptions): Promise<FetchResponse>;
	export = fetch;
}

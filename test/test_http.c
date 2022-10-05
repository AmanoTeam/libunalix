#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "http.h"
#include "errors.h"

int main() {
	
	int code = 0;
	struct HTTPRequest request = {};
	
	char* buffer = NULL;
	size_t buffer_size = 0;
	
	request.version = HTTP10;
	request.method = GET;
	
	http_request_set_url(&request, "http://example.com/");
	
	code = http_request_stringify(&request, &buffer, &buffer_size);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strncmp("GET / HTTP/1.0\r\nHost: example.com\r\n\r\n", buffer, 37) == 0);
	
	free(buffer);
	buffer = NULL;
	
	http_request_free(&request);
	
	request.version = HTTP10;
	request.method = GET;
	
	http_request_set_url(&request, "http://example.com:8080/path?key=value#fragment");
	
	code = http_request_stringify(&request, &buffer, &buffer_size);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strncmp("GET /path?key=value HTTP/1.0\r\nHost: example.com:8080\r\n\r\n", buffer, 56) == 0);
	
	free(buffer);
	buffer = NULL;
	
	http_request_free(&request);
	
	request.version = HTTP10;
	request.method = GET;
	
	http_headers_add(&request.headers, "Accept", "*/*");
	http_headers_add(&request.headers, "User-Agent", "Unalix/0.1");
	
	http_request_set_url(&request, "http://example.com/");
	
	http_body_set(&request.body, "Hello World!", 12);
	
	code = http_request_stringify(&request, &buffer, &buffer_size);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strncmp("GET / HTTP/1.0\r\nAccept: */*\r\nUser-Agent: Unalix/0.1\r\nHost: example.com\r\nContent-Length: 12\r\n\r\nHello World!", buffer, 106) == 0);
	
	free(buffer);
	buffer = NULL;
	
	http_request_free(&request);
	
	request.version = HTTP10;
	request.method = GET;
	
	http_request_set_url(&request, "http://example.com/");
	
	http_body_set(&request.body, "\0\0\0", 3);
	
	code = http_request_stringify(&request, &buffer, &buffer_size);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strncmp("GET / HTTP/1.0\r\nHost: example.com\r\nContent-Length: 3\r\n\r\n\0\0\0", buffer, 59) == 0);
	
	free(buffer);
	buffer = NULL;
	
	http_request_free(&request);
	
	return 0;
	
}
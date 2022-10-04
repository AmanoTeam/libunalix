#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	
	#define _WIN32_WINNT 0x0600
	
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <netdb.h>
#endif

#include <bearssl.h>

#include "address.h"
#include "http.h"
#include "callbacks.h"
#include "utils.h"
#include "errors.h"
#include "uri.h"
#include "ssl.h"

static const char PROTOCOL_NAME[] = "HTTP";
static const char CRLF[] = "\r\n";
static const char CRLFCRLF[] = "\r\n\r\n";

static const char HEADER_SEPARATOR[] = ": ";

static const char HEADER_NAME_SAFE_SYMBOLS[] = "-_";
static const char HEADER_VALUE_SAFE_SYMBOLS[] = "_ :;.,\\/\"'?!(){}[]@<>=-+*#$&`|~^%";

static const size_t MAX_CHUNK_SIZE = 1024;
static const size_t MAX_HEADERS_SIZE = MAX_CHUNK_SIZE * 10;

static int header_name_safe(const char* const s) {
	
	for (size_t index = 0; index < strlen(s); index++) {
		const char ch = s[index];
		
		const int is_safe = (isalnum(ch) || strchr(HEADER_NAME_SAFE_SYMBOLS, ch) != NULL);
		
		if (!is_safe) {
			return is_safe;
		}
	}
	
	return 1;
	
}

static int header_value_safe(const char* const s) {
	
	for (size_t index = 0; index < strlen(s); index++) {
		const char ch = s[index];
		
		const int is_safe = (isalnum(ch) || strchr(HEADER_VALUE_SAFE_SYMBOLS, ch) != NULL);
		
		if (!is_safe) {
			return is_safe;
		}
	}
	
	return 1;
	
}
	
static const char* http_method_stringify(const enum HTTPMethod method) {
	
	switch (method) {
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		case DELETE:
			return "DELETE";
		case CONNECT:
			return "CONNECT";
		case OPTIONS:
			return "OPTIONS";
		case TRACE:
			return "TRACE";
	}
	
	return NULL;
	
}

static const char* http_version_stringify(const enum HTTPVersion version) {
	
	switch (version) {
		case HTTP10:
			return "1.0";
		case HTTP11:
			return "1.1";
	}
	
	return NULL;
	
}

static const char* http_status_stringify(const enum HTTPStatusCode status_code) {
	
	switch (status_code) {
		case CONTINUE:
			return "Continue";
		case SWITCHING_PROTOCOLS:
			return "Switching Protocols";
		case PROCESSING:
			return "Processing";
		case EARLY_HINTS:
			return "Early Hints";
		case OK:
			return "OK";
		case CREATED:
			return "Created";
		case ACCEPTED:
			return "Accepted";
		case NON_AUTHORITATIVE_INFORMATION:
			return "Non-Authoritative Information";
		case NO_CONTENT:
			return "No Content";
		case RESET_CONTENT:
			return "Reset Content";
		case PARTIAL_CONTENT:
			return "Partial Content";
		case MULTI_STATUS:
			return "Multi-Status";
		case ALREADY_REPORTED:
			return "Already Reported";
		case IM_USED:
			return "IM Used";
		case MULTIPLE_CHOICES:
			return "Multiple Choices";
		case MOVED_PERMANENTLY:
			return "Moved Permanently";
		case FOUND:
			return "Found";
		case SEE_OTHER:
			return "See Other";
		case NOT_MODIFIED:
			return "Not Modified";
		case USE_PROXY:
			return "Use Proxy";
		case TEMPORARY_REDIRECT:
			return "Temporary Redirect";
		case PERMANENT_REDIRECT:
			return "Permanent Redirect";
		case BAD_REQUEST:
			return "Bad Request";
		case UNAUTHORIZED:
			return "Unauthorized";
		case PAYMENT_REQUIRED:
			return "Payment Required";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
		case METHOD_NOT_ALLOWED:
			return "Method Not Allowed";
		case NOT_ACCEPTABLE:
			return "Not Acceptable";
		case PROXY_AUTHENTICATION_REQUIRED:
			return "Proxy Authentication Required";
		case REQUEST_TIMEOUT:
			return "Request Timeout";
		case CONFLICT:
			return "Conflict";
		case GONE:
			return "Gone";
		case LENGTH_REQUIRED:
			return "Length Required";
		case PRECONDITION_FAILED:
			return "Precondition Failed";
		case REQUEST_ENTITY_TOO_LARGE:
			return "Request Entity Too Large";
		case REQUEST_URI_TOO_LONG:
			return "Request-URI Too Long";
		case UNSUPPORTED_MEDIA_TYPE:
			return "Unsupported Media Type";
		case REQUESTED_RANGE_NOT_SATISFIABLE:
			return "Requested Range Not Satisfiable";
		case EXPECTATION_FAILED:
			return "Expectation Failed";
		case IM_A_TEAPOT:
			return "I'm a Teapot";
		case MISDIRECTED_REQUEST:
			return "Misdirected Request";
		case UNPROCESSABLE_ENTITY:
			return "Unprocessable Entity";
		case LOCKED:
			return "Locked";
		case FAILED_DEPENDENCY:
			return "Failed Dependency";
		case TOO_EARLY:
			return "Too Early";
		case UPGRADE_REQUIRED:
			return "Upgrade Required";
		case PRECONDITION_REQUIRED:
			return "Precondition Required";
		case TOO_MANY_REQUESTS:
			return "Too Many Requests";
		case REQUEST_HEADER_FIELDS_TOO_LARGE:
			return "Request Header Fields Too Large";
		case UNAVAILABLE_FOR_LEGAL_REASONS:
			return "Unavailable For Legal Reasons";
		case INTERNAL_SERVER_ERROR:
			return "Internal Server Error";
		case NOT_IMPLEMENTED:
			return "Not Implemented";
		case BAD_GATEWAY:
			return "Bad Gateway";
		case SERVICE_UNAVAILABLE:
			return "Service Unavailable";
		case GATEWAY_TIMEOUT:
			return "Gateway Timeout";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";
		case VARIANT_ALSO_NEGOTIATES:
			return "Variant Also Negotiates";
		case INSUFFICIENT_STORAGE:
			return "Insufficient Storage";
		case LOOP_DETECTED:
			return "Loop Detected";
		case NOT_EXTENDED:
			return "Not Extended";
		case NETWORK_AUTHENTICATION_REQUIRED:
			return "Network Authentication Required";
	}
	
	return NULL;
	
}

static void http_headers_stringify(const struct HTTPHeaders headers, char* dst) {
	
	for (size_t index = 0; index < headers.offset; index++) {
		const struct HTTPHeader header = headers.items[index];
		
		if (*dst != '\0') {
			strcat(dst, CRLF);
		}
		
		if (header.key != NULL) {
			strcat(dst, header.key);
		}
		
		strcat(dst, COLON);
		strcat(dst, SPACE);
		
		if (header.value != NULL) {
			strcat(dst, header.value);
		}
	}
	
}

static int http_headers_add(struct HTTPHeaders* obj, const char* key, const char* value) {
	
	struct HTTPHeader header = {
		.key = malloc(strlen(key) + 1),
		.value = malloc(strlen(value) + 1)
	};
	
	if (header.key == NULL || header.value == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	strcpy(header.key, key);
	strcpy(header.value, value);
	
	const size_t size = obj->size + sizeof(struct HTTPHeader) * 1;
	struct HTTPHeader* items = realloc(obj->items, size);
	
	if (items == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	obj->size = size;
	obj->items = items;
	obj->items[obj->offset++] = header;
	
	if (obj->slength > 0) {
		obj->slength += strlen(CRLF);
	}
	
	if (key != NULL) {
		obj->slength += strlen(key);
	}
	
	obj->slength += strlen(COLON) + strlen(SPACE);
	
	if (value != NULL) {
		obj->slength += strlen(value);
	}
	
	return UNALIXERR_SUCCESS;
	
}

static const struct HTTPHeader* http_headers_get(const struct HTTPHeaders* obj, const char* key) {
	
	for (size_t index = 0; index < obj->offset; index++) {
		const struct HTTPHeader* header = &obj->items[index];
		
		if (strcmp(header->key, key) == 0) {
			return header;
		}
	}
	
	return NULL;
	
}

int http_request_add_header(struct HTTPContext* context, const char* key, const char* value) {
	return http_headers_add(&context->request.headers, key, value);
}

const struct HTTPHeader* http_response_get_header(const struct HTTPContext* context, const char* key) {
	return http_headers_get(&context->response.headers, key);
}

int http_body_set(struct HTTPBody* obj, const char* buffer, const size_t buffer_size) {
	
	obj->content = malloc(buffer_size);
	
	if (obj->content == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	memcpy(obj->content, buffer, buffer_size);
	obj->size = buffer_size;
	
	return UNALIXERR_SUCCESS;
	
}

const struct HTTPBody* http_response_get_body(struct HTTPContext* context) {
	return &context->response.body;
}

const struct HTTPStatus* http_response_get_status(struct HTTPContext* context) {
	return &context->response.status;
}

int http_request_stringify(struct HTTPRequest* obj, char** dst, size_t* dst_size) {
	
	if (obj->body.size > 0) {
		char value[intlen(obj->body.size) + 1];
		snprintf(value, sizeof(value), "%i", obj->body.size);
		
		const int code = http_headers_add(&obj->headers, "Content-Length", value);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
	}
	
	char resource_location[strlen(SLASH) + (obj->uri.path != NULL ? strlen(obj->uri.path) : 0) + (obj->uri.query != NULL ? strlen(QUESTION_MARK) + strlen(obj->uri.query) : 0) + 1];
	strcpy(resource_location, SLASH);
	
	if (obj->uri.path != NULL) {
		strcat(resource_location, obj->uri.path);
	}
	
	if (obj->uri.query != NULL) {
		strcat(resource_location, QUESTION_MARK);
		strcat(resource_location, obj->uri.query);
	}
	
	const char* const http_method = http_method_stringify(obj->method);
	const char* const http_version = http_version_stringify(obj->version);
	
	char headers[obj->headers.slength + 1];
	http_headers_stringify(obj->headers, memset(headers, '\0', 1));
	
	const size_t buffer_size = (
		strlen(http_method) + strlen(SPACE) + strlen(resource_location) + strlen(SPACE) + strlen(PROTOCOL_NAME) + strlen(SLASH) + strlen(http_version) + strlen(CRLF) +
		strlen(headers) + strlen(CRLFCRLF) + obj->body.size
	);
	char* buffer = malloc(buffer_size);
	
	if (buffer == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	// HTTP method
	strcpy(buffer, http_method);
	strcat(buffer, SPACE);
	
	// Resource location
	strcat(buffer, resource_location);
	strcat(buffer, SPACE);
	
	// HTTP version
	strcat(buffer, PROTOCOL_NAME);
	strcat(buffer, SLASH);
	strcat(buffer, http_version);
	strcat(buffer, CRLF);
	
	// Headers
	strcat(buffer, headers);
	strcat(buffer, CRLFCRLF);
	
	// Body
	if (obj->body.size > 0) {
		memcpy(strrchr(buffer, '\0'), obj->body.content, obj->body.size);
	}
	
	*dst = buffer;
	*dst_size = buffer_size;
	
	return UNALIXERR_SUCCESS;
	
}

static int set_host_header(struct HTTPHeaders* headers, const struct URI uri) {
	
	const size_t port_size = intlen(uri.port);
	
	char host[strlen(uri.hostname) + strlen(COLON) + (uri.port > 0 ? port_size : 0)];
	strcpy(host, uri.hostname);
	
	if (uri.port > 0 && ((strcmp(uri.scheme, HTTP_SCHEME) == 0 && uri.port != HTTP_PORT) || (strcmp(uri.scheme, HTTPS_SCHEME) == 0 && uri.port != HTTPS_PORT))) {
		strcat(host, COLON);
		
		char port[port_size + 1];
		snprintf(port, sizeof(port), "%i", uri.port);
		strcat(host, port);
	}
	
	const int code = http_headers_add(headers, "Host", host);
	
	return code;
	
}

int http_request_set_url(struct HTTPContext* context, const char* url) {
	
	int code = uri_parse(&context->request.uri, url);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = set_host_header(&context->request.headers, context->request.uri);
	
	return code;
	
}

int http_request_set_uri(struct HTTPContext* context, const struct URI uri) {
	
	context->request.uri.is_ipv6 = uri.is_ipv6;
	context->request.uri.has_authentication = uri.has_authentication;
	
	if (uri.username != NULL) {
		context->request.uri.username = malloc(strlen(uri.username) + 1);
		
		if (context->request.uri.username == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.username, uri.username);
	}
	
	if (uri.password != NULL) {
		context->request.uri.password = malloc(strlen(uri.password) + 1);
		
		if (context->request.uri.password == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.password, uri.password);
	}
	
	if (uri.scheme != NULL) {
		context->request.uri.scheme = malloc(strlen(uri.scheme) + 1);
		
		if (context->request.uri.scheme == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.scheme, uri.scheme);
	}
	
	if (uri.hostname != NULL) {
		context->request.uri.hostname = malloc(strlen(uri.hostname) + 1);
		
		if (context->request.uri.hostname == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.hostname, uri.hostname);
	}
	
	context->request.uri.port = uri.port;
	
	if (uri.query != NULL) {
		context->request.uri.query = malloc(strlen(uri.query) + 1);
		
		if (context->request.uri.query == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.query, uri.query);
	}
	
	if (uri.path != NULL) {
		context->request.uri.path = malloc(strlen(uri.path) + 1);
		
		if (context->request.uri.path == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.path, uri.path);
	}
	
	if (uri.fragment != NULL) {
		context->request.uri.fragment = malloc(strlen(uri.fragment) + 1);
		
		if (context->request.uri.fragment == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(context->request.uri.fragment, uri.fragment);
	}
	
	return set_host_header(&context->request.headers, context->request.uri);
	
}

static void http_headers_free(struct HTTPHeaders* obj) {
	
	if (obj->size < 1) {
		return;
	}
	
	for (size_t index = 0; index < obj->offset; index++) {
		struct HTTPHeader* header = &obj->items[index];
		
		if (header->key != NULL) {
			free(header->key);
			header->key = NULL;
		}
		
		if (header->value != NULL) {
			free(header->value);
			header->value = NULL;
		}
	}
	
	free(obj->items);
	obj->items = NULL;
	
	obj->size = 0;
	obj->offset = 0;
	
}

static void http_body_free(struct HTTPBody* obj) {
	
	if (obj->size < 1) {
		return;
	}
	
	free(obj->content);
	
	obj->content = NULL;
	obj->size = 0;
	
}

void http_request_free(struct HTTPRequest* obj) {
	
	obj->version = 0;
	obj->method = 0;
	
	http_headers_free(&obj->headers);
	http_body_free(&obj->body);
	
	uri_free(&obj->uri);
	
}

void http_response_free(struct HTTPResponse* obj) {
	
	obj->status.code = (enum HTTPStatusCode) 0;
	obj->status.message = NULL;
	
	obj->version = 0;
	
	http_headers_free(&obj->headers);
	http_body_free(&obj->body);
	
}

void http_context_free(struct HTTPContext* context) {
	
	connection_free(&context->connection);
	http_request_free(&context->request);
	http_response_free(&context->response);
	
}


static int parse_http_response(struct HTTPResponse* obj, const char* buffer, const size_t buffer_size) {
	
	char data[buffer_size + 1];
	memcpy(data, buffer, buffer_size);
	data[buffer_size] = '\0';
	
	const char* data_end = &data[buffer_size];
	
	const char* separator = strstr(data, CRLFCRLF);
	
	// Headers
	const size_t headers_size = (size_t) (separator - data);
	
	char headers[headers_size + 1];
	memcpy(headers, data, headers_size);
	headers[headers_size] = '\0';
	
	// Body
	const char* body_start = separator;
	body_start += strlen(CRLFCRLF);
	
	const size_t body_size = (size_t) (data_end - body_start);
	
	char body[body_size + 1];
	
	if (body_size > 0) {
		memcpy(body, body_start, body_size);
	}
	
	body[body_size] = '\0';
	
	// HTTP version
	const char* http_version_start = strstr(headers, SLASH);
	http_version_start += strlen(SLASH);
	const char* http_version_end = strstr(http_version_start, SPACE);
	
	const size_t http_version_size = (size_t) (http_version_end - http_version_start);
	
	char http_version[http_version_size + 1];
	memcpy(http_version, http_version_start, http_version_size);
	http_version[http_version_size] = '\0';
	
	if (strcmp(http_version, http_version_stringify(HTTP10)) == 0) {
		obj->version = HTTP10;
	} else if (strcmp(http_version, http_version_stringify(HTTP11)) == 0) {
		obj->version = HTTP11;
	} else {
		return UNALIXERR_HTTP_UNSUPPORTED_VERSION;
	}
	
	// Status code
	const char* status_code_start = strstr(http_version_end, SPACE);
	status_code_start += strlen(SPACE);
	const char* status_code_end = strstr(status_code_start, SPACE);
	
	const size_t status_code_size = (size_t) (status_code_end - status_code_start);
	
	if (status_code_size != intlen(OK)) {
		return UNALIXERR_HTTP_MALFORMED_STATUS_CODE;
	}
	
	char status_code[status_code_size + 1];
	memcpy(status_code, status_code_start, status_code_size);
	status_code[status_code_size] = '\0';
	
	if (!isnumeric(status_code)) {
		return UNALIXERR_HTTP_MALFORMED_STATUS_CODE;
	}
	
	const int value = atoi(status_code);
	
	const enum HTTPStatusCode http_status = (enum HTTPStatusCode) value;
	const char* http_status_message = http_status_stringify(http_status);
	
	obj->status.code = http_status;
	obj->status.message = http_status_message;
	
	if (http_status_message == NULL) {
		return UNALIXERR_HTTP_UNKNOWN_STATUS_CODE;
	}
	
	const char* header_start = strstr(headers, CRLF);
	header_start += strlen(CRLF);
	const char* header_end = strstr(header_start, CRLF);
	
	const char* headers_end = strchr(headers, '\0');
	
	while (1) {
		if (!header_end) {
			header_end = headers_end;
		}
		
		const char* separator = strstr(header_start, HEADER_SEPARATOR);
		
		if (separator == NULL || separator > header_end) {
			return UNALIXERR_HTTP_MALFORMED_HEADER;
		}
		
		const size_t key_size = (size_t) (separator - header_start);
		
		if (key_size < 1) {
			return UNALIXERR_HTTP_MISSING_HEADER_NAME;
		}
		
		char key[key_size + 1];
		memcpy(key, header_start, key_size);
		key[key_size] = '\0';
		
		if (!header_name_safe(key)) {
			return UNALIXERR_HTTP_HEADER_CONTAINS_INVALID_CHARACTER;
		}
		
		separator += strlen(HEADER_SEPARATOR);
		
		const size_t value_size = (size_t) (header_end - separator);
		
		if (value_size < 1) {
			return UNALIXERR_HTTP_MISSING_HEADER_VALUE;
		}
		
		char value[value_size + 1];
		memcpy(value, separator, value_size);
		value[value_size] = '\0';
		
		if (!header_value_safe(value)) {
			return UNALIXERR_HTTP_HEADER_CONTAINS_INVALID_CHARACTER;
		}
		
		const int code = http_headers_add(&obj->headers, key, value);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		if (header_end == headers_end) {
			break;
		}
		
		header_start = header_end;
		header_start += strlen(CRLF);
		
		header_end = strstr(header_start, CRLF);
	}
	
	if (body_size > 0) {
		obj->body.content = malloc(body_size);
		
		if (obj->body.content == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
	 	
		memcpy(obj->body.content, body, body_size);
		obj->body.size = body_size;
	}
	
	return UNALIXERR_SUCCESS;
	
}

static int find_crlfcrlf(const char* buffer, const size_t buffer_size) {
	
	for (size_t index = 0; index < buffer_size; index++) {
		if (memcmp(buffer + index, CRLFCRLF, strlen(CRLFCRLF)) == 0) {
			return 1;
		}
	}
	
	return 0;
	
}

int http_request_send(struct HTTPContext* context) {
	
	struct Address address = {0};
	
	const char* sa = context->request.uri.hostname;
	int sa_port = context->request.uri.port;
	
	if (sa_port == 0) {
		if (strcmp(context->request.uri.scheme, HTTP_SCHEME) == 0) {
			sa_port = 80;
		} else if (strcmp(context->request.uri.scheme, HTTPS_SCHEME) == 0) {
			sa_port = 443;
		} else {
			return UNALIXERR_URI_SCHEME_INVALID;
		}
	}
	
	if (address_parse(&address, sa, sa_port) != UNALIXERR_SUCCESS) {
		struct addrinfo hints = {.ai_family = AF_UNSPEC};
		struct addrinfo* res = NULL;
		
		char value[intlen(sa_port) + 1];
		snprintf(value, sizeof(value), "%i", sa_port);
		
		if (getaddrinfo(sa, value, &hints, &res) != 0) {
			return UNALIXERR_DNS_GAI_FAILURE;
		}
		
		memcpy(&address.addr_storage, res->ai_addr, (size_t) res->ai_addrlen);
		
		address.af = res->ai_family;
		address.addr_storage_size = res->ai_addrlen;
		
		freeaddrinfo(res);
	}
	
	context->connection.fd = socket(address.af, SOCK_STREAM, IPPROTO_TCP);
	
	if (context->connection.fd == -1) {
		return UNALIXERR_SOCKET_FAILURE;
	}
	
	if (context->connection.timeout > 0) {
		const int code = connection_set_timeout(&context->connection, context->connection.timeout);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
	}
	
	if (connect(context->connection.fd, (struct sockaddr*) &address.addr_storage, address.addr_storage_size) != 0) {
		return UNALIXERR_SOCKET_CONNECT_FAILURE;
	}
	
	char* buffer = NULL;
	size_t buffer_size = 0;
	
	const int code = http_request_stringify(&context->request, &buffer, &buffer_size);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	const int is_https = strcmp(context->request.uri.scheme, HTTPS_SCHEME) == 0;
	
	if (is_https) {
		if (!context->connection.ssl_context.is_initialized) {
			br_ssl_client_init_full(&context->connection.ssl_context.sc, &context->connection.ssl_context.xc, TAs, TAs_NUM);
			br_ssl_engine_set_buffer(&context->connection.ssl_context.sc.eng, context->connection.ssl_context.iobuf, sizeof(context->connection.ssl_context.iobuf), 1);
			br_sslio_init(&context->connection.ssl_context.ioc, &context->connection.ssl_context.sc.eng, sock_read, &context->connection.fd, sock_write, &context->connection.fd);
			
			context->connection.ssl_context.is_initialized = 1;
		}
		
		br_ssl_client_reset(&context->connection.ssl_context.sc, context->request.uri.hostname, 0);
		
		const ssize_t size = br_sslio_write_all(&context->connection.ssl_context.ioc, buffer, buffer_size);
		
		free(buffer);
		
		if (size != 0) {
			return UNALIXERR_SSL_FAILURE;
		}
		
		br_sslio_flush(&context->connection.ssl_context.ioc);
	} else {
		const ssize_t size = send(context->connection.fd, buffer, buffer_size, 0);
		
		free(buffer);
		
		if ((size_t) size != buffer_size) {
			return UNALIXERR_SOCKET_SEND_FAILURE;
		}
	}
	
	char headers[MAX_HEADERS_SIZE];
	size_t offset = 0;
	
	while (1) {
		char chunk[MAX_CHUNK_SIZE];
		ssize_t chunk_size = 0;
		
		if (is_https) {
			chunk_size = br_sslio_read(&context->connection.ssl_context.ioc, chunk, sizeof(chunk));
			
			if (chunk_size == -1) {
				if (br_ssl_engine_last_error(&context->connection.ssl_context.sc.eng) != BR_ERR_IO) {
					return UNALIXERR_SSL_FAILURE;
				}
				
				break;
			}
			
			if (chunk_size == 0) {
				if (offset == 0) {
					return UNALIXERR_SSL_FAILURE;
				}
				
				break;
			}
		} else {
			chunk_size = recv(context->connection.fd, chunk, sizeof(chunk), 0);
			
			if (chunk_size == -1) {
				return UNALIXERR_SOCKET_RECV_FAILURE;
			}
			
			if (chunk_size == 0) {
				if (offset == 0) {
					return UNALIXERR_SOCKET_RECV_FAILURE;
				}
				
				break;
			}
		}
		
		if ((offset + (size_t) chunk_size) > sizeof(headers)) {
			return UNALIXERR_HTTP_HEADERS_TOO_BIG;
		}
		
		memcpy(headers + offset, chunk, (size_t) chunk_size);
		offset += (size_t) chunk_size;
		
		if (find_crlfcrlf(headers, offset)) {
			break;
		}
	}
	
	return parse_http_response(&context->response, headers, offset);
	
}

int http_response_read(struct HTTPContext* context, FILE* file) {
	
	if (file != NULL && context->response.body.size > 0) {
		if (fwrite(context->response.body.content, sizeof(*context->response.body.content), context->response.body.size, file) != context->response.body.size) {
			return UNALIXERR_FILE_CANNOT_WRITE;
		}
	}
	
	while (1) {
		char chunk[MAX_CHUNK_SIZE];
		ssize_t chunk_size = 0;
		
		if (strcmp(context->request.uri.scheme, HTTPS_SCHEME) == 0) {
			chunk_size = br_sslio_read(&context->connection.ssl_context.ioc, chunk, sizeof(chunk));
			
			if (chunk_size == -1) {
				const int code = br_ssl_engine_last_error(&context->connection.ssl_context.sc.eng);
				
				if (!(code == BR_ERR_IO || code == BR_ERR_OK)) {
					return UNALIXERR_SSL_FAILURE;
				}
				
				break;
			}
			
			if (chunk_size == 0) {
				break;
			}
		} else {
			chunk_size = recv(context->connection.fd, chunk, sizeof(chunk), 0);
			
			if (chunk_size == -1) {
				return UNALIXERR_SOCKET_RECV_FAILURE;
			}
			
			if (chunk_size == 0) {
				break;
			}
		}
		
		if (file == NULL) {
			const size_t size = context->response.body.size + (size_t) chunk_size;
			char* content = realloc(context->response.body.content, size);
			
			if (content == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(content + context->response.body.size, chunk, (size_t) chunk_size);
			
			context->response.body.size = size;
			context->response.body.content = content;
		} else {
			if (fwrite(chunk, sizeof(*chunk), (size_t) chunk_size, file) != (size_t) chunk_size) {
				return UNALIXERR_FILE_CANNOT_WRITE;
			}
		}
	}
	
	return UNALIXERR_SUCCESS;
	
}

int http_get_redirect(const struct HTTPContext* context, char** dst) {
	
	const struct HTTPHeader* const location_header = http_headers_get(&context->response.headers, "Location");
	
	if ((context->response.status.code == MOVED_PERMANENTLY || context->response.status.code == FOUND || context->response.status.code == SEE_OTHER || context->response.status.code == TEMPORARY_REDIRECT || context->response.status.code == PERMANENT_REDIRECT) && location_header != NULL) {
		const char* const location = location_header->value;
		
		if (strncmp(location, HTTP_SCHEME, strlen(HTTP_SCHEME)) == 0 || strncmp(location, HTTPS_SCHEME, strlen(HTTPS_SCHEME)) == 0) {
			*dst = malloc(strlen(location) + 1);
			
			if (*dst == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			strcpy(*dst, location);
		} else {
			const struct HTTPHeader* const header = http_headers_get(&context->request.headers, "Host");
			
			// Absolute path
			if (*location == *SLASH && strlen(location) > 1 && location[2] != *SLASH) {
				char normalized_path[strlen(location) + strlen(SLASH) + 1];
				httpnormpath(location, normalized_path);
				
				*dst = malloc(strlen(context->request.uri.scheme) + strlen(SCHEME_SEPARATOR) + strlen(header->value) + strlen(normalized_path) + 1);
				
				if (*dst == NULL) {
					return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
				}
				
				strcpy(*dst, context->request.uri.scheme);
				strcat(*dst, SCHEME_SEPARATOR);
				strcat(*dst, header->value);
				strcat(*dst, normalized_path);
			} else if (*location != *SLASH) { // Relative path
				char path[strlen(SLASH) * 2 + (context->request.uri.path == NULL ? 0 : strlen(context->request.uri.path)) + strlen(header->value) + strlen(location) + 1];
				strcpy(path, SLASH);
				
				if (context->request.uri.path != NULL) {
					strcat(path, context->request.uri.path);
					strcat(path, SLASH);
				}
				
				strcat(path, location);
				
				char normalized_path[strlen(path) + strlen(SLASH) + 1];
				httpnormpath(path, normalized_path);
				
				*dst = malloc(strlen(context->request.uri.scheme) + strlen(SCHEME_SEPARATOR) + strlen(header->value) + strlen(normalized_path) + 1);
				
				if (*dst == NULL) {
					return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
				}
				
				strcpy(*dst, context->request.uri.scheme);
				strcat(*dst, SCHEME_SEPARATOR);
				strcat(*dst, header->value);
				strcat(*dst, normalized_path);
			} else if (strncmp(location, "///", 3) == 0 || strncmp(location, "//", 2) == 0) {
				const char* const start = location + countp(location, strlen(location), '/');
				
				*dst = malloc(strlen(context->request.uri.scheme) + strlen(SCHEME_SEPARATOR) + strlen(start) + 1);
				
				strcpy(*dst, context->request.uri.scheme);
				strcat(*dst, SCHEME_SEPARATOR);
				strcat(*dst, start);
			} else {
				return UNALIXERR_HTTP_HEADERS_INVALID_LOCATION;
			}
		}
	}
	
	return UNALIXERR_SUCCESS;
	
}

int http_request_send(struct HTTPContext* context);

	
/*
int main() {
	
	struct HTTPRequest request = {
		.version = HTTP10,
		.method = GET
	};
	
	http_headers_add(&request.headers, "Accept-Encoding", "identity");
	http_headers_add(&request.headers, "Connection", "close");
	http_request_set_url(&request, "https://g.co/10GB.bin");
	
	struct Connection connection = {0};
	struct HTTPResponse response = {0};
	
	http_request_send(&connection, &request, &response);
	http_response_read(&connection, &request, &response, NULL);
	
	printf("%i\n", response.body.size);
	puts(response.body.content);
	
}
*/
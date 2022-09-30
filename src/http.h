#include <stdlib.h>

#include "uri.h"
#include "connection.h"

enum HTTPMethod {
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
};

enum HTTPVersion {
	HTTP10,
	HTTP11
};

enum HTTPStatusCode {
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	PROCESSING = 102,
	EARLY_HINTS = 103,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTI_STATUS = 207,
	ALREADY_REPORTED = 208,
	IM_USED = 226,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTICATION_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	REQUEST_ENTITY_TOO_LARGE = 413,
	REQUEST_URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	REQUESTED_RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	IM_A_TEAPOT = 418,
	MISDIRECTED_REQUEST = 421,
	UNPROCESSABLE_ENTITY = 422,
	LOCKED = 423,
	FAILED_DEPENDENCY = 424,
	TOO_EARLY = 425,
	UPGRADE_REQUIRED = 426,
	PRECONDITION_REQUIRED = 428,
	TOO_MANY_REQUESTS = 429,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	UNAVAILABLE_FOR_LEGAL_REASONS = 451,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	VARIANT_ALSO_NEGOTIATES = 506,
	INSUFFICIENT_STORAGE = 507,
	LOOP_DETECTED = 508,
	NOT_EXTENDED = 510,
	NETWORK_AUTHENTICATION_REQUIRED = 511
};

struct HTTPHeader {
	char* key;
	char* value;
};

struct HTTPHeaders {
	size_t offset;
	size_t size;
	struct HTTPHeader* items;
	size_t slength;
};

struct HTTPBody {
	size_t size;
	char* content;
};

struct HTTPStatus {
	enum HTTPStatusCode code;
	const char* message;
};

struct HTTPRequest {
	enum HTTPVersion version;
	enum HTTPMethod method;
	struct HTTPHeaders headers;
	struct HTTPBody body;
	struct URI uri;
};

struct HTTPResponse {
	struct HTTPStatus status;
	enum HTTPVersion version;
	struct HTTPHeaders headers;
	struct HTTPBody body;
};

int http_request_stringify(struct HTTPRequest* obj, char** dst, size_t* dst_size);
int http_request_set_url(struct HTTPRequest* obj, const char* url);
int http_request_set_uri(struct HTTPRequest* obj, const struct URI uri);
void http_request_free(struct HTTPRequest* obj);

int http_request_send(struct Connection* connection, struct HTTPRequest* request, struct HTTPResponse* response);
int http_response_read(struct Connection* connection, struct HTTPRequest* request, struct HTTPResponse* response, FILE* file);

int http_headers_add(struct HTTPHeaders* obj, const char* key, const char* value);
const struct HTTPHeader* http_headers_get(const struct HTTPHeaders obj, const char* key);
int http_body_set(struct HTTPBody* obj, const char* buffer, const size_t buffer_size);

int http_response_parse(struct HTTPResponse* obj, const char* buffer, const size_t buffer_size);
void http_response_free(struct HTTPResponse* obj);

int http_get_redirect(const struct HTTPRequest request, const struct HTTPResponse response, char** dst);

static const int HTTP_MAX_REDIRECTS = 20;
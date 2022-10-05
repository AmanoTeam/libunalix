#include <stdlib.h>
#include <stdio.h>

#include "uri.h"
#include "connection.h"
#include "version.h"

#ifdef _WIN32
	#undef DELETE
#endif

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

struct HTTPContext {
	struct HTTPRequest request;
	struct HTTPResponse response;
	struct Connection connection;
};

static const char HTTP_DATE_FORMAT[] = "%a, %d %b %Y %H:%M:%S GMT";
static const size_t HTTP_DATE_SIZE = 29;

static const char HTTP_USER_AGENT[] =
	"libunalix/"
	UNALIX_VERSION_STRING
	" (+"
	UNALIX_HOMEPAGE_URL
	")";

static const int HTTP_MAX_REDIRECTS = 20;

static const int HTTP_TIMEOUT = 8;

int http_request_set_url(struct HTTPContext* context, const char* url);
int http_request_set_uri(struct HTTPContext* context, const struct URI uri);
int http_request_add_header(struct HTTPContext* context, const char* key, const char* value);
int http_request_send(struct HTTPContext* context);

int http_response_read(struct HTTPContext* context, FILE* file);
const struct HTTPHeader* http_response_get_header(const struct HTTPContext* context, const char* key);
const struct HTTPBody* http_response_get_body(struct HTTPContext* context);
const struct HTTPStatus* http_response_get_status(struct HTTPContext* context);

int http_get_redirect(const struct HTTPContext* context, char** dst);
void http_context_free(struct HTTPContext* context);

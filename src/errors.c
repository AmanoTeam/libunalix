#include "errors.h"

const char* unalix_strerror(const int code) {
	
	switch (code) {
		case UNALIXERR_SUCCESS:
			return "No error";
		case UNALIXERR_MEMORY_ALLOCATE_FAILURE:
			return "Failed to allocate memory";
		case UNALIXERR_FILE_CANNOT_OPEN:
			return "Cannot open file for read/write";
		case UNALIXERR_FILE_CANNOT_READ:
			return "Cannot read contents of file";
		case UNALIXERR_FILE_CANNOT_WRITE:
			return "Cannot write contents to file";
		case UNALIXERR_FILE_CANNOT_MOVE:
			return "Cannot move file";
		case UNALIXERR_JSON_CANNOT_PARSE:
			return "Cannot parse JSON tree";
		case UNALIXERR_JSON_MISSING_REQUIRED_KEY:
			return "Missing required key in JSON tree";
		case UNALIXERR_JSON_NON_MATCHING_TYPE:
			return "JSON object does not match the required type";
		case UNALIXERR_REGEX_COMPILE_PATTERN_FAILURE:
			return "Regex pattern compilation failed";
		case UNALIXERR_URI_SCHEME_INVALID:
			return "URI has an invalid or unrecognized scheme";
		case UNALIXERR_URI_SCHEME_MISSING:
			return "There is no scheme in the URI";
		case UNALIXERR_URI_SCHEME_SHOULD_STARTS_WITH_LETTER:
			return "URI scheme should starts with a letter";
		case UNALIXERR_URI_SCHEME_EMPTY:
			return "URI has an empty scheme";
		case UNALIXERR_URI_SCHEME_CONTAINS_INVALID_CHARACTER:
			return "URI scheme contains invalid character";
		case UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_STARTS_WITH_HYPHEN:
			return "Hostname label cannot starts with a hyphen";
		case UNALIXERR_URI_HOSTNAME_TOO_LONG:
			return "Hostname has more than 253 characters";
		case UNALIXERR_URI_HOSTNAME_TOO_SHORT:
			return "Hostname is lower than 4 characters";
		case UNALIXERR_URI_HOSTNAME_LABEL_TOO_LONG:
			return "Hostname label has more than 63 characters";
		case UNALIXERR_URI_HOSTNAME_LABEL_EMPTY:
			return "Hostname label is empty";
		case UNALIXERR_URI_HOSTNAME_LABEL_CONTAINS_INVALID_CHARACTER:
			return "Hostname label contains invalid character";
		case UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_ENDS_WITH_HYPHEN:
			return "Hostname label cannot ends with hyphen";
		case UNALIXERR_URI_PORT_OUT_OF_RANGE:
			return "Hostname port is out of range, should be between 1 and 65535";
		case UNALIXERR_URI_PORT_CONTAINS_INVALID_CHARACTER:
			return "Hostname port contains non-numerical character";
		case UNALIXERR_URI_HOSTNAME_MISSING_CLOSING_BRACKET:
			return "Missing closing bracket in IPv6 address";
		case UNALIXERR_URI_IPV6_ADDRESS_TOO_SHORT:
			return "The IPv6 address is too short";
		case UNALIXERR_URI_IPV6_ADDRESS_TOO_LONG:
			return "The IPv6 address is too long";
		case UNALIXERR_URI_IPV6_ADDRESS_INVALID:
			return "The IPv6 address is invalid";
		case UNALIXERR_RULESETS_EMPTY:
			return "No rulesets have been loaded";
		case UNALIXERR_RULESETS_NOT_MODIFIED:
			return "The remote file was not modified since the last update";
		case UNALIXERR_RULESETS_UPDATE_AVAILABLE:
			return "The remote file was modified since the last update";
		case UNALIXERR_RULESETS_MISMATCH_HASH:
			return "The SHA256 hash of the remote file did not match";
		case UNALIXERR_DNS_GAI_FAILURE:
			return "Cannot resolve hostname";
		case UNALIXERR_DNS_CANNOT_PARSE_ADDRESS:
			return "Cannot parse IP address";
		case UNALIXERR_SOCKET_FAILURE:
			return "Connot create socket";
		case UNALIXERR_SOCKET_SETOPT_FAILURE:
			return "Cannot set option on socket";
		case UNALIXERR_SOCKET_SEND_FAILURE:
			return "Cannot send data on socket";
		case UNALIXERR_SOCKET_RECV_FAILURE:
			return "Cannot receive data on socket";
		case UNALIXERR_SOCKET_CLOSE_FAILURE:
			return "Cannot close socket";
		case UNALIXERR_SOCKET_CONNECT_FAILURE:
			return "Cannot connect to remote address";
		case UNALIXERR_SSL_FAILURE:
			return "SSL/TLS connection failure";
		case UNALIXERR_HTTP_UNSUPPORTED_VERSION:
			return "The HTTP server replied using a unsupported protocol version";
		case UNALIXERR_HTTP_MALFORMED_STATUS_CODE:
			return "Malformed HTTP status code";
		case UNALIXERR_HTTP_UNKNOWN_STATUS_CODE:
			return "Unknown HTTP status code";
		case UNALIXERR_HTTP_MALFORMED_HEADER:
			return "Malformed HTTP header";
		case UNALIXERR_HTTP_MISSING_HEADER_NAME:
			return "HTTP header has an empty name";
		case UNALIXERR_HTTP_MISSING_HEADER_VALUE:
			return "HTTP header has an empty value";
		case UNALIXERR_HTTP_HEADER_CONTAINS_INVALID_CHARACTER:
			return "HTTP header contains invalid character";
		case UNALIXERR_HTTP_HEADERS_TOO_BIG:
			return "HTTP headers exceeded max allowed size";
		case UNALIXERR_HTTP_HEADERS_INVALID_LOCATION:
			return "This HTTP redirect points to an invalid location";
		case UNALIXERR_HTTP_TOO_MANY_REDIRECTS:
			return "Too many HTTP redirects followed";
		case UNALIXERR_HTTP_BAD_STATUS_CODE:
			return "Got a unexpected HTTP status code";
		case UNALIXERR_OS_STAT_FAILURE:
			return "Cannot get file status";
		case UNALIXERR_OS_GMTIME_FAILURE:
			return "Cannot convert local time to UTC time";
		case UNALIXERR_OS_MKTIME_FAILURE:
			return "Cannot get epoch representation of time object";
		case UNALIXERR_OS_STRFTIME_FAILURE:
			return "Cannot format time object to string";
		case UNALIXERR_OS_STRPTIME_FAILURE:
			return "Cannot parse string into time object";
		default:
			return "Unknown error code";
	}
	
}

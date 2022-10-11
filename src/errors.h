#define UNALIXERR_SUCCESS 0 /* No error */

#define UNALIXERR_MEMORY_ALLOCATE_FAILURE -1 /* Failed to allocate memory */

#define UNALIXERR_FILE_CANNOT_OPEN -2 /* Cannot open file for read/write */
#define UNALIXERR_FILE_CANNOT_READ -3 /* Cannot read contents of file */
#define UNALIXERR_FILE_CANNOT_WRITE -4 /* Cannot write contents to file */
#define UNALIXERR_FILE_CANNOT_MOVE -5 /* Cannot move file */

#define UNALIXERR_JSON_CANNOT_PARSE -6 /* Cannot parse JSON tree */
#define UNALIXERR_JSON_MISSING_REQUIRED_KEY -7 /* Missing required key in JSON tree */
#define UNALIXERR_JSON_NON_MATCHING_TYPE -8 /* JSON object does not match the required type */

#define UNALIXERR_REGEX_COMPILE_PATTERN_FAILURE -9 /* Regex pattern compilation failed */

#define UNALIXERR_URI_SCHEME_INVALID -10 /* URI has an invalid or unrecognized scheme */
#define UNALIXERR_URI_SCHEME_MISSING -11 /* There is no scheme in the URI */
#define UNALIXERR_URI_SCHEME_SHOULD_STARTS_WITH_LETTER -12 /* URI scheme should starts with a letter */
#define UNALIXERR_URI_SCHEME_EMPTY -13 /* URI has an empty scheme */
#define UNALIXERR_URI_SCHEME_CONTAINS_INVALID_CHARACTER -14 /* URI scheme contains invalid character */
#define UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_STARTS_WITH_HYPHEN -15 /* Hostname label cannot starts with a hyphen */
#define UNALIXERR_URI_HOSTNAME_TOO_LONG -16 /* Hostname has more than 253 characters */
#define UNALIXERR_URI_HOSTNAME_TOO_SHORT -17 /* Hostname is lower than 4 characters */
#define UNALIXERR_URI_HOSTNAME_LABEL_TOO_LONG -18 /* Hostname label has more than 63 characters */
#define UNALIXERR_URI_HOSTNAME_LABEL_EMPTY -19 /* Hostname label is empty */
#define UNALIXERR_URI_HOSTNAME_LABEL_CONTAINS_INVALID_CHARACTER -20 /* Hostname label contains invalid character */
#define UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_ENDS_WITH_HYPHEN -21 /* Hostname label cannot ends with hyphen */
#define UNALIXERR_URI_PORT_OUT_OF_RANGE -22 /* Hostname port is out of range, should be between 1 and 65535 */
#define UNALIXERR_URI_PORT_CONTAINS_INVALID_CHARACTER -23 /* Hostname port contains non-numerical character */
#define UNALIXERR_URI_HOSTNAME_MISSING_CLOSING_BRACKET -24 /* Missing closing bracket in IPv6 address */
#define UNALIXERR_URI_IPV6_ADDRESS_TOO_SHORT -25 /* The IPv6 address is too short */
#define UNALIXERR_URI_IPV6_ADDRESS_TOO_LONG -26 /* The IPv6 address is too long */
#define UNALIXERR_URI_IPV6_ADDRESS_INVALID -27 /* The IPv6 address is invalid */

#define UNALIXERR_RULESETS_EMPTY -28 /* No rulesets have been loaded */
#define UNALIXERR_RULESETS_NOT_MODIFIED -29 /* The remote file was not modified since the last update */
#define UNALIXERR_RULESETS_UPDATE_AVAILABLE -30 /* The remote file was modified since the last update */
#define UNALIXERR_RULESETS_MISMATCH_HASH -31 /* The SHA256 hash of the remote file did not match */

#define UNALIXERR_DNS_GAI_FAILURE -32 /* Cannot resolve hostname */
#define UNALIXERR_DNS_CANNOT_PARSE_ADDRESS -33 /* Cannot parse IP address */

#define UNALIXERR_SOCKET_FAILURE -34 /* Connot create socket */
#define UNALIXERR_SOCKET_SETOPT_FAILURE -35 /* Cannot set option on socket */
#define UNALIXERR_SOCKET_SEND_FAILURE -36 /* Cannot send data on socket */
#define UNALIXERR_SOCKET_RECV_FAILURE -37 /* Cannot receive data on socket */
#define UNALIXERR_SOCKET_CLOSE_FAILURE -38 /* Cannot close socket */
#define UNALIXERR_SOCKET_CONNECT_FAILURE -39 /* Cannot connect to remote address */

#define UNALIXERR_SSL_FAILURE -40 /* SSL/TLS connection failure */

#define UNALIXERR_HTTP_UNSUPPORTED_VERSION -41 /* The HTTP server replied using a unsupported protocol version */
#define UNALIXERR_HTTP_MALFORMED_STATUS_CODE -42 /* Malformed HTTP status code */
#define UNALIXERR_HTTP_UNKNOWN_STATUS_CODE -43 /* Unknown HTTP status code */
#define UNALIXERR_HTTP_MALFORMED_HEADER -44 /* Malformed HTTP header */
#define UNALIXERR_HTTP_MISSING_HEADER_NAME -45 /* HTTP header has an empty name */
#define UNALIXERR_HTTP_MISSING_HEADER_VALUE -46 /* HTTP header has an empty value */
#define UNALIXERR_HTTP_HEADER_CONTAINS_INVALID_CHARACTER -47 /* HTTP header contains invalid character */
#define UNALIXERR_HTTP_HEADERS_TOO_BIG -48 /* HTTP headers exceeded max allowed size */
#define UNALIXERR_HTTP_HEADERS_INVALID_LOCATION -49 /* This HTTP redirect points to an invalid location */
#define UNALIXERR_HTTP_TOO_MANY_REDIRECTS -50 /* Too many HTTP redirects followed */
#define UNALIXERR_HTTP_BAD_STATUS_CODE -51 /* Got a unexpected HTTP status code */

#define UNALIXERR_OS_STAT_FAILURE -52 /* Cannot get file status */
#define UNALIXERR_OS_GMTIME_FAILURE -53 /* Cannot convert local time to UTC time */
#define UNALIXERR_OS_MKTIME_FAILURE -54 /* Cannot get epoch representation of time object */
#define UNALIXERR_OS_STRFTIME_FAILURE -55 /* Cannot format time object to string */
#define UNALIXERR_OS_STRPTIME_FAILURE -56 /* Cannot parse string into time object */

#define UNALIXERR_ARG_INVALID -57 /* Invalid argument passed to function call */

const char* unalix_strerror(const int code);

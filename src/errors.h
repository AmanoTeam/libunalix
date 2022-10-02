#define UNALIXERR_SUCCESS 0 /* No error */

#define UNALIXERR_FAILURE -1 /* Generic error */

#define UNALIXERR_MEMORY_ALLOCATE_FAILURE -2 /* Failed to allocate memory */

#define UNALIXERR_CANNOT_OPEN_FILE -3 /* Cannot open file for reading */

#define UNALIXERR_JSON_CANNOT_PARSE -4 /* Cannot parse JSON tree */
#define UNALIXERR_JSON_MISSING_REQUIRED_KEY -5 /* Missing required key in JSON tree */
#define UNALIXERR_JSON_NON_MATCHING_TYPE -6 /* JSON object does not match the required type */

#define UNALIXERR_PCRE2_COMPILE_PATTERN_FAILURE -7 /* PCRE2 pattern compilation failed */

#define UNALIXERR_URI_SCHEME_INVALID -8 /* URI has an invalid or unrecognized scheme */
#define UNALIXERR_URI_SCHEME_MISSING -9 /* Scheme is missing in URI */
#define UNALIXERR_URI_SCHEME_SHOULD_STARTS_WITH_LETTER -10 /* URI scheme should starts with a letter */
#define UNALIXERR_URI_SCHEME_EMPTY -11 /* URI has an empty scheme */
#define UNALIXERR_URI_SCHEME_CONTAINS_INVALID_CHARACTER -12 /* URI scheme contains invalid character */
#define UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_STARTS_WITH_HYPHEN -13 /* Hostname label cannot starts with a hyphen */
#define UNALIXERR_URI_HOSTNAME_TOO_LONG -14 /* Hostname has more than 253 characters */
#define UNALIXERR_URI_HOSTNAME_TOO_SHORT -15 /* Hostname is lower than 4 characters */
#define UNALIXERR_URI_HOSTNAME_LABEL_TOO_LONG -16 /* Hostname label has more than 63 characters */
#define UNALIXERR_URI_HOSTNAME_LABEL_EMPTY -17 /* Hostname label is empty */
#define UNALIXERR_URI_HOSTNAME_LABEL_CONTAINS_INVALID_CHARACTER -18 /* Hostname label contains invalid character */
#define UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_ENDS_WITH_HYPHEN -19 /* Hostname label cannot ends with hyphen */
#define UNALIXERR_URI_PORT_OUT_OF_RANGE -20 /* Hostname port is out of range, should be between 1 and 65535 */
#define UNALIXERR_URI_PORT_CONTAINS_INVALID_CHARACTER -21 /* Hostname port contains non-numerical character */
#define UNALIXERR_URI_HOSTNAME_MISSING_CLOSING_BRACKET -22
#define UNALIXERR_URI_IPV6_ADDRESS_TOO_SHORT -23
#define UNALIXERR_URI_IPV6_ADDRESS_TOO_LONG -24
#define UNALIXERR_URI_IPV6_ADDRESS_INVALID -25

#define UNALIXERR_RULESETS_EMPTY -26
#define UNALIXERR_RULESETS_NOT_MODIFIED -99

#define UNALIXERR_DNS_BUFFER_SIZE_TOO_SMALL -27
#define UNALIXERR_DNS_BUFFER_SIZE_TOO_HIGH -28
#define UNALIXERR_DNS_UNSUPPORTED_PROTOCOL -29
#define UNALIXERR_DNS_GAI_FAILURE -30
#define UNALIXERR_DOH_BAD_STATUS_CODE -31
#define UNALIXERR_DNS_MALFORMED_RESPONSE -32
#define UNALIXERR_DNS_NXDOMAIN -33

#define UNALIXERR_SOCKET_FAILURE -34
#define UNALIXERR_SOCKET_SETOPT_FAILURE -35
#define UNALIXERR_SOCKET_SEND_FAILURE -36
#define UNALIXERR_SOCKET_RECV_FAILURE -37
#define UNALIXERR_SOCKET_CLOSE_FAILURE -38
#define UNALIXERR_SOCKET_MAX_BUFFER_SIZE_EXCEEDED -39
#define UNALIXERR_SOCKET_CONNECT_FAILURE -40

#define UNALIXERR_SSL_FAILURE -41

#define UNALIXERR_HTTP_UNSUPPORTED_VERSION -42
#define UNALIXERR_HTTP_MALFORMED_STATUS_CODE -43
#define UNALIXERR_HTTP_UNKNOWN_STATUS_CODE -44
#define UNALIXERR_HTTP_MALFORMED_HEADER -45
#define UNALIXERR_HTTP_MISSING_HEADER_NAME -46
#define UNALIXERR_HTTP_MISSING_HEADER_VALUE -47
#define UNALIXERR_HTTP_HEADER_CONTAINS_INVALID_CHARACTER -48
#define UNALIXERR_HTTP_HEADERS_TOO_BIG -49
#define UNALIXERR_HTTP_HEADERS_INVALID_LOCATION -50
#define UNALIXERR_HTTP_TOO_MANY_REDIRECTS -51
#define UNALIXERR_HTTP_BAD_STATUS_CODE -100

#define UNALIXERR_IO_FAILURE -52

#define UNALIXERR_OS_FAILURE -53

const char* strerrorcode(const int code);

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "uri.h"
#include "utils.h"
#include "errors.h"

static const char URI_SAFE_SYMBOLS[] = "!#$%&'()*+,-./:;=?@[]_~";
static const char SCHEME_SAFE_SYMBOLS[] = "+.-";

static int parse_uri(struct URI* obj, const char* uri) {
	
	if (!uri || *uri == '\0') {
		return UNALIXERR_FAILURE;
	}
	
	const char* uri_end = strrchr(uri, '\0');
	
	/*
	Scheme
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.1
	*/
	const char* scheme_end = strstr(uri, SCHEME_SEPARATOR);
	
	if (scheme_end == NULL) {
		return UNALIXERR_URI_SCHEME_MISSING;
	}
	
	const size_t scheme_size = (size_t) (scheme_end - uri);
	
	if (scheme_size < 1) {
		return UNALIXERR_URI_SCHEME_EMPTY;
	}
	
	obj->scheme = malloc(scheme_size + 1);
	
	if (obj->scheme == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	memcpy(obj->scheme, uri, scheme_size);
	obj->scheme[scheme_size] = '\0';
	
	for (size_t index = 0; index < scheme_size; index++) {
		const char ch = obj->scheme[index];
		
		if (index == 0 && !isalpha(ch)) {
			return UNALIXERR_URI_SCHEME_SHOULD_STARTS_WITH_LETTER;
		}
		
		if (!(isalnum(ch) || strchr(SCHEME_SAFE_SYMBOLS, ch) != NULL)) {
			return UNALIXERR_URI_SCHEME_CONTAINS_INVALID_CHARACTER;
		}
		
		if (isupper(ch)) {
			obj->scheme[index] = ch + 32;
		}
	}
	
	/*
	User Information 
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1
	*/
	scheme_end += strlen(SCHEME_SEPARATOR);
	
	const char* authentication_start = scheme_end;
	const char* authentication_end = strstr(authentication_start, AT);
	
	if (authentication_end != NULL) {
		const char* separator = strstr(authentication_start, COLON);
		
		if (!separator || separator > authentication_end) {
			separator = authentication_end;
		}
		
		const size_t username_size = (size_t) (separator - authentication_start);
		
		if (username_size > 0) {
			obj->username = malloc(username_size + 1);
			
			if (obj->username == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(obj->username, authentication_start, username_size);
			obj->username[username_size] = '\0';
			
			if (!obj->has_authentication) {
				obj->has_authentication = 1;
			}
		}
		
		if (separator != authentication_end) {
			separator += strlen(COLON);
		}
		
		const size_t password_size = (size_t) (authentication_end - separator);
		
		if (password_size > 0) {
			obj->password = malloc(password_size + 1);
			
			if (obj->password == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(obj->password, separator, password_size);
			obj->password[password_size] = '\0';
			
			if (!obj->has_authentication) {
				obj->has_authentication = 1;
			}
		}
	}
	
	const char* hostname_start = authentication_end == NULL ? authentication_start : authentication_end + 1;
	const char* hostname_end = NULL;
	
	const char* path_start = strstr(hostname_start, SLASH);
	const char* query_start = strstr(hostname_start, QUESTION_MARK);
	const char* fragment_start = strstr(hostname_start, HASHTAG);
	
	const char* port_start = strstr(hostname_start, COLON);
	
	const int is_ipv6 = (*hostname_start == *BRACKET_START);
	
	if (is_ipv6) {
		hostname_end = strstr(hostname_start, BRACKET_END);
		
		if (hostname_end == NULL) {
			return UNALIXERR_URI_HOSTNAME_MISSING_CLOSING_BRACKET;
		}
		
		if (port_start != NULL) {
			port_start = strstr(hostname_end, COLON);
		}
		
		hostname_start += strlen(BRACKET_START);
	}
	
	/*
	Port
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3
	*/
	if (port_start != NULL) {
		const char* port_end = NULL;
		
		if (path_start != NULL) {
			port_end = path_start;
		} else if (query_start != NULL) {
			port_end = path_start;
		} else if (fragment_start != NULL) {
			port_end = fragment_start;
		} else {
			port_end = uri_end;
		}
		
		if (port_start < port_end) {
			port_start += strlen(COLON);
			
			const size_t port_size = (size_t) (port_end - port_start);
			
			if (port_size > 0) {
				char value[port_size + 1];
				memcpy(value, port_start, port_size);
				value[port_size] = '\0';
				
				if (!isnumeric(value)) {
					return UNALIXERR_URI_PORT_CONTAINS_INVALID_CHARACTER;
				}
				
				obj->port = atoi(value);
				
				if (obj->port < MIN_PORT_NUMBER) {
					return UNALIXERR_URI_PORT_OUT_OF_RANGE;
				}
				
				if (obj->port > MAX_PORT_NUMBER) {
					return UNALIXERR_URI_PORT_OUT_OF_RANGE;
				}
			}
			
			if (!is_ipv6) {
				hostname_end = port_start - 1;
			}
		}
	}
	
	/*
	Host
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2
	*/
	if (hostname_end == NULL) {
		if (path_start != NULL) {
			hostname_end = path_start;
		} else if (query_start != NULL) {
			hostname_end = query_start;
		} else if (fragment_start != NULL) {
			hostname_end = fragment_start;
		} else {
			hostname_end = uri_end;
		}
	}
	
	const size_t hostname_size = (size_t) (hostname_end - hostname_start);
	
	if (is_ipv6) {
		if (hostname_size < MIN_IPV6_ADDRESS_SIZE) {
			return UNALIXERR_URI_IPV6_ADDRESS_TOO_SHORT;
		}
		
		if (hostname_size > MAX_IPV6_ADDRESS_SIZE) {
			return UNALIXERR_URI_IPV6_ADDRESS_TOO_LONG;
		}
	} else {
		if (hostname_size < MIN_HOSTNAME_SIZE) {
			return UNALIXERR_URI_HOSTNAME_TOO_SHORT;
		}
		
		if (hostname_size > MAX_HOSTNAME_SIZE) {
			return UNALIXERR_URI_HOSTNAME_TOO_LONG;
		}
	}
	
	obj->hostname = malloc(hostname_size + 1);
	
	if (obj->hostname == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	memcpy(obj->hostname, hostname_start, hostname_size);
	obj->hostname[hostname_size] = '\0';
	
	if (is_ipv6) {
		if (!isipv6(obj->hostname)) {
			return UNALIXERR_URI_IPV6_ADDRESS_INVALID;
		}
		
		obj->is_ipv6 = 1;
	} else {
		const char* label_start = obj->hostname;
		
		for (size_t index = 0; index < (hostname_size + 1); index++) {
			char* ch = &obj->hostname[index];
			const size_t label_length = (size_t) (ch - label_start);
			
			if (label_length == 0 && *ch == '-') {
				return UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_STARTS_WITH_HYPHEN;
			}
			
			if (*ch == '.' || *ch == '\0') {
				if (label_length < MIN_LABEL_SIZE) {
					return UNALIXERR_URI_HOSTNAME_LABEL_EMPTY;
				}
				
				if (label_length > MAX_LABEL_SIZE) {
					return UNALIXERR_URI_HOSTNAME_LABEL_TOO_LONG;
				}
				
				if (*(ch - 1) == '-') {
					return UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_ENDS_WITH_HYPHEN;
				}
				
				label_start = ch + 1;
				
				continue;
			}
			
			if (!(isalnum(*ch) || *ch == '-')) {
				return UNALIXERR_URI_HOSTNAME_LABEL_CONTAINS_INVALID_CHARACTER;
			}
			
			if (isupper(*ch)) {
				*ch = *ch + 32;
			}
		}
		
		obj->is_ipv6 = 0;
	}
	
	/*
	Fragment
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.5
	*/
	if (fragment_start != NULL) {
		fragment_start += strlen(HASHTAG);
		
		const size_t fragment_size = (size_t) (uri_end - fragment_start);
		
		if (fragment_size > 0) {
			obj->fragment = malloc(fragment_size + 1);
			
			if (obj->fragment == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(obj->fragment, fragment_start, fragment_size);
			obj->fragment[fragment_size] = '\0';
		}
	}
	
	/*
	Query
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.4
	*/
	if (query_start != NULL) {
		query_start += strlen(QUESTION_MARK);
		
		const size_t query_size = (size_t) ((fragment_start == NULL ? uri_end : fragment_start - strlen(HASHTAG)) - query_start);
		
		if (query_size > 0) {
			obj->query = malloc(query_size + 1);
			
			if (obj->query == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(obj->query, query_start, query_size);
			obj->query[query_size] = '\0';
		}
	}
	
	/*
	Path
	https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
	*/
	if (path_start != NULL) {
		path_start += strlen(SLASH);
		
		const size_t path_size = (size_t) ((query_start == NULL ? uri_end : query_start - strlen(QUESTION_MARK)) - path_start);
		
		if (path_size > 0) {
			obj->path = malloc(path_size + 1);
			
			if (obj->path == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(obj->path, path_start, path_size);
			obj->path[path_size] = '\0';
		}
	}
	
	return UNALIXERR_SUCCESS;
	
}

int uri_parse(struct URI* obj, const char* uri) {
	
	const int code = parse_uri(obj, uri);
	
	if (code != UNALIXERR_SUCCESS) {
		uri_free(obj);
	}
	
	return code;
	
}

char* uri_stringify(const struct URI obj) {
	
	size_t mem_size = 0;
	
	if (obj.scheme != NULL) {
		mem_size += strlen(obj.scheme) + strlen(SCHEME_SEPARATOR);
	}
	
	if (obj.has_authentication) {
		if (obj.username != NULL) {
			mem_size += strlen(obj.username);
		}
		
		if (obj.password != NULL) {
			mem_size += strlen(obj.password);
		}
		
		mem_size += strlen(COLON) + strlen(AT);
	}
	
	if (obj.hostname != NULL) {
		mem_size += strlen(obj.hostname);
		
		if (obj.is_ipv6) {
			mem_size += strlen(BRACKET_START) + strlen(BRACKET_END);
		}
	}
	
	size_t port_size = intlen(obj.port);
	
	if (obj.port > 0) {
		mem_size += strlen(COLON) + port_size;
	}
	
	mem_size += strlen(SLASH);
	
	if (obj.path != NULL) {
		mem_size += strlen(obj.path);
	}
	
	if (obj.query != NULL) {
		mem_size += strlen(QUESTION_MARK) + strlen(obj.query);
	}
	
	if (obj.fragment != NULL) {
		mem_size += strlen(HASHTAG) + strlen(obj.fragment);
	}
	
	char* uri = malloc(mem_size + 1);
	memset(uri, '\0', 1);
	
	if (obj.scheme != NULL) {
		strcat(uri, obj.scheme);
		strcat(uri, SCHEME_SEPARATOR);
	}
	
	if (obj.has_authentication) {
		if (obj.username != NULL) {
			strcat(uri, obj.username);
		}
		
		strcat(uri, COLON);
		
		if (obj.password != NULL) {
			strcat(uri, obj.password);
		}
		
		strcat(uri, AT);
	}
	
	if (obj.hostname != NULL) {
		if (obj.is_ipv6) {
			strcat(uri, BRACKET_START);
		}
		
		strcat(uri, obj.hostname);
		
		if (obj.is_ipv6) {
			strcat(uri, BRACKET_END);
		}
	}
	
	if (obj.port > 0) {
		char value[port_size + 1];
		snprintf(value, sizeof(value), "%i", obj.port);
		
		strcat(uri, COLON);
		strcat(uri, value);
	}
	
	strcat(uri, SLASH);
	
	if (obj.path != NULL) {
		strcat(uri, obj.path);
	}
	
	if (obj.query != NULL) {
		strcat(uri, QUESTION_MARK);
		strcat(uri, obj.query);
	}
	
	if (obj.fragment != NULL) {
		strcat(uri, HASHTAG);
		strcat(uri, obj.fragment);
	}
	
	return uri;
	
}

void uri_free(struct URI* obj) {
	
	obj->is_ipv6 = 0;
	
	if (obj->scheme) {
		free(obj->scheme);
		obj->scheme = NULL;
	}
	
	if (obj->username) {
		free(obj->username);
		obj->username = NULL;
	}
	
	if (obj->password) {
		free(obj->password);
		obj->password = NULL;
	}
	
	obj->has_authentication = 0;
	
	if (obj->hostname) {
		free(obj->hostname);
		obj->hostname = NULL;
	}
	
	obj->port = 0;
	
	if (obj->path) {
		free(obj->path);
		obj->path = NULL;
	}
	
	if (obj->query) {
		free(obj->query);
		obj->query = NULL;
	}
	
	if (obj->fragment) {
		free(obj->fragment);
		obj->fragment = NULL;
	}

}

static char from_hex(const char ch) {
	
	if (ch <= '9' && ch >= '0') {
		return ch - '0';
	}
	
	 if (ch <= 'f' && ch >= 'a') {
		return ch - ('a' - 10);
	}
	
	if (ch <= 'F' && ch >= 'A') {
		return ch - ('A' - 10);
	}
	
	return '\0';
	
}

void rfc3986_unquote_safe(const char* src, char* dst) {
	
	const size_t src_length = strlen(src);
	
	size_t dst_position = 0;
	
	for (size_t index = 0; index < src_length; index++) {
		const char ch = src[index];
		
		if (ch == '%' && src_length > (index + 2)) {
			const char ch1 = from_hex(src[index + 1]);
			const char ch2 = from_hex(src[index + 2]);
			
			const char c = (char) ((ch1 << 4) | ch2);
			
			if ((isalnum(ch) || strchr(URI_SAFE_SYMBOLS, ch) != NULL)) {
				dst[dst_position] = c;
				index += 2;
			} else {
				dst[dst_position] = ch;
			}
		} else {
			dst[dst_position] = ch;
		}
		
		dst_position++;
	}
	
	dst[dst_position] = '\0';
	
}

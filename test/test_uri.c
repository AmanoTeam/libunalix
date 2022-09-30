#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "uri.h"
#include "errors.h"

int main() {
	
	int code = 0;
	struct URI uri = {};
	
	char* buffer = NULL;
	const char* target_url = NULL;
	
	target_url = "https://username:password@example.com:12345/example?key=value#fragment";
	
	code = uri_parse(&uri, target_url);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strcmp(uri.scheme, "https") == 0);
	assert (strcmp(uri.username, "username") == 0);
	assert (strcmp(uri.password, "password") == 0);
	assert (strcmp(uri.hostname, "example.com") == 0);
	assert (uri.port == 12345);
	assert (strcmp(uri.path, "example") == 0);
	assert (strcmp(uri.query, "key=value") == 0);
	assert (strcmp(uri.fragment, "fragment") == 0);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, target_url) == 0);
	
	uri_free(&uri);
	free(buffer);
	
	assert (uri.scheme == NULL);
	assert (uri.username == NULL);
	assert (uri.password == NULL);
	assert (uri.hostname == NULL);
	assert (uri.port == 0);
	assert (uri.path == NULL);
	assert (uri.query == NULL);
	assert (uri.fragment == NULL);
	
	code = uri_parse(&uri, "username:password@example.com:12345/example?key=value#fragment");
	assert (code == UNALIXERR_URI_SCHEME_MISSING);
	
	target_url = "https://username:password@[::]:12345/example?key=value#fragment";
	
	code = uri_parse(&uri, target_url);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (uri.is_ipv6 == 1);
	assert (strcmp(uri.scheme, "https") == 0);
	assert (strcmp(uri.username, "username") == 0);
	assert (strcmp(uri.password, "password") == 0);
	assert (strcmp(uri.hostname, "::") == 0);
	assert (uri.port == 12345);
	assert (strcmp(uri.path, "example") == 0);
	assert (strcmp(uri.query, "key=value") == 0);
	assert (strcmp(uri.fragment, "fragment") == 0);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, target_url) == 0);
	
	uri_free(&uri);
	free(buffer);
	
	target_url = "https://username:password@[0000:0000:0000:0000:0000:0000:0000:0000]:12345/example?key=value#fragment";
	
	code = uri_parse(&uri, target_url);
	assert (code == UNALIXERR_SUCCESS);
	assert (uri.is_ipv6 == 1);
	assert (strcmp(uri.scheme, "https") == 0);
	assert (strcmp(uri.username, "username") == 0);
	assert (strcmp(uri.password, "password") == 0);
	assert (strcmp(uri.hostname, "0000:0000:0000:0000:0000:0000:0000:0000") == 0);
	assert (uri.port == 12345);
	assert (strcmp(uri.path, "example") == 0);
	assert (strcmp(uri.query, "key=value") == 0);
	assert (strcmp(uri.fragment, "fragment") == 0);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, target_url) == 0);
	
	uri_free(&uri);
	free(buffer);
	
	code = uri_parse(&uri, "https://[0000:0000:0000:0000:0000:0000:0000:00000]");
	assert (code == UNALIXERR_URI_IPV6_ADDRESS_TOO_LONG);
	
	code = uri_parse(&uri, "https://[:]");
	assert (code == UNALIXERR_URI_IPV6_ADDRESS_TOO_SHORT);
	
	code = uri_parse(&uri, "http://[exe]");
	assert (code == UNALIXERR_URI_IPV6_ADDRESS_INVALID);
	
	code = uri_parse(&uri, "http://[:::");
	assert (code == UNALIXERR_URI_HOSTNAME_MISSING_CLOSING_BRACKET);
	
	code = uri_parse(&uri, "http://example.com:0/example?key=value#fragment");
	assert (code == UNALIXERR_URI_PORT_OUT_OF_RANGE);
	
	code = uri_parse(&uri, "9http://example.com");
	assert (code == UNALIXERR_URI_SCHEME_SHOULD_STARTS_WITH_LETTER);
	
	code = uri_parse(&uri, "http://example.com:65536");
	assert (code == UNALIXERR_URI_PORT_OUT_OF_RANGE);
	
	code = uri_parse(&uri, "http://exe");
	assert (code == UNALIXERR_URI_HOSTNAME_TOO_SHORT);
	
	code = uri_parse(&uri, "http://aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	assert (code == UNALIXERR_URI_HOSTNAME_TOO_LONG);
	
	code = uri_parse(&uri, "http://-exe.com");
	assert (code == UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_STARTS_WITH_HYPHEN);
	
	code = uri_parse(&uri, "http://exe-.com:65535/example?key=value#fragment");
	assert (code == UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_ENDS_WITH_HYPHEN);
	
	code = uri_parse(&uri, "https://@aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.com");
	assert (code == UNALIXERR_URI_HOSTNAME_LABEL_TOO_LONG);
	
	code = uri_parse(&uri, "https://....");
	assert (code == UNALIXERR_URI_HOSTNAME_LABEL_EMPTY);
	
	code = uri_parse(&uri, "http://æ.com");
	assert (code == UNALIXERR_URI_HOSTNAME_LABEL_CONTAINS_INVALID_CHARACTER);
	
	target_url = "http://example.com/path";
	
	code = uri_parse(&uri, target_url);
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strcmp(uri.path, "path") == 0);
	assert (uri.query == NULL);
	assert (uri.fragment == NULL);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, target_url) == 0);
	
	uri_free(&uri);
	free(buffer);
	
	code = uri_parse(&uri, "http://example.com?key=value");
	assert (code == UNALIXERR_SUCCESS);
	
	assert (uri.path == NULL);
	assert (strcmp(uri.query, "key=value") == 0);
	assert (uri.fragment == NULL);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, "http://example.com/?key=value") == 0);
	
	uri_free(&uri);
	free(buffer);
	
	code = uri_parse(&uri, "HTTP://EXAMPLE.COM/");
	assert (code == UNALIXERR_SUCCESS);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, "http://example.com/") == 0);
	
	uri_free(&uri);
	free(buffer);
	
	code = uri_parse(&uri, "http://example.com#fragment");
	assert (code == UNALIXERR_SUCCESS);
	
	assert (uri.path == NULL);
	assert (uri.query == NULL);
	assert (strcmp(uri.fragment, "fragment") == 0);
	
	buffer = uri_stringify(uri);
	
	assert (strcmp(buffer, "http://example.com/#fragment") == 0);
	
	uri_free(&uri);
	free(buffer);
	
	return 0;
	
}

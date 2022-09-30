#ifndef URI_H_INCLUDED
#define URI_H_INCLUDED

static const char HTTP_SCHEME[] = "http";
static const char HTTPS_SCHEME[] = "https";

static const int HTTP_PORT = 80;
static const int HTTPS_PORT = 443;

static const char SCHEME_SEPARATOR[] = "://";

static const int MIN_HOSTNAME_SIZE = 4;
static const int MAX_HOSTNAME_SIZE = 253;

static const int MIN_LABEL_SIZE = 1;
static const int MAX_LABEL_SIZE = 63;

static const int MAX_PORT_SIZE = 5;

static const int MIN_IPV4_ADDRESS_SIZE = 7;
static const int MAX_IPV4_ADDRESS_SIZE = 15;

static const int MIN_IPV6_ADDRESS_SIZE = 2;
static const int MAX_IPV6_ADDRESS_SIZE = 39;

static const int MIN_PORT_NUMBER = 1;
static const int MAX_PORT_NUMBER = 65535;

static const char SLASH[] = "/";
static const char QUESTION_MARK[] = "?";
static const char AND[] = "&";
static const char EQUAL[] = "=";
static const char HASHTAG[] = "#";
static const char AT[] = "@";
static const char COLON[] = ":";
static const char BRACKET_START[] = "[";
static const char BRACKET_END[] = "]";
static const char SPACE[] = " ";

struct URI {
	int is_ipv6;
	int has_authentication;
	char* username;
	char* password;
	char* scheme;
	char* hostname;
	int port;
	char* path;
	char* query;
	char* fragment;
};

int uri_parse(struct URI* obj, const char* uri);
char* uri_stringify(const struct URI obj);
void uri_free(struct URI* uri);

void rfc3986_unquote_safe(const char* src, char* dst);

#endif

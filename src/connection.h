#include <bearssl.h>

struct SSLContext {
	br_ssl_client_context sc;
	br_x509_minimal_context xc;
	unsigned char iobuf[BR_SSL_BUFSIZE_BIDI];
	br_sslio_context ioc;
	int is_initialized;
};

struct Connection {
	int fd;
	struct SSLContext ssl_context;
};

void connection_free(struct Connection* obj);

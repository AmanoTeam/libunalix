#include <bearssl.h>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "connection.h"

void connection_free(struct Connection* obj) {
	
	if (obj->ssl_context.is_initialized) {
		br_sslio_close(&obj->ssl_context.ioc);
	}
	
	close(obj->fd);
	
}
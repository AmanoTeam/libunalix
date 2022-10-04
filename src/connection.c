#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	
	#define _WIN32_WINNT 0x0600
	
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <io.h>
#else
	#include <netdb.h>
	#include <unistd.h>
#endif

#include <bearssl.h>

#include "connection.h"
#include "errors.h"

void connection_free(struct Connection* obj) {
	
	if (obj->ssl_context.is_initialized) {
		br_sslio_close(&obj->ssl_context.ioc);
	}
	
	close(obj->fd);
	
}

int connection_set_timeout(struct Connection* obj, const int timeout) {
	
	#ifdef _WIN32
		const DWORD time = timeout * 1000;
	#else
		const struct timeval time = {.tv_sec = timeout};
	#endif
	
	int code = setsockopt(obj->fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &time, sizeof(time));
	
	if (code != 0) {
		return UNALIXERR_SOCKET_SETOPT_FAILURE;
	}
	
	code = setsockopt(obj->fd, SOL_SOCKET, SO_SNDTIMEO, (char*) &time, sizeof(time));
	
	if (code != 0) {
		return UNALIXERR_SOCKET_SETOPT_FAILURE;
	}
	
	return UNALIXERR_SUCCESS;
	
}

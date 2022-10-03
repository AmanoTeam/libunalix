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

int connection_set_timeout(struct Connection* obj, const int timeout) {
	
	#ifdef _WIN32
		const DWORD time = timeout * 1000;
	#else
		const struct timeval time = {.tv_sec = timeout};
	#endif
	
	#ifdef _WIN32
		int rc = setsockopt(obj->fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &time, sizeof(time));
	#else
		int rc = setsockopt(obj->fd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
	#endif
	
	if (rc < 0) {
		return UNALIXERR_SOCKET_SET_OPT_ERROR;
	}
	
	#ifdef _WIN32
		rc = setsockopt(obj->fd, SOL_SOCKET, SO_SNDTIMEO, (char*) &time, sizeof(time));
	#else
		rc = setsockopt(obj->fd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
	#endif
	
	if (rc < 0) {
		return UNALIXERR_SOCKET_SET_OPT_ERROR;
	}
	
	return UNALIXERR_SUCCESS;
	
}
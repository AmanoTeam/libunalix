#include <stdlib.h>
#include <arpa/inet.h>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "errors.h"
#include "socket.h"

int socket_create(
	const int domain,
	const int type,
	const int protocol
) {
	
	return socket(domain, type, protocol);
	
}

int socket_set_timeout(
	int fd,
	const int timeout
) {
	
	#ifdef _WIN32
		const DWORD time = timeout * 1000;
	#else
		const struct timeval time = {.tv_sec = timeout};
	#endif
	
	#ifdef _WIN32
		int rc = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &time, sizeof(time));
	#else
		int rc = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
	#endif
	
	if (rc < 0) {
		return UNALIXERR_SOCKET_SET_OPT_ERROR;
	}
	
	#ifdef _WIN32
		rc = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*) &time, sizeof(time));
	#else
		rc = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
	#endif
	
	if (rc < 0) {
		return UNALIXERR_SOCKET_SET_OPT_ERROR;
	}
	
	return UNALIXERR_SUCCESS;
	
}

int socket_connect(
	int fd, 
	const struct sockaddr* addr,
	const socklen_t addrlen
) {
	
	const int rc = connect(fd, addr, addrlen);
	
	if (rc < 0) {
		return UNALIXERR_SOCKET_ERROR;
	}
	
	return UNALIXERR_SUCCESS;
	
}

ssize_t socket_send(
	int fd, 
	const char* src,
	const size_t src_size,
	struct sockaddr* addr,
	const socklen_t addrlen
) {
	
	return (addr != NULL) ? sendto(fd, src, src_size, 0, addr, addrlen) : send(fd, src, src_size, 0);
	
}

ssize_t socket_recv(
	int fd, 
	char* dst,
	const size_t dst_size,
	struct sockaddr* addr,
	socklen_t* addrlen
) {
	
	return (addr != NULL) ? recvfrom(fd, dst, dst_size, 0, addr, addrlen) : recv(fd, dst, dst_size, 0);
	
}

int socket_close(const int fd) {
	return (close(fd) == 0 ? UNALIXERR_SUCCESS : UNALIXERR_SOCKET_CLOSE_ERROR);
}

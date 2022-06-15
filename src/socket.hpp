#include <sys/types.h>
#include <string.h>

#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	
	#define _WIN32_WINNT 0x0600
	
	#include <winsock2.h>
	#include <ws2tcpip.h>
	
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

const int create_socket(
	const int domain,
	const int type,
	const int protocol,
	const int timeout_in_seconds
);

const void connect_socket(
	int fd, 
	const struct sockaddr *src_addr,
	const socklen_t addrlen
);

const ssize_t send_tcp_data(
	int fd, 
	const void *src,
	const size_t src_size,
	void *dst,
	const size_t dst_size
);

const ssize_t send_udp_data(
	int fd, 
	const void *src,
	const size_t src_size,
	void *dst,
	const size_t dst_size,
	const struct sockaddr *src_addr,
	const socklen_t addrlen
);

#pragma once
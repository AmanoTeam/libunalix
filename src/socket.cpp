#include <cerrno>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "socket.hpp"
#include "exceptions.hpp"

const int create_socket(
	const int domain,
	const int type,
	const int protocol,
	const int timeout_in_seconds
) {
	
	int fd = socket(domain, type, protocol);
	
	if (fd < 0) {
		throw SocketError(strerror(errno));
	}
	
	#ifdef _WIN32
		const DWORD timeout = timeout_in_seconds * 1000;
	#else
		struct timeval timeout;
		timeout.tv_sec = timeout_in_seconds;
		timeout.tv_usec = 0;
	#endif
	
	#ifdef _WIN32
		int rc = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
	#else
		int rc = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	#endif
	
	if (rc < 0) {
		close(fd);
		throw SocketError("setsockopt: " + std::string(strerror(errno)));
	}
	
	#ifdef _WIN32
		rc = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*) &timeout, sizeof(timeout));
	#else
		rc = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	#endif
	
	if (rc < 0) {
		close(fd);
		throw SocketError("setsockopt: " + std::string(strerror(errno)));
	}
	
	return fd;
}

const void connect_socket(
	int fd, 
	const struct sockaddr *src_addr,
	const socklen_t addrlen
) {
	
	const int rc = connect(fd, src_addr, addrlen);
	
	if (rc < 0) {
		close(fd);
		throw ConnectError(strerror(errno));
	}
}

const ssize_t send_tcp_data(
	int fd, 
	const void *src,
	const size_t src_size,
	void *dst,
	const size_t dst_size
) {
	
	ssize_t size;
	
	#ifdef _WIN32
		size = send(fd, (char*) src, src_size, 0);
	#else
		size = send(fd, src, src_size, 0);
	#endif
	
	if (size != src_size) {
		close(fd);
		throw SendError(strerror(errno));
	}
	
	#ifdef _WIN32
		size = recv(fd, (char*) dst, dst_size, 0);
	#else
		size = recv(fd, dst, dst_size, 0);
	#endif
	
	switch (size) {
		case -1:
			close(fd);
			throw RecvError(strerror(errno));
		case 0:
			close(fd);
			throw RecvError("Server closed connection unexpectedly");
	}
	
	return size;
}

const ssize_t send_udp_data(
	int fd, 
	const void *src,
	const size_t src_size,
	void *dst,
	const size_t dst_size,
	const struct sockaddr *src_addr,
	const socklen_t addrlen
) {
	
	ssize_t size;
	
	#ifdef _WIN32
		size = sendto(fd, (char*) src, src_size, 0, src_addr, addrlen);
	#else
		size = sendto(fd, src, src_size, 0, src_addr, addrlen);
	#endif
	
	if (size != src_size) {
		close(fd);
		throw SendError(strerror(errno));
	}
	
	#ifdef _WIN32
		size = recvfrom(fd, (char*) dst, dst_size, 0, NULL, NULL);
	#else
		size = recvfrom(fd, dst, dst_size, 0, NULL, NULL);
	#endif
	
	switch (size) {
		case -1:
			close(fd);
			throw RecvError(strerror(errno));
		case 0:
			close(fd);
			throw RecvError("Server closed connection unexpectedly");
	}
	
	return size;
}
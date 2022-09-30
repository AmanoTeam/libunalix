#include <stdlib.h>
#include <errno.h>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "callbacks.h"

/*
 * Low-level data write callback for the simplified SSL I/O API.
 */
int sock_write(void* fd, const unsigned char *buffer, size_t buffer_size) {
	
	for (;;) {
		const ssize_t wlen = write(*(int*) fd, buffer, buffer_size);
		
		if (wlen < 0 && errno == EINTR) {
			continue;
		}
		
		if (wlen == 0) {
			return -1;
		}
		
		return (int) wlen;
	}
	
}

/*
 * Low-level data read callback for the simplified SSL I/O API.
 */
int sock_read(void* fd, unsigned char* buffer, size_t buffer_size) {
	
	for (;;) {
		const ssize_t rlen = read(*(int*) fd, buffer, buffer_size);
		
		if (rlen < 0 && errno == EINTR) {
			continue;
		}
		
		if (rlen == 0) {
			return -1;
		}
		
		return (int) rlen;
	}
	
}

#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	
	#define _WIN32_WINNT 0x0600
	
	#include <winsock2.h>
	#include <ws2tcpip.h>
	
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <netdb.h>
#endif

static const size_t MIN_BUFFER_SIZE = 1;
static const size_t MAX_BUFFER_SIZE = 1000;

int socket_set_timeout(
	int fd,
	const int timeout
);

int socket_close(const int fd);
#ifdef _WIN32
	#include <windows.h>
#else
	#include <arpa/inet.h>
#endif

struct Address {
	int af;
	struct sockaddr_storage addr_storage;
	socklen_t addr_storage_size;
};

int address_parse(struct Address* obj, const char* sa, const int sa_port);

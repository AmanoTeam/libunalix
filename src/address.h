#include <arpa/inet.h>

struct Address {
	int af;
	struct sockaddr_storage addr_storage;
	socklen_t addr_storage_size;
};

int address_parse(struct Address* obj, const char* sa, const int sa_port);

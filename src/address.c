#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "errors.h"
#include "address.h"

int address_parse(struct Address* obj, const char* sa, const int sa_port) {
	
	struct sockaddr_in addr_in = {
		.sin_family = AF_INET,
		.sin_port = htons(sa_port)
	};
	
	if (inet_pton(addr_in.sin_family, sa, &addr_in.sin_addr) == 1) {
		const size_t addr_in_size = sizeof(addr_in);
		memcpy(&obj->addr_storage, &addr_in, addr_in_size);
		
		obj->af = addr_in.sin_family;
		obj->addr_storage_size = addr_in_size;
		
		return UNALIXERR_SUCCESS;
	}
	
	struct sockaddr_in6 addr_in6 = {
		.sin6_family = AF_INET6,
		.sin6_port = htons(sa_port)
	};
	
	if (inet_pton(addr_in6.sin6_family, sa, &addr_in6.sin6_addr) == 1) {
		const size_t addr_in6_size = sizeof(addr_in6);
		memcpy(&obj->addr_storage, &addr_in6, addr_in6_size);
		
		obj->af = addr_in6.sin6_family;
		obj->addr_storage_size = addr_in6_size;
		
		return UNALIXERR_SUCCESS;
	}
	
	return UNALIXERR_FAILURE;
	
}

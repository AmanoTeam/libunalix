#include <stdlib.h>

// BearSSL callbacks
int sock_write(void* fd, const unsigned char *buffer, size_t buffer_size);
int sock_read(void* fd, unsigned char* buffer, size_t buffer_size);

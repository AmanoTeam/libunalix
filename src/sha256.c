#include <stdlib.h>
#include <stdio.h>

#include <bearssl.h>

#include "errors.h"
#include "utils.h"

int sha256_digest(const char* const filename, char* dst) {
	
	FILE* file = fopen(filename, "r");
	
	if (file == NULL) {
		return UNALIXERR_FILE_CANNOT_OPEN;
	}
	
	br_sha256_context context = {0};
	br_sha256_init(&context);
	
	while (1) {
		char chunk[FILERW_MAX_CHUNK_SIZE];
		const size_t size = fread(chunk, sizeof(*chunk), sizeof(chunk), file);
		
		if (size == 0) {
			fclose(file);
			
			if (ferror(file) != 0) {
				return UNALIXERR_FILE_CANNOT_READ;
			}
			
			break;
		}
		
		br_sha256_update(&context, chunk, size);
	}
	
	char sha256[br_sha256_SIZE];
	br_sha256_out(&context, sha256);
	
	size_t dst_offset = 0;
	
	for (size_t index = 0; index < sizeof(sha256); index++) {
		const char ch = sha256[index];
		
		dst[dst_offset++] = to_hex((ch & 0xF0) >> 4);
		dst[dst_offset++] = to_hex((ch & 0x0F) >> 0);
	}
	
	return UNALIXERR_SUCCESS;
	
}

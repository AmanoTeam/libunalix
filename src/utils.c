#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
	#include <ws2tcpip.h>
	#include <windows.h>
	#include <fileapi.h>
	#include <math.h>
	
	// The number of hectonanoseconds between 1601/01/01 (windows epoch) and 1970/01/01 (unix epoch).
	static const unsigned long long EPOCH_DIFF = 116444736000000000;
	static const int HNSECS_PER_SEC = 10000000; // 100 hectonanoseconds
#else
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/stat.h>
	#include <unistd.h>
	
	static const char* const ENV_TMP[] = {
		"TMPDIR",
		"TEMP",
		"TMP",
		"TEMPDIR"
	};
	
	static const char DEFAULT_TEMPORARY_DIRECTORY[] = "/tmp/";
#endif

#include "utils.h"

int isipv6(const char* const address) {
	
	struct sockaddr_in6 addr = {0};
	const int rc = inet_pton(AF_INET6, address, &addr.sin6_addr);
	
	return (rc == 1);
	
}

size_t intlen(const int value) {
	
	int val = value;
	size_t size = 0;
	
	do {
		val /= 10;
		size++;
	} while (val > 0);
	
	return size;
	
}

char to_hex(const char ch) {
	return ch + (ch > 9 ? ('a' - 10) : '0');
}

int isnumeric(const char* const s) {
	/*
	Return true (1) if the string is a numeric string, false (0) otherwise.
	
	A string is numeric if all characters in the string are numeric and there is at least one character in the string.
	*/
	
	for (size_t index = 0; index < strlen(s); index++) {
		const char ch = s[index];
		
		if (!isdigit(ch)) {
			return 0;
		}
	}
	
	return 1;
	
}

static const char SLASH[] = "/";
static const char DOTDOT[] = "..";

static const char* basename(const char* const path) {
	/*
	Returns the final component of a pathname.
	*/
	
	const char* last_comp = path;
	
	while (1) {
		char* slash_at = (char*) strchr(last_comp, *SLASH);
		
		if (slash_at == NULL) {
			break;
		}
		
		last_comp = slash_at + 1;
	}
	
	return last_comp;
	
}
	

char* normpath(const char* const path) {
	/*
	Normalize path, eliminating double slashes, etc.
	*/
	
	char* normalized_path = (char*) malloc(strlen(path) + strlen(SLASH) + 1);
	*normalized_path = '\0';
	
	const char* comp_start = path;
	
	for (size_t index = 0; index < strlen(path) + 1; index++) {
		const char* const ch = &path[index];
		
		if (*ch == *SLASH || *ch == '\0') {
			const size_t comp_length = ch - comp_start;
			
			if (comp_length < 1 || (comp_length == 1 && *ch == *SLASH)) {
				comp_start = ch + 1;
				continue;
			}
			
			if (memcmp(comp_start, DOTDOT, strlen(DOTDOT)) != 0 || (*normalized_path != '\0' && memcmp(basename(normalized_path), DOTDOT, strlen(DOTDOT)) == 0)) {
				char comp[comp_length + 1];
				memcpy(comp, comp_start, comp_length);
				comp[comp_length] = '\0';
			
				strcat(normalized_path, SLASH);
				strcat(normalized_path, comp);
			} else if (*normalized_path != '\0') {
				*((char*) basename(normalized_path) - 1) = '\0';
			}
			
			comp_start = ch + 1;
		}
	}
	
	if (*normalized_path == '\0') {
		strcat(normalized_path, SLASH);
	}
	
	return normalized_path;
	
}

size_t countp(const char* const s, const size_t slenth, const char p) {
	/*
	Count how many times p appears in the leading part of string
	*/
	
	size_t count = 0;
	
	for (size_t index = 0; index < slenth; index++) {
		const char ch = s[index];
		
		if (ch != p) {
			return count;
		}
		
		count++;
	}
	
	return count;
	
}

int in_range(const size_t index, const size_t min, const size_t max) {
	/*
	Check whether index is in range between min and max
	*/
	
	return (index >= min && index <= max);
	
}

void httpnormpath(const char* const path, char* normalized_path) {
	/*
	Normalize path, eliminating double slashes, etc.
	*/
	*normalized_path = '\0';
	
	const char* comp_start = path;
	const char* suffix = NULL;
	
	for (size_t index = 0; index < strlen(path) + 1; index++) {
		const char* const ch = &path[index];
		
		const int query_or_fragment = (*ch == '?' || *ch == '#');
		
		if (*ch == *SLASH || *ch == '\0' || query_or_fragment) {
			const size_t comp_length = ch - comp_start;
			
			if (comp_length < 1) {// || (comp_length == 1 && *ch == *SLASH)) {
				comp_start = ch + 1;
				continue;
			}
			
			const size_t dotted = countp(comp_start, comp_length, '.');
			
			const char* const bname = basename(normalized_path);
			const size_t bname_length = strlen(bname);
			
			if (!(in_range(comp_length, 1, 2) && dotted == comp_length) || (in_range(bname_length, 1, 2) && countp(bname, bname_length, '.') == bname_length)) {
				char comp[comp_length + 1];
				memcpy(comp, comp_start, comp_length);
				comp[comp_length] = '\0';
				
				strcat(normalized_path, SLASH);
				strcat(normalized_path, comp);
			} else if (*normalized_path != '\0') {
				for (size_t index = 0; index < dotted; index++) {
					*((char*) basename(normalized_path) - 1) = '\0';
				}
			}
			
			if (query_or_fragment) {
				suffix = ch;
				break;
			}
			
			comp_start = ch + 1;
		}
	}
	
	if (*normalized_path == '\0') {
		strcat(normalized_path, SLASH);
	}
	
	if (suffix != NULL) {
		strcat(normalized_path, suffix);
	}
	
}

int file_exists(const char* const filename) {
	
	/*
	Returns 1 (true) if file exists and is a regular file or symlink, 0 (false) otherwise.
	Directories, device files, named pipes and sockets return false.
	*/
	
	#ifdef _WIN32
		return (GetFileAttributesA(filename) & FILE_ATTRIBUTE_DIRECTORY) == 0;
	#else
		struct stat st = {0};
		return (stat(filename, &st) == 0 && S_ISREG(st.st_mode));
	#endif
	
}

time_t get_last_modification_time(const char* const filename) {
	/*
	Returns the file's last modification time.
	*/
	
	#ifdef _WIN32
		WIN32_FIND_DATA data = {0};
		const HANDLE handle = FindFirstFile(filename, &data);
		
		if (handle == INVALID_HANDLE_VALUE) {
			return 0;
		}
		
		FindClose(handle);
		
		// Convert Windows file time (100-nanosecond intervals since 1601-01-01T00:00:00Z) to Unix epoch.
		const unsigned long long windows_epoch = (data.ftLastWriteTime.dwLowDateTime | (unsigned long long) data.ftLastWriteTime.dwHighDateTime << 32);
		const time_t unix_epoch = floor((windows_epoch - EPOCH_DIFF) / HNSECS_PER_SEC);
		
		return unix_epoch;
	#else
		struct stat st = {0};
		
		if (stat(filename, &st) != 0) {
			return 0;
		}
		
		return st.st_mtime;
	#endif
	
}

char* get_temporary_directory(void) {
	/*
	Returns the temporary directory of the current user for applications to save temporary files in.
	On Windows, it calls GetTempPathA(). On Posix based platforms, it will check TMPDIR, TEMP, TMP and TEMPDIR environment variables in order.
	*/
	
	#ifdef _WIN32
		char path[MAX_PATH + 1];
		const DWORD size = GetTempPathA(sizeof(path), path);
		
		if (size == 0) {
			return NULL;
		}
		
		char* temporary_directory = (char*) malloc(size + 1);
		
		if (temporary_directory == NULL) {
			return NULL;
		}
		
		strcpy(temporary_directory, path);
		
		return temporary_directory;
	#else
		for (size_t index = 0; index < sizeof(ENV_TMP) / sizeof(*ENV_TMP); index++) {
			const char* const name = ENV_TMP[index];
			const char* const value = getenv(name);
			
			if (value != NULL) {
				const int has_trailing_slash = *(strchr(value, '\0') - 1) == *SLASH;
				
				char* temporary_directory = (char*) malloc(strlen(value) + (has_trailing_slash ? 0 : strlen(SLASH)) + 1);
				
				if (temporary_directory == NULL) {
					return temporary_directory;
				}
				
				strcpy(temporary_directory, value);
				
				if (!has_trailing_slash) {
					strcat(temporary_directory, SLASH);
				}
				
				return temporary_directory;
			}
		}
		
		char* temporary_directory = (char*) malloc(sizeof(DEFAULT_TEMPORARY_DIRECTORY));
		
		if (temporary_directory == NULL) {
			return temporary_directory;
		}
		
		memcpy(temporary_directory, DEFAULT_TEMPORARY_DIRECTORY, sizeof(DEFAULT_TEMPORARY_DIRECTORY));
		
		return temporary_directory;
	#endif
	
	
}

int remove_file(const char* const filename) {
	/*
	Removes the file. On Windows, ignores the read-only attribute.
	*/
	
	#ifdef _WIN32
		return DeleteFile(filename) == 1;
	#else
		return unlink(filename) == 0;
	#endif
	
}

int copy_file(const char* const source, const char* const destination) {
	/*
	Copies a file from source to destination.
	*/
	
	#ifdef _WIN32
		return CopyFileA(source, destination, FALSE) == 1;
	#else
		// Generic version which works for any platform
		FILE* sfile = fopen(source, "r");
		
		if (sfile == NULL) {
			return 0;
		}
		
		FILE* dfile = fopen(destination, "wb");
		
		if (dfile == NULL) {
			return 0;
		}
		
		while (1) {
			char chunk[FILERW_MAX_CHUNK_SIZE];
			const size_t rsize = fread(chunk, sizeof(*chunk), sizeof(chunk), sfile);
			
			if (rsize == 0) {
				fclose(sfile);
				fclose(dfile);
				
				if (ferror(sfile) != 0) {
					return 0;
				}
				
				break;
			}
			
			const size_t wsize = fwrite(chunk, sizeof(*chunk), rsize, dfile);
			
			if (wsize != rsize) {
				fclose(sfile);
				fclose(dfile);
				
				return 0;
			}
		}
		
		return 1;
	#endif

}

int move_file(const char* const source, const char* const destination) {
	/*
	Moves a file from source to destination. Symlinks are not followed: if source is a symlink, it is itself moved, not it's target.
	If destination already exists, it will be overwritten.
	*/
	
	#ifdef _WIN32
		return MoveFileExA(source, destination, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING) == 1;
	#else
		const int code = rename(source, destination);
		
		if (code == 0) {
			return 1;
		}
		
		if (errno == EXDEV) {
			const int code = copy_file(source, destination);
			
			if (code) {
				remove_file(source);
			};
			
			return code;
		}
		
		return code;
	#endif
	
}
/*
int main() {
	char a[5000];
	httpnormpath("/yuuii/_77/89/.", a);
	puts(a);
}
*/
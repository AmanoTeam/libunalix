#include <time.h> 

int isipv6(const char* host);
size_t intlen(const int value);
int isnumeric(const char* const s);
int in_range(const size_t index, const size_t min, const size_t max);
size_t countp(const char* const s, const size_t slenth, const char p);
void httpnormpath(const char* const path, char* normalized_path);
char to_hex(const char ch);

// Filesystem operations
time_t get_last_modification_time(const char* const filename);
int file_exists(const char* const filename);
char* get_temporary_directory(void);
int remove_file(const char* const filename);
int move_file(const char* const source, const char* const destination);

// Max chunk size for R/W operations on files
static const size_t FILERW_MAX_CHUNK_SIZE = 8192;
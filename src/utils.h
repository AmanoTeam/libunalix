#include <time.h> 

int isipv6(const char* host);
size_t intlen(const int value);
int isnumeric(const char* const s);
int in_range(const size_t index, const size_t min, const size_t max);
size_t countp(const char* const s, const size_t slenth, const char p);
void httpnormpath(const char* const path, char* normalized_path);
time_t get_last_modification_time(const char* const filename);
int file_exists(const char* const filename);
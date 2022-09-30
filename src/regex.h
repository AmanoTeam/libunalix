#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

//int regex_compile(pcre2_code** obj, const char* pattern);
int regex_match(const pcre2_code* pattern, const PCRE2_SPTR subject);
void regex_strip(const pcre2_code* pattern, const PCRE2_SPTR subject, char** destination);

void concatenate_pattern(const char* const src, char* dst);

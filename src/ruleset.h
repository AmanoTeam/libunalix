#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

struct Rules {
	size_t total_items;
	pcre2_code** items;
};

struct Ruleset {
	pcre2_code* url_pattern;
	struct Rules rules;
	struct Rules raw_rules;
	struct Rules referral_marketing;
	struct Rules exceptions;
	struct Rules redirections;
};

struct Rulesets {
	size_t offset;
	size_t size;
	struct Ruleset* items;
};

int unalix_ruleset_check_update(const char* const filename, const char* const url);
int unalix_ruleset_update(const char* const filename, const char* const url, const char* const sha256_url, const char* const temporary_directory);

int unalix_load_file(const char* const filename);
int unalix_load_string(const char* const string);

void unalix_unload_rulesets(void);
struct Rulesets get_rulesets(void);

int unalix_clean_url(
	const char* source_url,
	char** target_url,
	const int ignore_referral_marketing,
	const int ignore_rules,
	const int ignore_exceptions,
	const int ignore_raw_rules,
	const int ignore_redirections,
	const int strip_empty,
	const int strip_duplicates
);

int unalix_unshort_url(
	const char* const source_url,
	char** target_url,
	const int ignore_referral_marketing,
	const int ignore_rules,
	const int ignore_exceptions,
	const int ignore_raw_rules,
	const int ignore_redirections,
	const int strip_empty,
	const int strip_duplicates,
	const char* const user_agent,
	const int timeout
);

int unalix_load_file(const char* const filename);
int unalix_load_string(const char* const string);

int unalix_ruleset_check_update(const char* const filename, const char* const url);
int unalix_ruleset_update(const char* const filename, const char* const url, const char* const sha256_url, const char* const temporary_directory);

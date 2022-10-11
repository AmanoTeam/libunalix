#include <assert.h>
#include <string.h>

#include "unalix.h"
#include "errors.h"
#include "ruleset.h"

int main() {
	
	int code = 0;
	
	char* source_url = NULL;
	char* target_url = NULL;
	
	int ignore_referral_marketing = 0;
	int ignore_rules = 0;
	int ignore_exceptions = 0;
	int ignore_raw_rules = 0;
	int ignore_redirections = 0;
	int strip_empty = 0;
	int strip_duplicates = 0;
	
	code = unalix_clean_url(
		NULL,
		NULL,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_RULESETS_EMPTY);
	
	code = unalix_load_file("./test/rulesets/rulesets.json");
	assert (code == UNALIXERR_SUCCESS);
	
	code = unalix_load_file("./test/rulesets/wrong_urlpattern_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/wrong_rules_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/wrong_rawrules_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/wrong_redirections_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/wrong_exceptions_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/wrong_referralmarketing_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/wrong_providers_type.json");
	assert (code == UNALIXERR_JSON_NON_MATCHING_TYPE);
	
	code = unalix_load_file("./test/rulesets/missing_providers_key.json");
	assert (code == UNALIXERR_JSON_MISSING_REQUIRED_KEY);
	
	code = unalix_load_file("./test/rulesets/missing_urlpattern_key.json");
	assert (code == UNALIXERR_JSON_MISSING_REQUIRED_KEY);
	
	code = unalix_load_file("./test/rulesets/this_file_does_not_exists.json");
	assert (code == UNALIXERR_CANNOT_OPEN_FILE);
	
	source_url = "https://example.com/?exampleRule=exampleValue";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "https://example.com/") == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		1,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	source_url = "https://example.com/?exampleRedirection=https://example.org/";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	printf("%i\n", code);
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "https://example.org/") == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		1,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	source_url = "https://example.com/?exampleRedirection=example.org/";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "http://example.org/") == 0);
	free(target_url);
	
	source_url = "https://example.com/exampleRawRule";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "https://example.com/") == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		1,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	source_url = "https://example.com/exampleException?exampleRule=exampleValue";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		1,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "https://example.com/exampleException") == 0);
	free(target_url);
	
	source_url = "https://example.com/?exampleReferralMarketing=exampleValue";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "https://example.com/") == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		1,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	source_url = "http://example.com/?p1=&p2=";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		1,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "http://example.com/") == 0);
	free(target_url);
	
	source_url = "http://example.com/?a=value&a=value2";
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		strip_duplicates
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, source_url) == 0);
	free(target_url);
	
	code = unalix_clean_url(
		source_url,
		&target_url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		strip_empty,
		1
	);
	
	assert (code == UNALIXERR_SUCCESS);
	assert (strcmp(target_url, "http://example.com/?a=value") == 0);
	free(target_url);
	
	unalix_unload_rulesets();
	
	return 0;
	
}
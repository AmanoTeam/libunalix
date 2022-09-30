#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "errors.h"
#include "regex.h"
#include "ruleset.h"

static const char URL_PATTERN[] = "urlPattern";
static const char COMPLETE_PROVIDER[] = "completeProvider";
static const char RULES[] = "rules";
static const char RAW_RULES[] = "rawRules";
static const char REFERRAL_MARKETING[] = "referralMarketing";
static const char EXCEPTIONS[] = "exceptions";
static const char REDIRECTIONS[] = "redirections";

static const char PROVIDERS[] = "providers";

static const char* const names[] = {
	(const char*) RULES,
	(const char*) RAW_RULES,
	(const char*) REFERRAL_MARKETING,
	(const char*) EXCEPTIONS,
	(const char*) REDIRECTIONS
};

static const char PREFIX_PROVIDER_IGNORE[] = "ClearURLsTest";

static const char PREFIX_EXTENDED_PATTERN[] = "(?:^|&|\\?)";
static const char SUFFIX_EXTENDED_PATTERN[] = "(?:=[^&]*)?";

static int regex_compile(const char* src, pcre2_code** dst) {
	
	int error_number = 0;
	PCRE2_SIZE error_offset = 0;
	
	pcre2_code* re = pcre2_compile(
		(PCRE2_SPTR) src,
		PCRE2_ZERO_TERMINATED,
		0,
		&error_number,
		&error_offset,
		NULL
	);
	
	if (re == NULL) {
		return UNALIXERR_PCRE2_COMPILE_PATTERN_FAILURE;
	}
	
	*dst = re;
	
	return UNALIXERR_SUCCESS;
	
}

static struct Rulesets rulesets = {0};

static int load_ruleset(json_t* tree) {
	
	json_t* providers = json_object_get(tree, PROVIDERS);
	
	if (providers == NULL) {
		puts("a");
		return UNALIXERR_JSON_MISSING_REQUIRED_KEY;
	}
	
	if (!json_is_object(providers)) {
		return UNALIXERR_JSON_NON_MATCHING_TYPE;
	}
	
	const char* key = NULL;
	json_t* value = NULL;
	
	json_object_foreach(providers, key, value) {
		if (memcmp(key, PREFIX_PROVIDER_IGNORE, strlen(PREFIX_PROVIDER_IGNORE)) == 0) {
			continue;
		}
		
		if (!json_is_object(value)) {
			return UNALIXERR_JSON_NON_MATCHING_TYPE;
		}
		
		/*
		completeProvider
		https://docs.clearurls.xyz/latest/specs/rules/#completeprovider
		*/
		const json_t* obj = json_object_get(value, COMPLETE_PROVIDER);
		
		if (!(obj == NULL || json_typeof(obj) == JSON_NULL)) {
			if (!json_is_boolean(obj)) {
				return UNALIXERR_JSON_NON_MATCHING_TYPE;
			}
			
			const int complete_provider = json_boolean_value(obj);
			
			// Providers with this key set to true should be ignored (See https://github.com/ClearURLs/Rules/issues/15#issuecomment-1043443837)
			if (complete_provider) {
				continue;
			}
		}
		
		struct Ruleset ruleset = {0};
		
		/*
		urlPattern
		https://docs.clearurls.xyz/latest/specs/rules/#urlpattern
		*/
		obj = json_object_get(value, URL_PATTERN);
		
		if (obj == NULL) {
			puts("z");
			return UNALIXERR_JSON_MISSING_REQUIRED_KEY;
		}
		
		if (!json_is_string(obj)) {
			return UNALIXERR_JSON_NON_MATCHING_TYPE;
		}
		
		const char* const url_pattern = json_string_value(obj);
		const int code = regex_compile(url_pattern, &ruleset.url_pattern);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		/*
		rules
		https://docs.clearurls.xyz/latest/specs/rules/#rules
		
		rawRules
		https://docs.clearurls.xyz/latest/specs/rules/#rawrules
		
		referralMarketing
		https://docs.clearurls.xyz/latest/specs/rules/#referralmarketing
		
		exceptions
		https://docs.clearurls.xyz/latest/specs/rules/#exceptions
		
		redirections
		https://docs.clearurls.xyz/latest/specs/rules/#redirections
		*/
		for (size_t index = 0; index < sizeof(names) / sizeof(*names); index++) {
			const char* const name = names[index];
			const json_t* const obj = json_object_get(value, name);
			
			if (obj == NULL || json_typeof(obj) == JSON_NULL) {
				continue;
			}
			
			if (!json_is_array(obj)) {
				return UNALIXERR_JSON_NON_MATCHING_TYPE;
			}
			
			size_t array_index = 0;
			json_t *array_item = NULL;
			const size_t array_size = json_array_size(obj);
			
			if (array_size < 1) {
				continue;
			}
			
			struct Rules rules = {
				.total_items = array_size,
				.items = malloc(sizeof(pcre2_code*) * array_size)
			};
			
			if (rules.items == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			const int should_modify_pattern = ((strcmp(name, RULES) == 0) || (strcmp(name, REFERRAL_MARKETING) == 0));
			
			json_array_foreach(obj, array_index, array_item) {
				if (!json_is_string(array_item)) {
					return UNALIXERR_JSON_NON_MATCHING_TYPE;
				}
				
				const char* const src = json_string_value(array_item);
				pcre2_code* dst = NULL;
				
				int code = 0;
				
				if (should_modify_pattern) {
					char extended_src[strlen(PREFIX_EXTENDED_PATTERN) + strlen(src) + strlen(SUFFIX_EXTENDED_PATTERN) + 1];
					strcpy(extended_src, PREFIX_EXTENDED_PATTERN);
					strcat(extended_src, src);
					strcat(extended_src, SUFFIX_EXTENDED_PATTERN);
					
					code = regex_compile(extended_src, &dst);
				} else {
					code = regex_compile(src, &dst);
				}
				
				if (code != UNALIXERR_SUCCESS) {
					return code;
				}
				
				rules.items[array_index] = dst;
			}
			
			if (strcmp(name, RULES) == 0) {
				ruleset.rules = rules;
			} else if (strcmp(name, RAW_RULES) == 0) {
				ruleset.raw_rules = rules;
			} else if (strcmp(name, REFERRAL_MARKETING) == 0) {
				ruleset.referral_marketing = rules;
			} else if (strcmp(name, EXCEPTIONS) == 0) {
				ruleset.exceptions = rules;
			} else if (strcmp(name, REDIRECTIONS) == 0) {
				ruleset.redirections = rules;
			}
		}
		
		const size_t size = rulesets.size + sizeof(ruleset) * 1;
		struct Ruleset* items = realloc(rulesets.items, size);
		
		if (items == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		rulesets.items = items;
		rulesets.size = size;
		
		rulesets.items[rulesets.offset++] = ruleset;
	}
	
	return UNALIXERR_SUCCESS;
	
}

void unalix_unload_rureturn(void) {
	
	for (size_t index = 0; index < rulesets.offset; index++) {
		struct Ruleset* ruleset = &rulesets.items[index];
		
		if (ruleset->url_pattern != NULL) {
			pcre2_code_free(ruleset->url_pattern);
			ruleset->url_pattern = NULL;
		}
		
		const struct Rules objects[] = {
			ruleset->rules,
			ruleset->raw_rules,
			ruleset->referral_marketing,
			ruleset->exceptions,
			ruleset->redirections
		};
		
		for (size_t index = 0; index < sizeof(objects) / sizeof(*objects); index++) {
			struct Rules object = objects[index];
			
			if (object.items != NULL) {
				for (size_t index = 0; index < object.total_items; index++) {
					pcre2_code_free(object.items[index]);
				}
				
				free(object.items);
				object.items = NULL;
				object.total_items = 0;
			}
		}
	}
	
	rulesets.offset = 0;
	rulesets.size = 0;
	
	free(rulesets.items);
	rulesets.items = NULL;
	
}

int unalix_load_file(const char* const filename) {
	
	json_t* tree = json_load_file(filename, 0, NULL);
	
	if (tree == NULL) {
		return UNALIXERR_JSON_CANNOT_PARSE;
	}
	
	const int code = load_ruleset(tree);
	
	json_decref(tree);
	
	return code;
	
}

int unalix_load_string(const char* const string) {
	
	json_t* tree = json_loads(string, 0, NULL);
	
	if (tree == NULL) {
		return UNALIXERR_JSON_CANNOT_PARSE;
	}
	
	const int code = load_ruleset(tree);
	
	json_decref(tree);
	
	return code;
	
}

struct Rulesets get_rulesets(void) {
	return rulesets;
}
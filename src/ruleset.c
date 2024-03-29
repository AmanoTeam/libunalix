#ifdef _WIN32
	#include <stdio.h>
#else
	#define _XOPEN_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <jansson.h>

#include "errors.h"
#include "regex.h"
#include "ruleset.h"
#include "http.h"
#include "utils.h"
#include "sha256.h"

static const char URL_PATTERN[] = "urlPattern";
static const char COMPLETE_PROVIDER[] = "completeProvider";
static const char RULES[] = "rules";
static const char RAW_RULES[] = "rawRules";
static const char REFERRAL_MARKETING[] = "referralMarketing";
static const char EXCEPTIONS[] = "exceptions";
static const char REDIRECTIONS[] = "redirections";

static const char PROVIDERS[] = "providers";

static const char* const ARRAY_KEYS[] = {
	(const char*) RULES,
	(const char*) RAW_RULES,
	(const char*) REFERRAL_MARKETING,
	(const char*) EXCEPTIONS,
	(const char*) REDIRECTIONS
};

static const char PREFIX_PROVIDER_IGNORE[] = "ClearURLsTest";

static const char PREFIX_EXTENDED_PATTERN[] = "(?:^|&|\\?)";
static const char SUFFIX_EXTENDED_PATTERN[] = "(?:=[^&]*)?";

static const char RULESET_TEMPORARY_FILE[] = "ruleset.json";

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
		return UNALIXERR_REGEX_COMPILE_PATTERN_FAILURE;
	}
	
	*dst = re;
	
	return UNALIXERR_SUCCESS;
	
}

static struct Rulesets rulesets = {0};

static int load_ruleset(json_t* tree, struct Rulesets* rulesets) {
	
	json_t* providers = json_object_get(tree, PROVIDERS);
	
	if (providers == NULL) {
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
		for (size_t index = 0; index < sizeof(ARRAY_KEYS) / sizeof(*ARRAY_KEYS); index++) {
			const char* const name = ARRAY_KEYS[index];
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
				.items = (pcre2_code**) malloc(sizeof(pcre2_code*) * array_size)
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
		
		const size_t size = rulesets->size + sizeof(ruleset) * 1;
		struct Ruleset* items = (struct Ruleset*) realloc(rulesets->items, size);
		
		if (items == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		rulesets->items = items;
		rulesets->size = size;
		
		rulesets->items[rulesets->offset++] = ruleset;
	}
	
	return UNALIXERR_SUCCESS;
	
}

void rulesets_free(struct Rulesets* rulesets) {
	
	for (size_t index = 0; index < rulesets->offset; index++) {
		struct Ruleset* ruleset = &rulesets->items[index];
		
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
	
	rulesets->offset = 0;
	rulesets->size = 0;
	
	free(rulesets->items);
	rulesets->items = NULL;
	
}

static int load_file(const char* const filename, struct Rulesets* dst) {
	
	json_t* tree = json_load_file(filename, 0, NULL);
	
	if (tree == NULL) {
		return UNALIXERR_JSON_CANNOT_PARSE;
	}
	
	const int code = load_ruleset(tree, dst);
	
	json_decref(tree);
	
	return code;
	
}

struct Rulesets get_rulesets(void) {
	return rulesets;
}

static int check_ruleset_update(
	const char* const filename,
	const char* const url,
	struct HTTPContext* context
) {
	
	if (!file_exists(filename)) {
		return UNALIXERR_OS_STAT_FAILURE;
	}
	
	const time_t local_last_modified = get_last_modification_time(filename);
	
	if (local_last_modified == 0) {
		return UNALIXERR_OS_STAT_FAILURE;
	}
	
	struct tm time = {0};
	
	#ifdef _WIN32
		if (gmtime_s(&time, &local_last_modified) != 0) {
			return UNALIXERR_OS_GMTIME_FAILURE;
		}
	#else
		if (gmtime_r(&local_last_modified, &time) == NULL) {
			return UNALIXERR_OS_GMTIME_FAILURE;
		}
	#endif
	
	// Convert local time to UTC time
	const time_t last_modified = mktime(&time);
	
	if (last_modified == -1) {
		return UNALIXERR_OS_MKTIME_FAILURE;
	}
	
	char if_modified_since[HTTP_DATE_SIZE + 1];
	const size_t size = strftime(if_modified_since, sizeof(if_modified_since), HTTP_DATE_FORMAT, &time);
	
	if (size != HTTP_DATE_SIZE) {
		return UNALIXERR_OS_STRFTIME_FAILURE;
	}
	
	int code = http_request_set_url(context, url);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_add_header(context, HTTP_HEADER_USER_AGENT, HTTP_DEFAULT_USER_AGENT);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_add_header(context, HTTP_HEADER_ACCEPT, "application/json");
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_add_header(context, HTTP_HEADER_IF_MODIFIED_SINCE, if_modified_since);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_send(context);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	const struct HTTPStatus* status = http_response_get_status(context);
	
	if (status->code == NOT_MODIFIED) {
		return UNALIXERR_RULESETS_NOT_MODIFIED;
	}
	
	if (status->code != OK) {
		return UNALIXERR_HTTP_BAD_STATUS_CODE;
	}
	
	/*
	Some server implementations ignores the "If-Modified-Since" header and always returns 200 OK, even if the remote file was not modified since the specified time.
	But luckily, some of these servers still includes the "Last-Modified" header in the response, so we are able to manually compare both timestamps.
	*/
	const struct HTTPHeader* const header = http_response_get_header(context, HTTP_HEADER_LAST_MODIFIED);
	
	if (header != NULL) {
		struct tm time = {0};
		
		#ifdef _WIN32
			char month[4];
			
			if (sscanf(header->value, "%*s, %d %s %d %d:%d:%d GMT", &time.tm_mday, month, &time.tm_year, &time.tm_hour, &time.tm_min, &time.tm_sec) != 6) {
				return UNALIXERR_OS_STRPTIME_FAILURE;
			}
			
			if (strcmp(month, "Jan") == 0) {
				time.tm_mon = 0;
			} else if (strcmp(month, "Feb") == 0) {
				time.tm_mon = 1;
			} else if (strcmp(month, "Mar") == 0) {
				time.tm_mon = 2;
			} else if (strcmp(month, "Apr") == 0) {
				time.tm_mon = 3;
			} else if (strcmp(month, "May") == 0) {
				time.tm_mon = 4;
			} else if (strcmp(month, "Jun") == 0) {
				time.tm_mon = 5;
			} else if (strcmp(month, "Jul") == 0) {
				time.tm_mon = 6;
			} else if (strcmp(month, "Aug") == 0) {
				time.tm_mon = 7;
			} else if (strcmp(month, "Sep") == 0) {
				time.tm_mon = 8;
			} else if (strcmp(month, "Oct") == 0) {
				time.tm_mon = 9;
			} else if (strcmp(month, "Nov") == 0) {
				time.tm_mon = 10;
			} else if (strcmp(month, "Dec") == 0) {
				time.tm_mon = 11;
			}
		#else
			if (strptime(header->value, HTTP_DATE_FORMAT, &time) == NULL) {
				return UNALIXERR_OS_STRPTIME_FAILURE;
			}
		#endif
		
		const time_t remote_last_modified = mktime(&time);
		
		if (remote_last_modified == -1) {
			return UNALIXERR_OS_MKTIME_FAILURE;
		}
		
		// Compare local file's timestamp with remote file's timestamp
		if (remote_last_modified <= last_modified) {
			return UNALIXERR_RULESETS_NOT_MODIFIED;
		}
	}
	
	return UNALIXERR_RULESETS_UPDATE_AVAILABLE;
	
}

static int ruleset_update(const char* const filename, const char* const url, const char* const sha256_url, const char* const temporary_directory, struct HTTPContext* context) {
	
	int code = http_request_set_url(context, url);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_add_header(context, HTTP_HEADER_USER_AGENT, HTTP_DEFAULT_USER_AGENT);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_add_header(context, HTTP_HEADER_ACCEPT, "application/json");
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	code = http_request_send(context);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	const struct HTTPStatus* status = http_response_get_status(context);
	
	if (status->code != OK) {
		return UNALIXERR_HTTP_BAD_STATUS_CODE;
	}
	
	char* directory = (char*) temporary_directory;
	
	if (directory == NULL) {
		directory = get_temporary_directory();
	}
	
	char ruleset_file[strlen(directory) + strlen(RULESET_TEMPORARY_FILE) + 1];
	strcpy(ruleset_file, directory);
	strcat(ruleset_file, RULESET_TEMPORARY_FILE);
	
	if (temporary_directory == NULL) {
		free(directory);
	}
	
	FILE* file = fopen(ruleset_file, "wb");
	
	if (file == NULL) {
		return UNALIXERR_FILE_CANNOT_OPEN;
	}
	
	code = http_response_read(context, file);
	
	fclose(file);
	
	http_context_free(context);
	
	if (code != UNALIXERR_SUCCESS) {
		remove_file(ruleset_file);
		
		return code;
	}
	
	if (sha256_url != NULL) {
		int code = http_request_set_url(context, sha256_url);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		code = http_request_add_header(context, HTTP_HEADER_USER_AGENT, HTTP_DEFAULT_USER_AGENT);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		code = http_request_add_header(context, HTTP_HEADER_ACCEPT, "text/plain");
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		code = http_request_send(context);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		const struct HTTPStatus* status = http_response_get_status(context);
		
		if (status->code != OK) {
			return UNALIXERR_HTTP_BAD_STATUS_CODE;
		}
		
		code = http_response_read(context, NULL);
		
		if (code != UNALIXERR_SUCCESS) {
			remove_file(ruleset_file);
			
			return code;
		}
		
		const struct HTTPBody* body = http_response_get_body(context);
		
		if (body->size < (br_sha256_SIZE * 2)) {
			return UNALIXERR_RULESETS_MISMATCH_HASH;
		}
		
		char rsha256[br_sha256_SIZE * 2];
		memcpy(rsha256, body->content, sizeof(rsha256));
		
		http_context_free(context);
		
		// Normalize SHA256 hash, converting uppercase characters to lower ones
		for (size_t index = 0; index < sizeof(rsha256); index++) {
			char* ch = &rsha256[index];
			
			if (*ch >= 'A' && *ch <= 'F') {
				*ch = *ch + 32;
			}
		}
		
		char lsha256[SHA256_DIGEST_SIZE];
		code = sha256_digest(ruleset_file, lsha256);
		
		if (code != UNALIXERR_SUCCESS) {
			remove_file(ruleset_file);
			
			return code;
		}
		
		// Compare both hashes to ensure the remote file was not tampered
		if (memcmp(lsha256, rsha256, SHA256_DIGEST_SIZE) != 0) {
			remove_file(ruleset_file);
			
			return UNALIXERR_RULESETS_MISMATCH_HASH;
		}
	}
	
	// Attempt to load the ruleset manually before moving it to the specified location
	struct Rulesets rulesets = {0};
	code = load_file(ruleset_file, &rulesets);
	
	rulesets_free(&rulesets);
	
	if (code != UNALIXERR_SUCCESS) {
		remove_file(ruleset_file);
		
		return code;
	}
	
	if (!move_file(ruleset_file, filename)) {
		return UNALIXERR_FILE_CANNOT_MOVE;
	}
	
	return UNALIXERR_SUCCESS;
	
}

int unalix_ruleset_check_update(const char* const filename, const char* const url) {
	
	if (filename == NULL || *filename == '\0' || url == NULL || *url == '\0') {
		return UNALIXERR_ARG_INVALID;
	}
	
	struct HTTPContext context = {
		.request = {
			.version = HTTP10,
			.method = HEAD
		},
		.connection = {
			.timeout = HTTP_DEFAULT_TIMEOUT
		},
	};
	
	const int code = check_ruleset_update(filename, url, &context);
	
	http_context_free(&context);
	
	return code;
	
}

int unalix_ruleset_update(const char* const filename, const char* const url, const char* const sha256_url, const char* const temporary_directory) {
	
	if (filename == NULL || *filename == '\0' || url == NULL || *url == '\0' || temporary_directory == NULL || *temporary_directory == '\0') {
		return UNALIXERR_ARG_INVALID;
	}
	
	struct HTTPContext context = {
		.request = {
			.version = HTTP10,
			.method = GET
		},
		.connection = {
			.timeout = HTTP_DEFAULT_TIMEOUT
		}
	};
	
	const int code = ruleset_update(filename, url, sha256_url, temporary_directory, &context);
	
	http_context_free(&context);
	
	return code;
	
}

int unalix_load_file(const char* const filename) {
	
	if (filename == NULL || *filename == '\0') {
		return UNALIXERR_ARG_INVALID;
	}
	
	return load_file(filename, &rulesets);
}

int unalix_load_string(const char* const string) {
	
	if (string == NULL || *string == '\0') {
		return UNALIXERR_ARG_INVALID;
	}
	
	json_t* tree = json_loads(string, 0, NULL);
	
	if (tree == NULL) {
		return UNALIXERR_JSON_CANNOT_PARSE;
	}
	
	const int code = load_ruleset(tree, &rulesets);
	
	json_decref(tree);
	
	return code;
	
}

void unalix_unload_rulesets(void) {
	rulesets_free(&rulesets);
}

/*
int main() {
	printf("%i\n", update_ruleset("/storage/emulated/0/z.json", "https://rules2.clearurls.xyz/data.minify.json", "https://rules2.clearurls.xyz/rules.minify.hash"));
	puts(HTTP_DEFAULT_USER_AGENT);
}
*/
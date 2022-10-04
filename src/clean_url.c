#include <stdlib.h>
#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "errors.h"
#include "regex.h"
#include "ruleset.h"
#include "uri.h"
#include "query.h"
#include "utils.h"

static void prepend_scheme_if_needed(char* src) {
	
	struct URI uri = {0};
	
	const int code = uri_parse(&uri, src);
	
	uri_free(&uri);
	
	if (code == UNALIXERR_URI_SCHEME_MISSING) {
		const size_t prefix_length = strlen(HTTP_SCHEME) + strlen(SCHEME_SEPARATOR);
		char prefix[prefix_length + 1];
		
		strcpy(prefix, HTTP_SCHEME);
		strcat(prefix, SCHEME_SEPARATOR);
		
		memmove(src + prefix_length, src, strlen(src) + 1);
		memcpy(src, prefix, prefix_length);
	}
	
}

int clean_url(
	const char* const source_url,
	char** target_url,
	const int ignore_referral_marketing,
	const int ignore_rules,
	const int ignore_exceptions,
	const int ignore_raw_rules,
	const int ignore_redirections,
	const int strip_empty,
	const int strip_duplicates
) {
	
	const struct Rulesets rulesets = get_rulesets();
	
	if (rulesets.offset < 1) {
		return UNALIXERR_RULESETS_EMPTY;
	}
	
	struct URI uri = {0};
	
	const int code = uri_parse(&uri, source_url);
	
	if (code != UNALIXERR_SUCCESS) {
		return code;
	}
	
	const PCRE2_SPTR subject = (PCRE2_SPTR) source_url;
	
	for (size_t index = 0; index < rulesets.offset; index++) {
		const struct Ruleset ruleset = rulesets.items[index];
		
		if (regex_match(ruleset.url_pattern, subject)) {
			if (!ignore_exceptions) {
				int exception_matched = 0;
				
				for (size_t index = 0; index < ruleset.exceptions.total_items; index++) {
					const pcre2_code* pattern = ruleset.exceptions.items[index];
					
					if (regex_match(pattern, subject)) {
						exception_matched = 1;
						break;
					}
				}
				
				if (exception_matched) {
					continue;
				}
			}
			
			if (!ignore_redirections) {
				for (size_t index = 0; index < ruleset.redirections.total_items; index++) {
					const pcre2_code* redirection = ruleset.redirections.items[index];
					
					pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(redirection, NULL);
					const int code = pcre2_match(redirection, subject, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
					
					if (code > 0) {
						const PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
						
						const char* start = (const char*) subject + ovector[2];
						const size_t length = ovector[3] - ovector[2];
						
						char url[length + 1];
						memcpy(url, start, length);
						url[length] = '\0';
						
						char safe_unquoted_url[sizeof(url) + strlen(HTTPS_SCHEME) + strlen(SCHEME_SEPARATOR)];
						rfc3986_unquote_safe(url, safe_unquoted_url);
						
						// Workaround for URLs without scheme (see https://github.com/ClearURLs/Addon/issues/71)
						prepend_scheme_if_needed(&safe_unquoted_url[0]);
						
						const int rc = clean_url(
							safe_unquoted_url,
							target_url,
							ignore_referral_marketing,
							ignore_rules,
							ignore_exceptions,
							ignore_raw_rules,
							ignore_redirections,
							strip_empty,
							strip_duplicates
						);
						
						uri_free(&uri);
						pcre2_match_data_free(match_data);
						
						return rc;
					}
					
					pcre2_match_data_free(match_data);
				}
			}
			
			if (uri.query != NULL && !ignore_rules) {
				for (size_t index = 0; index < ruleset.rules.total_items; index++) {
					const pcre2_code* pattern = ruleset.rules.items[index];
					const PCRE2_SPTR subject = (PCRE2_SPTR) uri.query;
					
					regex_strip(pattern, subject, &uri.query);
					
					if (uri.query == NULL) {
						break;
					}
				}
			}
			
			if (uri.query != NULL && !ignore_referral_marketing) {
				for (size_t index = 0; index < ruleset.referral_marketing.total_items; index++) {
					const pcre2_code* pattern = ruleset.referral_marketing.items[index];
					const PCRE2_SPTR subject = (PCRE2_SPTR) uri.query;
					
					regex_strip(pattern, subject, &uri.query);
					
					if (uri.query == NULL) {
						break;
					}
				}
			}
			
			// The fragment might contains tracking fields as well
			if (uri.fragment != NULL && !ignore_rules) {
				for (size_t index = 0; index < ruleset.rules.total_items; index++) {
					const pcre2_code* pattern = ruleset.rules.items[index];
					const PCRE2_SPTR subject = (PCRE2_SPTR) uri.fragment;
					
					regex_strip(pattern, subject, &uri.fragment);
					
					if (uri.fragment == NULL) {
						break;
					}
				}
			}
			
			if (uri.fragment != NULL && !ignore_referral_marketing) {
				for (size_t index = 0; index < ruleset.referral_marketing.total_items; index++) {
					const pcre2_code* pattern = ruleset.referral_marketing.items[index];
					const PCRE2_SPTR subject = (PCRE2_SPTR) uri.fragment;
					
					regex_strip(pattern, subject, &uri.fragment);
					
					if (uri.fragment == NULL) {
						break;
					}
				}
			}
			
			if (uri.path != NULL && !ignore_raw_rules) {
				for (size_t index = 0; index < ruleset.raw_rules.total_items; index++) {
					const pcre2_code* pattern = ruleset.raw_rules.items[index];
					const PCRE2_SPTR subject = (PCRE2_SPTR) uri.path;
					
					regex_strip(pattern, subject, &uri.path);
					
					if (uri.path == NULL) {
						break;
					}
				}
			}
		}
	}
	
	if (uri.query != NULL) {
		struct Query query = {0};
		
		const int code = query_parse(&query, uri.query);
		
		if (code != UNALIXERR_SUCCESS) {
			uri_free(&uri);
			query_free(&query);
			
			return code;
		}
		
		size_t position = 0;
		struct Parameter* parameters = NULL;
		
		if (strip_duplicates) {
			parameters = malloc(query.size);
			
			if (parameters == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
		}
		
		uri.query = realloc(uri.query, strlen(uri.query) * 3 + 1);
		
		if (uri.query == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		memset(uri.query, '\0', 1);
		
		for (size_t index = 0; index < query.position; index++) {
			const struct Parameter parameter = query.parameters[index];
			
			if (strip_empty && (!parameter.key || !parameter.value)) {
				continue;
			}
			
			if (strip_duplicates) {
				int is_duplicate = 0;
				
				for (size_t index = 0; index < position; index++) {
					const struct Parameter other = parameters[index];
					
					is_duplicate = ((!parameter.key && !other.key) || (parameter.key && other.key && strcmp(parameter.key, other.key) == 0));
					
					if (is_duplicate) {
						break;
					}
				}
				
				if (is_duplicate) {
					continue;
				}
				
				parameters[position++] = parameter;
			}
			
			if (*uri.query != '\0') {
				strcat(uri.query, AND);
			}
			
			if (parameter.key != NULL) {
				strcat(uri.query, parameter.key);
			}
			
			strcat(uri.query, EQUAL);
			
			if (parameter.value != NULL) {
				strcat(uri.query, parameter.value);
			}
		}
		
		query_free(&query);
		
		if (parameters != NULL) {
			free(parameters);
		}
		
		if (*uri.query == '\0') {
			free(uri.query);
			uri.query = NULL;
		}
	}
	
	*target_url = uri_stringify(uri);
	
	uri_free(&uri);
	
	return UNALIXERR_SUCCESS;
	
}

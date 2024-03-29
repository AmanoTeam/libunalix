#include "clean_url.h"
#include "http.h"
#include "errors.h"
#include "utils.h"
#include "ruleset.h"

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
) {
	
	if (source_url == NULL || *source_url == '\0' || target_url == NULL) {
		return UNALIXERR_ARG_INVALID;
	}
	
	int total_redirects = 0;
	
	char* url = NULL;
	char* location = NULL;
	
	while (1) {
		int code = unalix_clean_url(
			location == NULL ? source_url : location,
			&url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			strip_empty,
			strip_duplicates
		);
		
		if (location != NULL) {
			free(location);
			location = NULL;
		}
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		if (*target_url != NULL) {
			free(*target_url);
			*target_url = NULL;
		}
		
		*target_url = url;
		
		struct HTTPContext context = {
			.request = {
				.version = HTTP10,
				.method = GET
			},
			.connection = {
				.timeout = (timeout > 0) ? timeout : HTTP_DEFAULT_TIMEOUT
			}
		};
		
		code = http_request_add_header(&context, "Accept", "*/*");
		
		if (code != UNALIXERR_SUCCESS) {
			http_context_free(&context);
			
			return code;
		}
		
		const char* const ua = (user_agent == NULL || *user_agent == '\0') ? HTTP_DEFAULT_USER_AGENT : user_agent;
		
		code = http_request_add_header(&context, HTTP_HEADER_USER_AGENT, ua);
		
		if (code != UNALIXERR_SUCCESS) {
			http_context_free(&context);
			
			return code;
		}
		
		code = http_request_set_url(&context, url);
		
		if (code != UNALIXERR_SUCCESS) {
			http_context_free(&context);
			
			return code;
		}
		
		code = http_request_send(&context);
		
		if (code != UNALIXERR_SUCCESS) {
			http_context_free(&context);
			
			return code;
		}
		
		code = http_get_redirect(&context, &location);
		
		if (code != UNALIXERR_SUCCESS) {
			http_context_free(&context);
			
			return code;
		}
		
		http_context_free(&context);
		
		if (location == NULL) {
			break;
		}
		
		if (location != NULL) {
			total_redirects++;
			
			if (total_redirects > HTTP_DEFAULT_MAX_REDIRECTS) {
				return UNALIXERR_HTTP_TOO_MANY_REDIRECTS;
			}
			
			continue;
		}
	}
	
	return UNALIXERR_SUCCESS;
	
}

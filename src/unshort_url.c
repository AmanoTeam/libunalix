#include "clean_url.h"
#include "http.h"
#include "errors.h"
#include "utils.h"
#include "ruleset.h"

int unshort_url(
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

	int total_redirects = 0;
	
	char* url = NULL;
	char* location = NULL;
	
	while (1) {
		int code = clean_url(
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
		
		struct Connection connection = {0};
		struct HTTPRequest request = {.version = HTTP10, .method = GET};
		struct HTTPResponse response = {0};
		
		code = http_headers_add(&request.headers, "Accept", "*/*");
		
		if (code != UNALIXERR_SUCCESS) {
			http_free(&connection, &request, &response);
			return code;
		}
		
		if (user_agent == NULL || *user_agent = '\0') {
			code = http_headers_add(&request.headers, "User-Agent", HTTP_USER_AGENT);
		} else {
			code = http_headers_add(&request.headers, "User-Agent", user_agent);
		}
		
		if (code != UNALIXERR_SUCCESS) {
			http_free(&connection, &request, &response);
			return code;
		}
		
		code = http_request_set_url(&request, url);
		
		if (code != UNALIXERR_SUCCESS) {
			http_free(&connection, &request, &response);
			return code;
		}
		
		code = http_request_send(&connection, &request, &response);
		
		if (code != UNALIXERR_SUCCESS) {
			http_free(&connection, &request, &response);
			return code;
		}
		
		code = http_get_redirect(request, response, &location);
		
		if (code != UNALIXERR_SUCCESS) {
			http_free(&connection, &request, &response);
			return code;
		}
		
		http_free(&connection, &request, &response);
		
		if (location == NULL) {
			break;
		}
		
		if (location != NULL) {
			total_redirects++;
			
			if (total_redirects > HTTP_MAX_REDIRECTS) {
				return UNALIXERR_HTTP_TOO_MANY_REDIRECTS;
			}
			
			continue;
		}
	}
	
	return UNALIXERR_SUCCESS;
	
}

int main() {
	printf("%i\n", unalix_load_file("/storage/emulated/0/z.json"));
	char* f = NULL;
	printf("%i\n", unshort_url("http://g.co/yuuii/_77/89", &f, 0,0,0,0,0,0,0));
	puts(f);
}

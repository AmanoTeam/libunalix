#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "errors.h"
#include "regex.h"

int regex_cobmpile(const char* src, pcre2_code** dst) {
	
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

int regex_match(const pcre2_code* pattern, const PCRE2_SPTR subject) {
	
	pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(pattern, NULL);
	const int code = pcre2_match(pattern, subject, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
	pcre2_match_data_free(match_data);
	
	return (code > 0);
	
}

void regex_strip(const pcre2_code* pattern, const PCRE2_SPTR subject, char** destination) {
	
	if (regex_match(pattern, subject)) {
		PCRE2_UCHAR output[strlen(*destination)];
		PCRE2_SIZE output_length = sizeof(output);
		
		pcre2_substitute(
			pattern,
			subject,
			PCRE2_ZERO_TERMINATED,
			0,
			PCRE2_SUBSTITUTE_GLOBAL,
			NULL,
			NULL,
			NULL,
			0,
			(PCRE2_UCHAR8*) &output,
			&output_length
		);
		
		if (output_length < 1) {
			free(*destination);
			*destination = NULL;
			
			return;
		}
		
		strncpy(*destination, (char*) &output, output_length + 1);
	}
	
}

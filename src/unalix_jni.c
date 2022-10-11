#include <stdlib.h>
#include <jni.h>

#include "unalix_jni.h"
#include "unalix.h"
#include "errors.h"

const char* get_exception_class(const int code) {
	
	switch (code) {
		case UNALIXERR_MEMORY_ALLOCATE_FAILURE:
			return "com/amanoteam/libunalix/exceptions/UnalixMemoryException";
		case UNALIXERR_FILE_CANNOT_OPEN:
		case UNALIXERR_FILE_CANNOT_READ:
		case UNALIXERR_FILE_CANNOT_WRITE:
		case UNALIXERR_FILE_CANNOT_MOVE:
			return "com/amanoteam/libunalix/exceptions/UnalixFileException";
		case UNALIXERR_JSON_CANNOT_PARSE:
		case UNALIXERR_JSON_MISSING_REQUIRED_KEY:
		case UNALIXERR_JSON_NON_MATCHING_TYPE:
			return "com/amanoteam/libunalix/exceptions/UnalixJSONException";
		case UNALIXERR_REGEX_COMPILE_PATTERN_FAILURE:
			return "com/amanoteam/libunalix/exceptions/UnalixRegexException";
		case UNALIXERR_URI_SCHEME_INVALID:
		case UNALIXERR_URI_SCHEME_MISSING:
		case UNALIXERR_URI_SCHEME_SHOULD_STARTS_WITH_LETTER:
		case UNALIXERR_URI_SCHEME_EMPTY:
		case UNALIXERR_URI_SCHEME_CONTAINS_INVALID_CHARACTER:
		case UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_STARTS_WITH_HYPHEN:
		case UNALIXERR_URI_HOSTNAME_TOO_LONG:
		case UNALIXERR_URI_HOSTNAME_TOO_SHORT:
		case UNALIXERR_URI_HOSTNAME_LABEL_TOO_LONG:
		case UNALIXERR_URI_HOSTNAME_LABEL_EMPTY:
		case UNALIXERR_URI_HOSTNAME_LABEL_CONTAINS_INVALID_CHARACTER:
		case UNALIXERR_URI_HOSTNAME_LABEL_CONNOT_ENDS_WITH_HYPHEN:
		case UNALIXERR_URI_PORT_OUT_OF_RANGE:
		case UNALIXERR_URI_PORT_CONTAINS_INVALID_CHARACTER:
		case UNALIXERR_URI_HOSTNAME_MISSING_CLOSING_BRACKET:
		case UNALIXERR_URI_IPV6_ADDRESS_TOO_SHORT:
		case UNALIXERR_URI_IPV6_ADDRESS_TOO_LONG:
		case UNALIXERR_URI_IPV6_ADDRESS_INVALID:
			return "com/amanoteam/libunalix/exceptions/UnalixURIException";
		case UNALIXERR_RULESETS_EMPTY:
		case UNALIXERR_RULESETS_NOT_MODIFIED:
		case UNALIXERR_RULESETS_UPDATE_AVAILABLE:
		case UNALIXERR_RULESETS_MISMATCH_HASH:
			return "com/amanoteam/libunalix/exceptions/UnalixRulesetsException";
		case UNALIXERR_DNS_GAI_FAILURE:
		case UNALIXERR_DNS_CANNOT_PARSE_ADDRESS:
			return "com/amanoteam/libunalix/exceptions/UnalixDNSException";
		case UNALIXERR_SOCKET_FAILURE:
		case UNALIXERR_SOCKET_SETOPT_FAILURE:
		case UNALIXERR_SOCKET_SEND_FAILURE:
		case UNALIXERR_SOCKET_RECV_FAILURE:
		case UNALIXERR_SOCKET_CLOSE_FAILURE:
		case UNALIXERR_SOCKET_CONNECT_FAILURE:
			return "com/amanoteam/libunalix/exceptions/UnalixSocketException";
		case UNALIXERR_SSL_FAILURE:
			return "com/amanoteam/libunalix/exceptions/UnalixSSLException";
		case UNALIXERR_HTTP_UNSUPPORTED_VERSION:
		case UNALIXERR_HTTP_MALFORMED_STATUS_CODE:
		case UNALIXERR_HTTP_UNKNOWN_STATUS_CODE:
		case UNALIXERR_HTTP_MALFORMED_HEADER:
		case UNALIXERR_HTTP_MISSING_HEADER_NAME:
		case UNALIXERR_HTTP_MISSING_HEADER_VALUE:
		case UNALIXERR_HTTP_HEADER_CONTAINS_INVALID_CHARACTER:
		case UNALIXERR_HTTP_HEADERS_TOO_BIG:
		case UNALIXERR_HTTP_HEADERS_INVALID_LOCATION:
		case UNALIXERR_HTTP_TOO_MANY_REDIRECTS:
		case UNALIXERR_HTTP_BAD_STATUS_CODE:
			return "com/amanoteam/libunalix/exceptions/UnalixHTTPException";
		case UNALIXERR_OS_STAT_FAILURE:
		case UNALIXERR_OS_GMTIME_FAILURE:
		case UNALIXERR_OS_MKTIME_FAILURE:
		case UNALIXERR_OS_STRFTIME_FAILURE:
		case UNALIXERR_OS_STRPTIME_FAILURE:
			return "com/amanoteam/libunalix/exceptions/UnalixOSException";
		default:
			return "com/amanoteam/libunalix/exceptions/UnalixException";
	}

}

jboolean Java_com_amanoteam_libunalix_LibUnalix_rulesetCheckUpdate(
	JNIEnv *env,
	const jobject obj,
	const jstring filename,
	const jstring url
) {
	(void) obj;
	
	const char* const cfilename = (*env)->GetStringUTFChars(env, filename, NULL);
	const char* const curl = (*env)->GetStringUTFChars(env, url, NULL);
	
	const int code = unalix_ruleset_check_update(cfilename, curl);
	
	(*env)->ReleaseStringUTFChars(env, filename, cfilename);
	(*env)->ReleaseStringUTFChars(env, url, curl);
	
	if (code == UNALIXERR_RULESETS_UPDATE_AVAILABLE) {
		return (jboolean) 1;
	}
	
	if (code == UNALIXERR_RULESETS_NOT_MODIFIED) {
		return (jboolean) 0;
	}
	
	(*env)->ThrowNew(
		env,
		(*env)->FindClass(
			env,
			get_exception_class(code)
		),
		unalix_strerror(code)
	);
	
	return (jboolean) 0;
	
}

void Java_com_amanoteam_libunalix_LibUnalix_rulesetUpdate(
	JNIEnv *env,
	const jobject obj,
	const jstring filename,
	const jstring url,
	const jstring sha256_url,
	const jstring temporary_directory
) {
	(void) obj;
	
	const char* const cfilename = (*env)->GetStringUTFChars(env, filename, NULL);
	const char* const curl = (*env)->GetStringUTFChars(env, url, NULL);
	const char* const csha256_url = (*env)->GetStringUTFChars(env, sha256_url, NULL);
	const char* const ctemporary_directory = (*env)->GetStringUTFChars(env, temporary_directory, NULL);
	
	const int code = unalix_ruleset_update(cfilename, curl, csha256_url, ctemporary_directory);
	
	(*env)->ReleaseStringUTFChars(env, filename, cfilename);
	(*env)->ReleaseStringUTFChars(env, url, curl);
	(*env)->ReleaseStringUTFChars(env, sha256_url, csha256_url);
	(*env)->ReleaseStringUTFChars(env, temporary_directory, ctemporary_directory);
	
	if (code != UNALIXERR_SUCCESS) {
		(*env)->ThrowNew(
			env,
			(*env)->FindClass(
				env,
				get_exception_class(code)
			),
			unalix_strerror(code)
		);
	}
	
}

jstring Java_com_amanoteam_libunalix_LibUnalix_cleanUrl(
	JNIEnv *env,
	const jobject obj,
	const jstring url,
	const jboolean ignoreReferralMarketing,
	const jboolean ignoreRules,
	const jboolean ignoreExceptions,
	const jboolean ignoreRawRules,
	const jboolean ignoreRedirections,
	const jboolean stripEmpty,
	const jboolean stripDuplicates
) {
	(void) obj;
	
	const char* const curl = (*env)->GetStringUTFChars(env, url, NULL);
	
	char* target_url = NULL;
	
	const int code = unalix_clean_url(
		curl,
		&target_url,
		ignoreReferralMarketing,
		ignoreRules,
		ignoreExceptions,
		ignoreRawRules,
		ignoreRedirections,
		stripEmpty,
		stripDuplicates
	);
	
	(*env)->ReleaseStringUTFChars(env, url, curl);
	
	if (code == UNALIXERR_SUCCESS) {
		const jstring string = (*env)->NewStringUTF(env, target_url);
		free(target_url);
		
		return string;
	}
	
	(*env)->ThrowNew(
		env,
		(*env)->FindClass(
			env,
			get_exception_class(code)
		),
		unalix_strerror(code)
	);
	
	return NULL;
	
}

jstring Java_com_amanoteam_libunalix_LibUnalix_unshortUrl(
	JNIEnv *env,
	const jobject obj,
	const jstring url,
	const jboolean ignoreReferralMarketing,
	const jboolean ignoreRules,
	const jboolean ignoreExceptions,
	const jboolean ignoreRawRules,
	const jboolean ignoreRedirections,
	const jboolean stripEmpty,
	const jboolean stripDuplicates,
	const jstring userAgent,
	const jint timeout
) {
	(void) obj;
	
	const char* const curl = (*env)->GetStringUTFChars(env, url, NULL);
	const char* const cuserAgent = (*env)->GetStringUTFChars(env, userAgent, NULL);
	
	char* target_url = NULL;
	
	const int code = unalix_unshort_url(
		curl,
		&target_url,
		ignoreReferralMarketing,
		ignoreRules,
		ignoreExceptions,
		ignoreRawRules,
		ignoreRedirections,
		stripEmpty,
		stripDuplicates,
		cuserAgent,
		timeout
	);
	
	(*env)->ReleaseStringUTFChars(env, url, curl);
	(*env)->ReleaseStringUTFChars(env, userAgent, cuserAgent);
	
	if (code == UNALIXERR_SUCCESS) {
		const jstring string = (*env)->NewStringUTF(env, target_url);
		free(target_url);
		
		return string;
	}
	
	(*env)->ThrowNew(
		env,
		(*env)->FindClass(
			env,
			get_exception_class(code)
		),
		unalix_strerror(code)
	);
	
	return NULL;
	
}

void Java_com_amanoteam_libunalix_LibUnalix_loadFile(
	JNIEnv *env,
	const jobject obj,
	const jstring filename
) {
	
	(void) obj;
	
	const char* const cfilename = (*env)->GetStringUTFChars(env, filename, NULL);
	
	const int code = unalix_load_file(cfilename);
	
	(*env)->ReleaseStringUTFChars(env, filename, cfilename);
	
	if (code != UNALIXERR_SUCCESS) {
		(*env)->ThrowNew(
			env,
			(*env)->FindClass(
				env,
				get_exception_class(code)
			),
			unalix_strerror(code)
		);
	}
	
}

void Java_com_amanoteam_libunalix_LibUnalix_loadString(
	JNIEnv *env,
	const jobject obj,
	const jstring string
) {
	
	(void) obj;
	
	const char* const cstring = (*env)->GetStringUTFChars(env, string, NULL);
	
	const int code = unalix_load_string(cstring);
	
	(*env)->ReleaseStringUTFChars(env, string, cstring);
	
	if (code != UNALIXERR_SUCCESS) {
		(*env)->ThrowNew(
			env,
			(*env)->FindClass(
				env,
				get_exception_class(code)
			),
			unalix_strerror(code)
		);
	}
	
}
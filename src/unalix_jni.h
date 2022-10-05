#include <jni.h>

extern "C"

JNIEXPORT jboolean JNICALL Java_com_amanoteam_libunalix_LibUnalix_rulesetCheckUpdate(
	JNIEnv *env,
	const jobject obj,
	const jstring filename,
	const jstring url
);

extern "C"

JNIEXPORT void JNICALL Java_com_amanoteam_libunalix_LibUnalix_rulesetUpdate(
	JNIEnv *env,
	const jobject obj,
	const jstring filename,
	const jstring url,
	const jstring sha256_url
)

extern "C"

JNIEXPORT jstring JNICALL Java_com_amanoteam_libunalix_LibUnalix_cleanUrl(
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
);

extern "C"

JNIEXPORT jstring JNICALL Java_com_amanoteam_libunalix_LibUnalix_unshortUrl(
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
);
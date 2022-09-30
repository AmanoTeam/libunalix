#include <curl/curl.h>


struct CURL* curl = NULL;

static char* CURL_DOH_URL = NULL;
static long CURL_TIMEOUT = 0L;
static char* CURL_USERAGENT = NULL;

static long CURL_PROXYTYPE = 0;

CURLcode unalix_set_proxy(
	const enum ProxyType type,
	const char* const hostname,
	const int port
) {
	
	CURLcode code = CURLE_OK;
	
	switch (type) {
		case PROXY_HTTP:
			code = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
			break;
		case PROXY_SOCKS4:
			code = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
			break;
		case PROXY_SOCKS4A:
			code = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4A);
			break;
		case PROXY_SOCKS5:
			code = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
			break;
		case PROXY_SOCKS5H:
			code = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
			break;
		default:
			code = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
			break;
	}
	
	if (code != CURLE_OK) {
		return code;
	}
	
	code = curl_easy_setopt(curl, CURLOPT_PROXY, hostname);
	
	if (code != CURLE_OK) {
		return code;
	}
	
	code = curl_easy_setopt(curl, CURLOPT_PROXYPORT, port);
	
	return code;
	
}

CURLcode unalix_set_user_agent(const char* const user_agent) {
	return curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
}

static void init_curl() {
	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	
	
}

CURL* get_curl(void) {
	return curl;
}

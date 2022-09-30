#include <curl/curl.h>

CURL* get_curl(void);

enum ProxyType {
	PROXY_HTTP;
	PROXY_SOCKS4;
	PROXY_SOCKS4A;
	PROXY_SOCKS5;
	PROXY_SOCKS5H;
};
	
	
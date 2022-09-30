#include "proxy.h"
#include "dns.h"

struct ProxySettings {
	enum Proxy specification;
	struct URI uri;
	struct Address address;
}

struct DNSSettings {
	enum DNS specification;
	struct URI uri;
	struct Address address;
};

struct TimeoutSettings {
	int timeout;
};
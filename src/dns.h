enum QType {
	A = 1,
	AAAA = 28
};

enum DNS {
	DNS_OVER_HTTPS,
	DNS_OVER_TLS,
	DNS_OVER_PLAIN_UDP,
	DNS_OVER_PLAIN_TCP,
};

struct DNSRequest {
	enum QType qtype;
	const char* domain_name;
	enum DNS specification;
};
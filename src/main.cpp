#include <iostream>
#include <iomanip>

#include "http_request.hpp"
#include "dns.hpp"
#include "unshort_url.hpp"

int main() {
	HTTPRequest r = HTTPRequest("http://ipv6.google.com");
	
	r.set_http_method(GET);
	//r.set_data({'a'});
	r.set_http_version(HTTP10);
	r.add_header("User-Agent", "cu");
	
	
	
	std::cout << unshort_url("http://g.co") << std::endl;
	
	while (true) {
		const DNSResponse addresses = dns_query("ipv6.google.com", 80, "udp://one.one.one.one", 5);
		
		std::cout << addresses.get_address() << std::endl;
	}
	
	/*
	std::cout << dns_query("ipv6.google.com", 5, "tls://one.one.one.one") << std::endl;
	std::cout << dns_query("ipv6.google.com", 5, "https://cloudflare-dns.com/dns-query") << std::endl;
	std::cout << dns_query("ipv6.google.com", 5, "tcp://1.0.0.1") << std::endl;
	std::cout << dns_query("ipv6.google.com", 5, "udp://1.0.0.1") << std::endl;
	std::cout << dns_query("ipv6.google.com", 5, "") << std::endl;
	*/
}
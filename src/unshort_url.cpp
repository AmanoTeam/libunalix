#include <regex>
#include <string>
#include <algorithm>
#include <cstdio>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "clean_url.hpp"
#include "unshort_url.hpp"
#include "ssl.hpp"
#include "http_utils.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_utils.hpp"
#include "uri.hpp"
#include "dns.hpp"
#include "exceptions.hpp"
#include "socket.hpp"
#include "socks5.hpp"
#include "utils.hpp"

const std::string unshort_url(
	const std::string url,
	const bool ignore_referral_marketing,
	const bool ignore_rules,
	const bool ignore_exceptions,
	const bool ignore_raw_rules,
	const bool ignore_redirections,
	const bool skip_blocked,
	const int timeout,
	const int max_redirects,
	const std::string user_agent,
	const std::string dns,
	const std::string proxy,
	const std::string proxy_username,
	const std::string proxy_password
) {
	
	int total_redirects = 0;
	
	std::string this_url = clean_url(
		url,
		ignore_referral_marketing,
		ignore_rules,
		ignore_exceptions,
		ignore_raw_rules,
		ignore_redirections,
		skip_blocked
	);
	
	if (!(starts_with(this_url, "https://") || starts_with(this_url, "http://"))) {
		throw UnsupportedProtocolError("Unrecognized URI or unsupported protocol");
	}
	
	while (true) {
		HTTPRequest request = HTTPRequest(this_url);
		request.set_http_method(GET);
		request.set_http_version(HTTP10);
		
		request.add_header("Accept", "*/*");
		request.add_header("Accept-Encoding", "identity");
		request.add_header("Connection", "close");
		
		if (!user_agent.empty()) {
			request.add_header("User-Agent", user_agent);
		}
		
		const std::vector<char> raw_request = request.get_request();
		
		const URI uri = request.get_uri();
		
		const char* data = raw_request.data();
		const size_t data_size = raw_request.size();
		
		char response[1024];
		const size_t response_size = sizeof(response);
		
		std::string raw_response;
		
		if (proxy.empty()) {
			int fd;
			
			try {
				DNSResponse answer = dns_query(
					uri.is_ipv6() ? uri.get_ipv6_host() : uri.get_host(),
					uri.get_port(),
					dns,
					timeout
				);
				
				const struct sockaddr* sock_addr = answer.get_sockaddr();
				const int sock_addr_size = answer.get_sockaddr_size();
				const int addr_family = answer.get_qtype() == A ? AF_INET : AF_INET6;
				
				fd = create_socket(addr_family, SOCK_STREAM, IPPROTO_TCP, timeout);
				
				connect_socket(fd, sock_addr, sock_addr_size);
				
				if (uri.get_scheme() == "https") {
					send_encrypted_data(
						fd,
						uri.get_host().c_str(),
						data,
						data_size,
						response,
						response_size
					);
				} else {
					send_tcp_data(
						fd,
						data,
						data_size,
						response,
						response_size
					);
				}
			} catch (UnalixException &exception) {
				exception.set_url(this_url);
				
				throw;
			}
			
			close(fd);
			
			raw_response = std::string(response);
		} else {
			try {
				raw_response = send_proxied_tcp_data(
					data,
					uri.get_host(),
					uri.get_port(),
					proxy,
					proxy_username,
					proxy_password,
					dns,
					timeout
				);
			} catch (UnalixException &exception) {
				exception.set_url(this_url);
				
				throw;
			}
		}
		
		const Response r = get_response(request, raw_response);
		
		if (r.is_redirect()) {
			const std::string location = r.get_location();
			
			// Avoid redirect loops
			if (location == this_url) {
				return this_url;
			}
			
			// Strip tracking fields from the redirect URL
			this_url = clean_url(
				location,
				ignore_referral_marketing,
				ignore_rules,
				ignore_exceptions,
				ignore_raw_rules,
				ignore_redirections,
				skip_blocked
			);
			
			total_redirects++;
			
			if (total_redirects > max_redirects) {
				throw TooManyRedirectsError("Exceeded maximum allowed redirects");
			}
			
			continue;
		}
		
		break;
	}
	
	return this_url;
}

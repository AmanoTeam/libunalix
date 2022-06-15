/*
RFC 1035 - DOMAIN NAMES - IMPLEMENTATION AND SPECIFICATION
RFC 8484 - DNS Queries over HTTPS (DoH)
RFC 7858 - Specification for DNS over Transport Layer Security (TLS)
*/

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cstdlib>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "http_request.hpp"
#include "dns.hpp"
#include "uri.hpp"
#include "ssl.hpp"
#include "utils.hpp"
#include "exceptions.hpp"
#include "socket.hpp"

static constexpr int MIN_PACKET_SIZE = 20;
static constexpr int MAX_PACKET_SIZE = 512;

static constexpr int HTTPS_CONNECTION_PORT = 443;
static constexpr int TLS_CONNECTION_PORT = 853;
static constexpr int PLAIN_CONNECTION_PORT = 53;

static constexpr int IPV4_ADDRESS_SIZE = 0x04;
static constexpr int IPV6_ADDRESS_SIZE = 0x10;

static constexpr QType QTYPES[] = {
	A,
	AAAA
};

enum Specification {
	DNS_OVER_HTTPS,
	DNS_OVER_TLS,
	DNS_PLAIN_UDP,
	DNS_PLAIN_TCP,
};

const Specification get_specification(const std::string protocol) {
	if (protocol == "https") {
		return DNS_OVER_HTTPS;
	}
	
	if (protocol == "tls") {
		return DNS_OVER_TLS;
	}
	
	if (protocol == "udp") {
		return DNS_PLAIN_UDP;
	}
	
	if (protocol == "tcp") {
		return DNS_PLAIN_TCP;
	}
	
	throw UnsupportedProtocolError("Unrecognized URI or unsupported protocol");
}

const int get_socket_type(const Specification specification) {
	switch (specification) {
		case DNS_OVER_HTTPS:
		case DNS_OVER_TLS:
		case DNS_PLAIN_TCP:
			return SOCK_STREAM;
		case DNS_PLAIN_UDP:
			return SOCK_DGRAM;
	}
}

const int get_port(const Specification specification) {
	switch (specification) {
		case DNS_OVER_HTTPS:
			return HTTPS_CONNECTION_PORT;
		case DNS_OVER_TLS:
			return TLS_CONNECTION_PORT;
		case DNS_PLAIN_UDP:
		case DNS_PLAIN_TCP:
			return PLAIN_CONNECTION_PORT;
	}
}

const std::vector<char> encode_plain_query(
	const std::string name,
	const QType qtype
) {
	
	// Header section
	std::vector<char> buffer = {
		(char) 0xAA, (char) 0xAA, 0x01, 0x00,
		0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	
	std::string domain = name + ".";
	
	// Question section
	std::vector<char> data = {};
	
	for (const char ch : domain) {
		if (ch == '.') {
			buffer.push_back(data.size());
			buffer.insert(buffer.end(), data.begin(), data.end());
			
			data.clear();
		} else {
			data.push_back(ch);
		}
	}
	
	buffer.insert(buffer.end(), {0x00, 0x00, (char) qtype, 0x00, 0x01});
	
	const int buffer_size = buffer.size();
	
	if (buffer_size < MIN_PACKET_SIZE) {
		throw ValueError("DNS packet size too small");
	}
	
	if (buffer_size > MAX_PACKET_SIZE) {
		throw ValueError("DNS packet size too big");
	}
	
	return buffer;
}

const std::vector<char> encode_doh_query(
	const std::string domain,
	const QType qtype,
	const std::string server
) {
	std::vector<char> data = encode_plain_query(domain, qtype);
	
	HTTPRequest request = HTTPRequest(server);
	request.set_http_method(POST);
	request.set_http_version(HTTP10);
	
	request.add_header("Accept", "application/dns-udpwireformat");
	request.add_header("Accept-Encoding", "identity");
	request.add_header("Connection", "close");
	request.add_header("Content-Type", "application/dns-udpwireformat");
	request.add_header("Content-Length", std::to_string(data.size()));
	
	request.set_body(data);
	
	return request.get_request();
}

const std::vector<char> encode_tcp_query(
	const std::string name,
	const QType qtype
) {
	std::vector<char> buffer = encode_plain_query(name, qtype);
	const int buffer_size = buffer.size();
	
	buffer.insert(buffer.begin(), {0x00, (char) buffer_size});
	
	return buffer;
}

const std::vector<char> encode_query(
	const std::string name,
	const QType qtype,
	const Specification specification,
	const std::string server = NULL
) {
	switch (specification) {
		case DNS_OVER_HTTPS:
			return encode_doh_query(name, qtype, server);
		case DNS_PLAIN_TCP:
		case DNS_OVER_TLS:
			return encode_tcp_query(name, qtype);
		case DNS_PLAIN_UDP:
			return encode_plain_query(name, qtype);
	}
}

const DNSResponse get_address(const std::string domain, const int port) {
	
	struct addrinfo hints = {};
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	struct addrinfo* res;
	
	int rc = getaddrinfo(domain.c_str(), std::to_string(port).c_str(), &hints, &res);
	
	if (rc != 0) {
		throw GAIError(gai_strerror(rc));
	}
	
	char host[NI_MAXHOST];
	rc = getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
	
	if (rc != 0) {
		throw GAIError(gai_strerror(rc));
	}
	
	const QType qtype =  res->ai_addr->sa_family == AF_INET ? A : AAAA;
	const std::string address = std::string(host);
	
	const DNSResponse answer = DNSResponse(
		qtype,
		address,
		port
	);
	
	freeaddrinfo(res);
	
	return answer;
}

const DNSResponse dns_query(
	const std::string domain,
	const int port,
	const std::string dns_server,
	const int timeout
) {
	
	if (is_ipv4(domain)) {
		const DNSResponse answer = DNSResponse(
			A,
			domain,
			port
		);
		
		return answer;
	} else if (is_ipv6(domain)) {
		const DNSResponse answer = DNSResponse(
			AAAA,
			domain,
			port
		);
		
		return answer;
	} else if (dns_server.empty()) {
		// Fallback using the system DNS resolver
		return get_address(domain, port);
	}
	
	const URI uri = URI::from_string(dns_server);
	
	// Get current DNS implementation
	const Specification specification = get_specification(uri.get_scheme());
	
	const int server_port = uri.get_port() == 0 ? get_port(specification) : uri.get_port();
	
	DNSResponse server_address = DNSResponse();
	
	if (uri.is_ipv4()) {
		server_address.set_qtype(A);
		server_address.set_address(uri.get_host());
		server_address.set_port(server_port);
	} else if (uri.is_ipv6()) {
		server_address.set_qtype(AAAA);
		server_address.set_address(uri.get_ipv6_host());
		server_address.set_port(server_port);
	} else {
		const DNSResponse answer = get_address(
			uri.get_host(),
			server_port
		);
		
		server_address.set_qtype(answer.get_qtype());
		server_address.set_address(answer.get_address());
		server_address.set_port(answer.get_port());
	}
	
	const struct sockaddr* sock_addr = server_address.get_sockaddr();
	const int sock_addr_size = server_address.get_sockaddr_size();
	
	const int socket_type = get_socket_type(specification);
	const int addr_family = server_address.get_qtype() == A ? AF_INET : AF_INET6;
	
	for (const QType qtype : QTYPES) {
		const std::vector<char> query = encode_query(domain, qtype, specification, dns_server);
		
		const char* buffer = query.data();
		const int buffer_size = query.size();
		
		// Send DNS query
		char response[1024];
		size_t response_size;
		
		int fd = create_socket(addr_family, socket_type, IPPROTO_IP, timeout);
		
		if (socket_type == SOCK_STREAM) {
			connect_socket(fd, sock_addr, sock_addr_size);
			
			if (specification == DNS_OVER_HTTPS || specification == DNS_OVER_TLS) {
				response_size = send_encrypted_data(
					fd,
					uri.get_host().c_str(),
					buffer,
					buffer_size,
					response,
					sizeof(response)
				);
			} else {
				response_size = send_tcp_data(
					fd,
					buffer,
					buffer_size,
					response,
					sizeof(response)
				);
			}
		} else {
			response_size = send_udp_data(
				fd,
				buffer,
				buffer_size,
				response,
				sizeof(response),
				sock_addr,
				sock_addr_size
			);
		}
		
		close(fd);
		
		// Parse DNS response
		unsigned int rcode[2];
		
		for (int index = 0; index < response_size; index++) {
			if (response[index] == (char) 0xAA && response[index + 1] == (char) 0xAA) {
				rcode[0] = response[index + 2];
				rcode[1] = response[index + 3];
				break;
			}
		}
		
		if (!(rcode[0] == 0x81 && rcode[1] == 0x80)) {
			throw DNSError("This domain does not exists");
		}
		
		std::ostringstream address;
		
		switch (qtype) {
			case A:
				{
					const int rdlength = (int) response[response_size - 5];
					
					if (rdlength != IPV4_ADDRESS_SIZE) {
						continue;
					}
				}
				
				address << (int) response[response_size - 4] << '.' << (int) response[response_size - 3] << '.' << (int) response[response_size - 2] << '.' << (int) response[response_size - 1];
				
				break;
			case AAAA:
				{
					const int rdlength = (int) response[response_size - (IPV6_ADDRESS_SIZE + 1)];
					
					if (rdlength != IPV6_ADDRESS_SIZE) {
						continue;
					}
				}
				
				const int address_start = response_size - 16;
				
				for (int index = address_start; index < response_size; index += 2) {
					if (index != address_start) {
						address << ':';
					}
					
					address << std::hex << std::setw(2) << std::setfill('0') << (int) response[index];
					address << std::hex << std::setw(2) << std::setfill('0') << (int) response[index + 1];
				}
				
				break;
		}
		
		const DNSResponse answer = DNSResponse(
			qtype,
			address.str(),
			port
		);
		
		return answer;
	}
	
	throw DNSError("There are no addresses associated with this domain");
	
}

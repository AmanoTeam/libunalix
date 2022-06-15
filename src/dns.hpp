#include <string>
#include <vector>

#include <arpa/inet.h>

enum QType {
	A = 1,
	AAAA = 28
};

struct DNSResponse {
	private:
		QType qtype;
		std::string address;
		int port;
		struct sockaddr_in* addr_in = nullptr;
		struct sockaddr_in6* addr_in6 = nullptr;
		struct sockaddr* addr = nullptr;
		int addr_size = 0;
	
	public:
		DNSResponse() {
			return;
		}
		
		DNSResponse(
			const QType qtype,
			const std::string address,
			const int port
		) :
			qtype(qtype),
			address(address),
			port(port)
		{}
		
		const void set_qtype(const QType qtype) {
			this->qtype = qtype;
		}
		
		const QType get_qtype() const {
			return this->qtype;
		}
		
		const void set_address(const std::string address) {
			this->address = address;
		}
		
		const std::string get_address() const {
			return this->address;
		}
		
		const void set_port(const int port) {
			this->port = port;
		}
		
		const int get_port() const {
			return this->port;
		}
		
		struct sockaddr* get_sockaddr() {
			if (this->addr == nullptr) {
				switch (this->qtype) {
					case A:
						this->addr_in = new struct sockaddr_in;
						this->addr_in->sin_family = AF_INET;
						this->addr_in->sin_port = htons(this->port);
						inet_pton(this->addr_in->sin_family, this->address.c_str(), &this->addr_in->sin_addr);
						
						this->addr = (struct sockaddr*) this->addr_in;
						this->addr_size = sizeof(*this->addr_in);
						
						break;
					case AAAA:
						this->addr_in6 = new struct sockaddr_in6;
						this->addr_in6->sin6_family = AF_INET6;
						this->addr_in6->sin6_port = htons(this->port);
						inet_pton(this->addr_in6->sin6_family, this->get_address().c_str(), &this->addr_in6->sin6_addr);
						
						this->addr = (struct sockaddr*) this->addr_in6;
						this->addr_size = sizeof(*this->addr_in6);
						
						break;
				}
			}
			
			return this->addr;
		}
		
		const int get_sockaddr_size() const {
			return this->addr_size;
		}
};

const DNSResponse dns_query(
	const std::string domain,
	const int port,
	const std::string dns_server,
	const int timeout
);

#pragma once
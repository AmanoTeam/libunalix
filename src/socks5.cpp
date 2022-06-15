#include <string>
#include <vector>

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "socket.hpp"
#include "ssl.hpp"
#include "dns.hpp"
#include "uri.hpp"
#include "exceptions.hpp"

const std::string socks5_strerror(const int code) {
	
	std::string message;
	
	switch (code) {
		case 0x01:
			message = "General SOCKS server failure";
			break;
		case 0x02:
			message = "Connection not allowed";
			break;
		case 0x03:
			message = "Network unreachable";
			break;
		case 0x04:
			message = "Host unreachable";
			break;
		case 0x05:
			message = "Connection refused";
			break;
		case 0x06:
			message = "TTL expired";
			break;
		case 0x07:
			message = "Command not supported";
			break;
		case 0x08:
			message = "Address type not supported";
			break;
		case 0x09:
			message = "Unassigned";
			break;
		default:
			message = "Unknown SOCKS server failure";
			break;
	}
	
	return message;
}

const std::string send_proxied_tcp_data(
	const std::string data,
	const std::string hostname,
	const int port,
	const std::string server,
	const std::string username,
	const std::string password,
	const std::string dns,
	const int timeout
) {
	
	const URI uri = URI::from_string(server);
	
	DNSResponse dnsr = dns_query(
		hostname,
		(uri.get_port() == 0) ? 8081 : uri.get_port(),
		dns,
		timeout
	);
	
	const struct sockaddr* sock_addr = dnsr.get_sockaddr();
	const int sock_addr_size = dnsr.get_sockaddr_size();
	const int addr_family = dnsr.get_qtype() == A ? AF_INET : AF_INET6;
	
	int fd = create_socket(addr_family, SOCK_STREAM, IPPROTO_TCP, timeout);
	
	connect_socket(fd, sock_addr, sock_addr_size);
	
	const char hello[] = {0x05, 0x01, 0x00};
	char hello_response[2];
	
	send_tcp_data(
		fd,
		hello,
		sizeof(hello),
		hello_response,
		sizeof(hello_response)
	);
	
	const char socks_version = hello_response[0];
	
	if (socks_version != 0x05) {
		close(fd);
		throw Socks5Error("Unsupported SOCKS server version");
	}
	
	char reply_status = hello_response[1];
	
	/*
	 Authentication
	*/
	if (reply_status == 0x02) {
		std::vector<char> authentication = {0x01};
		
		// Username
		authentication.push_back((char) username.length());
		authentication.insert(authentication.end(), username.begin(), username.end());
		
		// Password
		authentication.push_back((char) password.length());
		authentication.insert(authentication.end(), password.begin(), password.end());
		
		char auth[2];
		send_tcp_data(
			fd,
			authentication.data(),
			authentication.size(),
			auth,
			sizeof(auth)
		);
		
		reply_status = auth[1];
	} else if (reply_status != 0x00) {
		close(fd);
		throw Socks5Error("Unsupported authentication method");
	}
	
	if (reply_status != 0x00) {
		close(fd);
		throw Socks5Error(socks5_strerror(reply_status));
	}
	
	/*
	  Ask SOCKS5 server to connect to the desired hostname
	*/
	std::vector<char> question = {0x05, 0x01, 0x00, 0x03};
	
	question.push_back((char) hostname.size());
	question.insert(question.end(), hostname.begin(), hostname.end());
	question.insert(question.end(), {(char) (port >> 8), (char) (port & 0xFF)});
	
	char answer[10];
	send_tcp_data(
		fd,
		question.data(),
		question.size(),
		answer,
		sizeof(answer)
	);
	
	reply_status = answer[1];
	
	if (reply_status != 0x00) {
		close(fd);
		throw Socks5Error(socks5_strerror(reply_status));
	}
	
	/*
	  Send HTTP request through SOCKS5 server
	*/
	char response[1024];
	
	if (port == 443) {
		send_encrypted_data(
			fd,
			hostname.c_str(),
			data.c_str(),
			data.length(),
			response,
			sizeof(response)
		);
	} else {
		send_tcp_data(
			fd,
			data.c_str(),
			data.length(),
			response,
			sizeof(response)
		);
	}
	
	close(fd);
	
	return std::string(response);
}

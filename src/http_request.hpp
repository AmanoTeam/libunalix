#include <string>
#include <vector>

#include "uri.hpp"

enum HTTPMethod {
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
};

enum HTTPVersion {
	HTTP10,
	HTTP11
};

struct HTTPRequest {
	private:
		URI uri;
		std::string url;
		HTTPVersion http_version;
		HTTPMethod http_method;
		std::vector<std::tuple<std::string, std::string>> headers;
		std::vector<char> body;
	
	public:
		HTTPRequest(const std::string url) :
			url(url)
		{}
		
		const void set_http_version(const HTTPVersion http_version);
		const HTTPVersion get_http_version() const;
		
		const void set_http_method(const HTTPMethod http_method);
		const HTTPMethod get_http_method() const;
		
		const void set_body(const std::vector<char> body);
		const std::vector<char> get_body() const;
		
		const void add_header(const std::string key, const std::string value);
		const std::string get_header(const std::string name) const;
		
		const std::vector<char> get_request();
		
		const URI get_uri() const;
};

#pragma once
#include <string>
#include <vector>

#include "uri.hpp"
#include "http_request.hpp"

static constexpr char CRLF[] = "\r\n";

const std::string to_string(const HTTPVersion http_version) {
	switch (http_version) {
		case HTTP10:
			return "1.0";
		case HTTP11:
			return "1.1";
	}
}

const std::string to_string(const HTTPMethod http_method) {
	switch (http_method) {
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		case DELETE:
			return "DELETE";
		case CONNECT:
			return "CONNECT";
		case OPTIONS:
			return "OPTIONS";
		case TRACE:
			return "TRACE";
	}
}

const void HTTPRequest::set_http_version(const HTTPVersion http_version) {
	this->http_version = http_version;
}

const HTTPVersion HTTPRequest::get_http_version() const {
	return this->http_version;
}

const void HTTPRequest::set_http_method(const HTTPMethod http_method) {
	this->http_method = http_method;
}

const HTTPMethod HTTPRequest::get_http_method() const {
	return this->http_method;
}

const void HTTPRequest::set_body(const std::vector<char> body) {
	this->body = body;
}

const std::vector<char> HTTPRequest::get_body() const {
	return this->body;
}

const void HTTPRequest::add_header(const std::string key, const std::string value) {
	this->headers.push_back(std::make_tuple(key, value));
}

const std::string HTTPRequest::get_header(const std::string name) const {
	for (const std::tuple<std::string, std::string> &header : this->headers) {
		if (std::get<0>(header) == name) {
			return std::get<1>(header);
		}
	}
	
	return "";
}

const std::vector<char> HTTPRequest::get_request() {
	this->uri = URI::from_string(this->url);
	
	int port = this->uri.get_port();
	
	if (!port) {
		port = (uri.get_scheme() == "http") ? 80 : 443;
	}
	
	this->uri.set_port(port);
	
	std::string hostname = this->uri.get_host();
	
	if ((this->uri.get_scheme() == "http" && this->uri.get_port() != 80) || (this->uri.get_scheme() == "https" && this->uri.get_port() != 443)) {
		hostname.append(":");
		hostname.append(std::to_string(this->uri.get_port()));
	}
	
	if (this->uri.get_path() == "") {
		this->uri.set_path("/");
	}
	
	// HTTP method
	std::string data = to_string(this->http_method);
	
	// URI path
	data.append(" ");
	data.append((this->uri.get_query() == "") ? this->uri.get_path() : this->uri.get_path() + this->uri.get_query());
	data.append(" ");
	
	// HTTP version
	data.append("HTTP/" + to_string(this->http_version));
	data.append(CRLF);
	
	// "Host" header
	data.append("Host: " + hostname);
	data.append(CRLF);
	
	// Append headers added by add_header()
	for (const std::tuple<std::string, std::string> &header : this->headers) {
		const std::string key = std::get<0>(header);
		const std::string value = std::get<1>(header);
		
		data.append(key);
		data.append(": ");
		data.append(value);
		data.append(CRLF);
	}
	
	data.append(CRLF);
	
	std::vector<char> request = std::vector<char>(data.begin(), data.end());
	
	if (!this->body.empty()) {
		request.insert(request.end(), this->body.begin(), this->body.end());
	}
	
	return request;
}

const URI HTTPRequest::get_uri() const {
	return this->uri;
}
#include <regex>
#include <string>
#include <algorithm>

#include "uri.hpp"
#include "utils.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "exceptions.hpp"

const std::string get_message(const int status_code) {
	switch (status_code) {
		case 100:
			return "Continue";
		case 101:
			return "Switching Protocols";
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 203:
			return "Non-Authoritative Information";
		case 204:
			return "Non-Authoritative Information";
		case 205:
			return "Reset Content";
		case 206:
			return "Partial Content";
		case 300:
			return "Multiple Choices";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		case 305:
			return "Use Proxy";
		case 307:
			return "Temporary Redirect";
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 402:
			return "Payment Required";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 406:
			return "Not Acceptable";
		case 407:
			return "Proxy Authentication Required";
		case 408:
			return "Request Time-out";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 411:
			return "Length Required";
		case 412:
			return "Precondition Failed";
		case 413:
			return "Request Entity Too Large";
		case 414:
			return "Request-URI Too Large";
		case 415:
			return "Unsupported Media Type";
		case 416:
			return "Requested range not satisfiable";
		case 417:
			return "Expectation Failed";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Time-out";
		case 505:
			return "HTTP Version not supported";
		default:
			throw RemoteProtocolError("Unknown status code: " + std::to_string(status_code));
	}
}

const Response get_response(const HTTPRequest request, const std::string data) {
	
	const size_t index = data.find("\r\n\r\n");
	const std::string raw_headers = data.substr(0, index);
	const std::string body = data.substr(index + 4, data.length());
	
	Response response = Response();
	
	response.set_body(body);
	
	const std::string::const_iterator headers_start = raw_headers.begin();
	const std::string::const_iterator headers_end = raw_headers.end();
	
	const std::string::const_iterator http_version_start = std::find(headers_start, headers_end, '/') + 1;
	const std::string::const_iterator http_version_end = std::find(http_version_start, headers_end, ' ');
	
	const float http_version = std::stof(std::string(http_version_start, http_version_end).c_str());
	
	if (!(http_version == 1.0f || http_version == 1.1f)) {
		throw RemoteProtocolError("Unsupported protocol version: " + std::to_string(http_version));
	}
	
	response.set_http_version(http_version);
	
	const std::string::const_iterator status_code_start = std::find(http_version_end, headers_end, ' ') + 1;
	const std::string::const_iterator status_code_end = std::find(status_code_start, headers_end, ' ');
	
	const int status_code = std::stoi(std::string(status_code_start, status_code_end));
	const std::string status_message = get_message(status_code);
	
	response.set_status_code(status_code);
	response.set_status_message(status_message);
	
	std::vector<std::tuple<std::string, std::string>> headers;
	std::string::const_iterator header_end = status_code_end;
	
	while (header_end != headers_end) {
		const std::string::const_iterator header_start = std::find(header_end, headers_end, '\r') + 2;
		header_end = std::find(header_start, headers_end, '\r');
		
		const std::string header = std::string(header_start, header_end);
		
		const std::string::const_iterator key_end = std::find(header_start, header_end, ':');
		
		const std::string key = std::string(header_start, key_end);
		const std::string::const_iterator value_start = key_end + 2;
		
		if (value_start > header_end) {
			break;
		}
		
		const std::string value = std::string(value_start, header_end);
		
		headers.push_back(std::make_tuple(key, value));
	}
	
	response.set_headers(headers);
	
	std::string location;
	
	if (response.get_status_code() >= 300 && response.get_status_code() <= 399 && response.has_header("Location")) {
		location = response.get_header("Location");
	} else if (response.get_status_code() >= 200 && response.get_status_code() <= 299 && response.has_header("Content-Location")) {
		location = response.get_header("Content-Location");
	}
	
	if (!location.empty()) {
		size_t pos = location.find(' ');
		
		while (pos != std::string::npos) {
			location.replace(pos, 1, "%20");
			pos = location.find(' ');
		}
		
		if (!(starts_with(location, "https://") || starts_with(location, "http://"))) {
			const URI uri = request.get_uri();
			
			if (starts_with(location, "//")) {
				location = (
					uri.get_scheme() +
					"://" +
					std::regex_replace(location, std::regex("^/*"), "")
				);
			} else if (starts_with(location, "/")) {
				location = (
					uri.get_scheme() +
					"://" +
					request.get_header("Host") +
					location
				);
			} else {
				location = (
					uri.get_scheme() +
					"://" +
					request.get_header("Host") +
					((uri.get_path() == "/") ? uri.get_path() : dirname(uri.get_path())) +
					location
				);
			}
		}
	}
	
	response.set_location(location);
	
	return response;
}
		
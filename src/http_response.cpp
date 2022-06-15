#include <string>
#include <vector>

#include "http_response.hpp"

void Response::set_http_version(const float value) {
	this->http_version = value;
}

const float Response::get_http_version() const {
	return this->http_version;
}

void Response::set_status_code(const int value) {
	this->status_code = value;
}

const int Response::get_status_code() const {
	return this->status_code;
}

void Response::set_status_message(const std::string value) {
	this->status_message = value;
}

const std::string Response::get_status_message() const {
	return this->status_message;
}

void Response::set_headers(const std::vector<std::tuple<std::string, std::string>> value) {
	this->headers = value;
}

const std::vector<std::tuple<std::string, std::string>> Response::get_headers() const {
	return this->headers;
}

const std::string Response::get_header(const std::string name) const {
	for (const std::tuple<std::string, std::string> &header : this->headers) {
		if (std::get<0>(header) == name) {
			return std::get<1>(header);
		}
	}
	
	return "";
}

const bool Response::has_header(const std::string name) const {
	for (const std::tuple<std::string, std::string> &header : this->headers) {
		if (std::get<0>(header) == name) {
			return true;
		}
	}
	
	return false;
}

const bool Response::is_redirect() const {
	return (this->get_status_code() >= 300 && this->get_status_code() <= 399 && this->get_header("Location") != "");
}

void Response::set_body(const std::string value) {
	this->body = value;
}

const std::string Response::get_body() const {
	return this->body;
}

void Response::set_location(const std::string value) {
	this->location = value;
}

const std::string Response::get_location() const {
	return this->location;
}

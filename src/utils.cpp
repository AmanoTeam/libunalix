#include <string>
#include <sstream>

#include <arpa/inet.h>
#include <string.h>

inline unsigned char to_hex(unsigned char ch){
	return ch + (ch > 9 ? ('A' - 10) : '0');
}

const std::string urlencode(const std::string &str) {
	
	std::ostringstream os;

	for (std::string::const_iterator ci = str.begin(); ci != str.end(); ci++) {
		if ((*ci >= 'a' && *ci <= 'z') || (*ci >= 'A' && *ci <= 'Z') || (*ci >= '0' && *ci <= '9') || strchr("-._~", *ci)) {
			os << *ci;
		} else if (*ci == ' '){
			os << "%20";
		} else {
			os << '%' << to_hex(*ci >> 4) << to_hex(*ci % 16);
		}
	}

	return os.str();
	
}

inline unsigned char from_hex(unsigned char ch){
	
	if (ch <= '9' && ch >= '0') {
		ch -= '0';
	} else if (ch <= 'f' && ch >= 'a') {
		ch -= 'a' - 10;
	} else if (ch <= 'F' && ch >= 'A') {
		ch -= 'A' - 10;
	} else {
		ch = 0;
	}
	
	return ch;
	
}

const std::string urldecode(const std::string &str){
	
	std::string result;
	
	for (std::string::size_type index = 0; index < str.size(); index++) {
		if (str[index] == '+') {
			result += ' ';
		} else if (str[index] == '%' && str.size() > index + 2) {
			const unsigned char ch1 = from_hex(str[index + 1]);
			const unsigned char ch2 = from_hex(str[index + 2]);
			const unsigned char ch = (ch1 << 4) | ch2;
			result += ch;
			index += 2;
		} else {
			result += str[index];
		}
	}
	
	return result;
	
}

const std::string urlencode(const unsigned char ch) {
	
	std::ostringstream os;

	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || strchr("-._~", ch)) {
		os << ch;
	} else if (ch == ' '){
		os << "%20";
	} else {
		os << '%' << to_hex(ch >> 4) << to_hex(ch % 16);
	}

	return os.str();
	
}

const std::string unquote_unreserved(const std::string &str){
	
	std::string result;
	
	for (std::string::size_type index = 0; index < str.size(); index++) {
		if (str[index] == '%' && str.size() > index + 2) {
			const unsigned char ch1 = from_hex(str[index + 1]);
			const unsigned char ch2 = from_hex(str[index + 2]);
			const unsigned char ch = (ch1 << 4) | ch2;
			
			if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || strchr("-._~", ch)){
				result += ch;
			} else {
				result += '%' + ch;
			}
			
			index += 2;
		} else {
			result += str[index];
		}
	}
	
	return result;
	
}

const std::string requote_uri(const std::string &str){
	
	std::string result;
	
	for (std::string::size_type index = 0; index < str.size(); index++) {
		const unsigned char ch = str[index];
		
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || strchr("!#$%&'()*+,/:;=?@[]~", ch)){
			result += ch;
		} else {
			result += urlencode(ch);
		}
	}
	
	return result;
}

const std::string strip_query(const std::string &query, const char query_start) {
	
	const char characters[] = {query_start, '&', '\0'};
	
	std::ostringstream os;

	for (std::string::size_type index = 0; index < query.size(); index++) {
		if ((strchr(characters, query[index]) && ((query.size() > index + 1 && strchr(characters, query[index + 1])) || index + 1 == query.size()))) {
			index++;
		} else {
			os << query[index];
		}
	}
	
	std::string result = os.str();
	
	if (result.size() > 0 && result[0] == '&') {
		result[0] = query_start;
	}
	
	return result;
	
}

const bool starts_with(const std::string source, const std::string prefix) {
	return (source.rfind(prefix, 0) == 0);
}

const std::string dirname(const std::string path) {
	
	const int position = path.rfind('/');
	
	if (position == 0) {
		return path;
	}
	
	return path.substr(0, position);
	
}

const bool is_ipv4(const std::string host) {
	struct sockaddr_in addr;
	const int rc = inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
	
	return (rc == 1);
}

const bool is_ipv6(const std::string host) {
	struct sockaddr_in6 addr;
	const int rc = inet_pton(AF_INET6, host.c_str(), &addr.sin6_addr);
	
	return (rc == 1);
}

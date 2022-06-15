#include <string>

const std::string urlencode(const std::string &str);
const std::string urldecode(const std::string &str);
const std::string requote_uri(const std::string &str);
const std::string strip_query(const std::string &query, const char query_start = '?');
const bool starts_with(const std::string source, const std::string prefix);
const std::string dirname(const std::string path);
const bool is_ipv4(const std::string host);
const bool is_ipv6(const std::string host);

#pragma once
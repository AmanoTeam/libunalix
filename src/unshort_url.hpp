#include <string>

const std::string unshort_url(
	const std::string url,
	const bool ignore_referral_marketing = false,
	const bool ignore_rules = false,
	const bool ignore_exceptions = false,
	const bool ignore_raw_rules = false,
	const bool ignore_redirections  = false,
	const bool skip_blocked = false,
	const int timeout = 5,
	const int max_redirects = 13,
	const std::string user_agent = "UnalixAndroid (+https://github.com/AmanoTeam/UnalixAndroid)",
	const std::string dns = "",
	const std::string proxy = "",
	const std::string proxy_username = "",
	const std::string proxy_password = ""
);

#pragma once
#include <iostream>
#include <vector>
#include <cctype>
#include <string>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
	#include <windows.h>
	#include <shellapi.h>
#else
	#include <cstdlib>
	
	const int exec_shell(const std::string cmd, const std::string arg) {
		std::string command = arg;
		
		const char old_value = '\'';
		const std::string new_value = "'\"'\"'";
		
		size_t pos = command.find(old_value);
		
		while (pos != std::string::npos) {
			command.replace(pos, 1, new_value);
			pos = command.find(old_value, pos + new_value.size());
		}
		
		command.insert(0, " '");
		command.insert(0, cmd);
		command.append("'");
		
		return system(command.c_str());
	}
#endif

#if defined(__unix__) || __APPLE__
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "cli_help.hpp"
#include "version.hpp"
#include "unalix.hpp"
#include "utils.hpp"

const bool is_atty(std::FILE* stream) {
	const int fd = fileno(stream);
	
	#if defined(__unix__) || __APPLE__
		return (isatty(fd) != 0);
	#else
		return (_isatty(fd) != 0);
	#endif
}

const void write_stderr(const std::string name, const std::string message) {
	std::cerr << name << ": " << message << std::endl;
}

const void abort_program(const std::string name, const std::string message) {
	write_stderr(name, message);
	exit(1);
}

const void show_help() {
	std::cout << cli_help;
	exit(0);
}

const void open_default_browser(const std::string url) {
	
	#if defined(_WIN32) || __APPLE__
		const std::string cmd = "open";
	#else
		const std::string cmd = "xdg-open";
	#endif
	
	#if defined(_WIN32)
		ShellExecuteA(0, cmd.c_str(), url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	#elif __APPLE__
		exec_shell(cmd, url);
	#else
		const int code = exec_shell(cmd, url);
		
		if (code == 0) {
			return;
		}
		
		char* value = std::getenv("BROWSER");
		
		if (value == NULL) {
			return;
		}
		
		char* browser = strtok(value, ":");
		
		while (browser != NULL) {
			exec_shell(std::string(browser), url);
			browser = strtok(NULL, ":");
		}
	#endif
	
}

const bool is_empty_or_whitespace(const std::string s) {
	
	for (const char character : s) {
		if (!(isspace(character))) {
			return false;
		}
	}
	
	return true;
	
}

int main(const int argc, const char* argv[]) {
	
	bool ignore_referral_marketing = false;
	bool ignore_rules = false;
	bool ignore_exceptions = false;
	bool ignore_raw_rules = false;
	bool ignore_redirections = false;
	bool skip_blocked = false;
	
	int timeout = 5;
	int max_redirects = 13;
	std::string user_agent = "UnalixAndroid (+https://github.com/AmanoTeam/libunalix)";
	std::string dns = "";
	std::string proxy = "";
	std::string proxy_username = "";
	std::string proxy_password = "";
	
	bool unshort = false;
	bool launch_in_browser = false;
	bool strict_errors = false;
	
	const std::string program_name = argv[0];
	
	std::vector<std::string> urls = {};
	
	for (int index = 1; index < argc; ++index) {
		const std::string argument = argv[index];
		
		const std::string::const_iterator value_s = std::find(argument.begin(), argument.end(), '=');
		
		const std::string key = std::string(argument.begin(), value_s);
		const std::string value = (value_s == argument.end()) ? "" : std::string(value_s + 1, argument.end());
		
		if (!(starts_with(argument, "--") || starts_with(argument, "-"))) {
			abort_program(program_name, "invalid argument: " + key);
		}
		
		if (key == "--ignore-referral-marketing") {
			ignore_referral_marketing = true;
		} else if (key == "--ignore-rules") {
			ignore_rules = true;
		} else if (key == "--ignore-exceptions") {
			ignore_exceptions = true;
		} else if (key == "--ignore-raw-rules") {
			ignore_raw_rules = true;
		} else if (key == "--ignore-redirections") {
			ignore_redirections = true;
		} else if (key == "--skip-blocked") {
			skip_blocked = true;
		} else if (key == "--unshort" || argument == "-s") {
			unshort = true;
		} else if (key == "--launch-in-browser") {
			launch_in_browser = true;
		} else if (key == "--strict-errors") {
			strict_errors = true;
		} else if (key == "--url" || key == "-u") {
			if (!(starts_with(value, "https://") || starts_with(value, "http://"))) {
				abort_program(program_name, "unrecognized URI or unsupported protocol: " + value);
			}
			
			urls.push_back(value);
		} else if (key == "--timeout") {
			const int integer = std::stoi(value);
			
			if (integer > 30) {
				abort_program(program_name, "timeout value too big");
			}
			
			if (integer < 1) {
				abort_program(program_name, "timeout value too low");
			}
			
			timeout = integer;
		} else if (key == "--max-redirs") {
			const int integer = std::stoi(value);
			
			if (integer > 20) {
				abort_program(program_name, "max_redirects value too big");
			}
			
			if (integer < 3) {
				abort_program(program_name, "max_redirects value too low");
			}
			
			max_redirects = integer;
		} else if (key == "--user-agent") {
			user_agent = (value == "none") ? "" : value;
		} else if (key == "--dns-server") {
			if (!(starts_with(value, "tcp://") || starts_with(value, "udp://") || starts_with(value, "https://") || starts_with(value, "tls://"))) {
				abort_program(program_name, "unrecognized URI or unsupported protocol: " + value);
			}
			
			dns = value;
		} else if (key == "--proxy") {
			if (!(starts_with(value, "socks5://"))) {
				abort_program(program_name, "unrecognized URI or unsupported protocol: " + value);
			}
			
			proxy = value;
		} else if (key == "--proxy-user") {
			proxy_username = value;
		} else if (key == "--proxy-pass") {
			proxy_password = value;
		} else if (key == "-h" || key == "--help") {
			show_help();
		} else if (key == "-v" || key == "--version") {
			std::cout << "Unalix v" << Unalix_VERSION_MAJOR << "." << Unalix_VERSION_MINOR << std::endl;;
			exit(0);
		} else {
			abort_program(program_name, "unrecognized argument: " + key);
		}
	}
	
	if (!is_atty(stdin)) {
		for (std::string line; getline(std::cin, line);) {
			if (!(starts_with(line, "https://") || starts_with(line, "http://"))) {
				abort_program(program_name, "unrecognized URI or unsupported protocol: " + line);
			}
			
			urls.push_back(line);
		}
	}
	
	if (urls.empty()) {
		show_help();
	}
	
	for (const std::string url : urls) {
		std::string result;
		
		if (unshort) {
			try {
				result = unshort_url(
					url,
					ignore_referral_marketing,
					ignore_rules,
					ignore_exceptions,
					ignore_raw_rules,
					ignore_redirections,
					skip_blocked,
					timeout,
					max_redirects,
					user_agent,
					dns,
					proxy,
					proxy_username,
					proxy_password
				);
			} catch (UnalixException &e) {
				if (strict_errors) {
					abort_program(program_name, "fatal error: " + e.get_message());
				} else {
					write_stderr(program_name, "error: " + e.get_message());
					result = e.get_url();
				}
			}
		} else {
			result = clean_url(
				url,
				ignore_referral_marketing,
				ignore_rules,
				ignore_exceptions,
				ignore_raw_rules,
				ignore_redirections,
				skip_blocked
			);
		}
		
		if (launch_in_browser) {
			open_default_browser(result);
		}
		
		std::cout << result << std::endl;
	}
	
}
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <cerrno>

#include "uri.hpp"
#include "rulesets.hpp"
#include "utils.hpp"
#include "exceptions.hpp"

#include <json/json.h>

static constexpr char KEY_URL_PATTERN[] = "urlPattern";
static constexpr char KEY_COMPLETE_PROVIDER[] = "completeProvider";
static constexpr char KEY_RULES[] = "rules";
static constexpr char KEY_RAW_RULES[] = "rawRules";
static constexpr char KEY_REFERRAL_MARKETING[] = "referralMarketing";
static constexpr char KEY_EXCEPTIONS[] = "exceptions";
static constexpr char KEY_REDIRECTIONS[] = "redirections";

static constexpr char REGEX_PATTERN_PREFIX[] = "(%(?:26|23|3[Ff])|&|\\?)";
static constexpr char REGEX_PATTERN_SUFFIX[] = "(?:(?:=|%3[Dd])[^&]*)";

static constexpr char REGEX_PATTERN_REPLACEMENT[] = "$1";

std::vector<Ruleset> rulesets = {};

const void load_ruleset(const std::string filename) {
	
	std::ifstream stream = std::ifstream(filename, std::ifstream::binary);
	
	if (!stream) {
		throw RulesetParserError("Cannot open file for reading: " + std::string(strerror(errno)));
	}
	
	Json::Value tree = Json::Value();
	Json::CharReaderBuilder builder = Json::CharReaderBuilder();
	
	builder["allowComments"] = false;
	builder["failIfExtra"] = true;
	builder["strictRoot"] = true;
	builder["rejectDupKeys"] = true;
	
	JSONCPP_STRING errs;
	
	if (!parseFromStream(builder, stream, &tree, &errs)) {
		stream.close();
		
		throw RulesetParserError("Cannot parse JSON tree: " + errs);
	}
	
	stream.close();
	
	const Json::Value root = tree["providers"];
	const Json::Value::Members providers = root.getMemberNames();
	
	for (const Json::Value provider : providers) {
		const std::string name = provider.asString();
		
		const Json::Value item = root[name];
		
		if (!item.isMember(KEY_URL_PATTERN)) {
			throw RulesetParserError("Missing required attribute in JSON tree: " + std::string(KEY_URL_PATTERN));
		}
		
		const std::regex url_pattern = std::regex(item[KEY_URL_PATTERN].asString());
		
		bool complete_provider = false;
		
		if (item.isMember(KEY_COMPLETE_PROVIDER)) {
			complete_provider = item[KEY_COMPLETE_PROVIDER].asBool();
		}
		
		std::vector<std::regex> rules = {};
		
		if (item.isMember(KEY_RULES)) {
			const Json::Value values = item[KEY_RULES];
			
			for (const Json::Value &value : values) {
				rules.push_back(std::regex(REGEX_PATTERN_PREFIX + value.asString() + REGEX_PATTERN_SUFFIX));
			}
		}
		
		std::vector<std::regex> raw_rules = {};
		
		if (item.isMember(KEY_RAW_RULES)) {
			const Json::Value values = item[KEY_RAW_RULES];
			
			for (const Json::Value &value : values) {
				raw_rules.push_back(std::regex(value.asString()));
			}
		}
		
		std::vector<std::regex> referral_marketing = {};
		
		if (item.isMember(KEY_REFERRAL_MARKETING)) {
			const Json::Value values = item[KEY_REFERRAL_MARKETING];
			
			for (const Json::Value &value : values) {
				referral_marketing.push_back(std::regex(REGEX_PATTERN_PREFIX + value.asString() + REGEX_PATTERN_SUFFIX));
			}
		}
		
		std::vector<std::regex> exceptions = {};
		
		if (item.isMember(KEY_EXCEPTIONS)) {
			const Json::Value values = item[KEY_EXCEPTIONS];
			
			for (const Json::Value &value : values) {
				exceptions.push_back(std::regex(value.asString()));
			}
		}
		
		std::vector<std::regex> redirections = {};
		
		if (item.isMember(KEY_REDIRECTIONS)) {
			const Json::Value values = item[KEY_REDIRECTIONS];
			
			for (const Json::Value &value : values) {
				redirections.push_back(std::regex(value.asString()));
			}
		}
		
		const Ruleset ruleset = Ruleset(
			url_pattern,
			complete_provider,
			rules,
			raw_rules,
			referral_marketing,
			exceptions,
			redirections
		);
		
		rulesets.push_back(ruleset);
	}
}

const std::string clean_url(
	const std::string url,
	const bool ignore_referral_marketing,
	const bool ignore_rules,
	const bool ignore_exceptions,
	const bool ignore_raw_rules,
	const bool ignore_redirections,
	const bool skip_blocked
) {
	
	std::string this_url = url;
	
	for (const Ruleset &ruleset: rulesets) {
		if (skip_blocked && ruleset.get_complete_provider()) {
			continue;
		}
		
		std::smatch groups;
		
		if (std::regex_search(this_url, groups, ruleset.get_url_pattern())) {
			if (!ignore_exceptions) {
				bool exception_matched = false;
				
				for (const std::regex &exception : ruleset.get_exceptions()) {
					std::smatch groups;
					
					if (std::regex_search(this_url, groups, exception)) {
						exception_matched = true;
						break;
					}
				}
				
				if (exception_matched) {
					continue;
				}
			}
			
			if (!ignore_redirections) {
				for (const std::regex &redirection : ruleset.get_redirections()) {
					std::smatch groups;
					
					if (std::regex_search(this_url, groups, redirection)) {
						const std::string target_url = groups[1];
						const std::string redirection_result = requote_uri(urldecode(target_url));
						
						// Avoid empty URLs
						if (redirection_result.empty()) {
							continue;
						}
						
						// Avoid duplicate URLs
						if (redirection_result == this_url) {
							continue;
						}
						
						URI uri = URI::from_string(redirection_result);
						
						// Workaround for URLs without scheme (see https://github.com/ClearURLs/Addon/issues/71)
						if (uri.get_scheme().empty()) {
							uri.set_scheme("http");
						}
						
						return clean_url(
							uri.to_string(),
							ignore_referral_marketing,
							ignore_rules,
							ignore_exceptions,
							ignore_raw_rules,
							ignore_redirections,
							skip_blocked
						);
						
					}
				}
			}

			URI uri = URI::from_string(this_url);

			if (!uri.get_query().empty()) {
				if (!ignore_rules) {
					for (const std::regex &rule : ruleset.get_rules()) {
						const std::string query = std::regex_replace(uri.get_query(), rule, REGEX_PATTERN_REPLACEMENT);
						uri.set_query(query);
					}
				}
				
				if (!ignore_referral_marketing) {
					for (const std::regex &referral_marketing : ruleset.get_referral_marketing()) {
						const std::string query = std::regex_replace(uri.get_query(), referral_marketing, REGEX_PATTERN_REPLACEMENT);
						uri.set_query(query);
					}
				}
			}
			
			// The fragment might contains tracking fields as well
			if (!uri.get_fragment().empty()) {
				if (!ignore_rules) {
					for (const std::regex &rule : ruleset.get_rules()) {
						const std::string fragment = std::regex_replace(uri.get_fragment(), rule, REGEX_PATTERN_REPLACEMENT);
						uri.set_fragment(fragment);
					}
				}
				
				if (!ignore_referral_marketing) {
					for (const std::regex &referral_marketing : ruleset.get_referral_marketing()) {
						const std::string fragment = std::regex_replace(uri.get_fragment(), referral_marketing, REGEX_PATTERN_REPLACEMENT);
						uri.set_fragment(fragment);
					}
				}
			}
			
			if (!uri.get_path().empty()) {
				if (!ignore_raw_rules) {
					for (const std::regex &raw_rule : ruleset.get_raw_rules()) {
						const std::string path = std::regex_replace(uri.get_path(), raw_rule, REGEX_PATTERN_REPLACEMENT);
						uri.set_path(path);
					}
				}
			}
			
			if (!uri.get_query().empty()) {
				uri.set_query(strip_query(uri.get_query()));
			}
			
			if (!uri.get_fragment().empty()) {
				uri.set_fragment(strip_query(uri.get_fragment(), '#'));
			}
			
			this_url = uri.to_string();
		}
	}
	
	return this_url;
	
}

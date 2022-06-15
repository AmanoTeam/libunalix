#include <string>
#include <vector>

struct Ruleset {
	private:
		const std::regex urlPattern;
		const bool completeProvider;
		const std::vector<std::regex> rules;
		const std::vector<std::regex> rawRules;
		const std::vector<std::regex> referralMarketing;
		const std::vector<std::regex> exceptions;
		const std::vector<std::regex> redirections;
	
	public:
		Ruleset(
			const std::regex urlPattern,
			const bool completeProvider,
			const std::vector<std::regex> rules,
			const std::vector<std::regex> rawRules,
			const std::vector<std::regex> referralMarketing,
			const std::vector<std::regex> exceptions,
			const std::vector<std::regex> redirections
		) :
			urlPattern(urlPattern),
			completeProvider(completeProvider),
			rules(rules),
			rawRules(rawRules),
			referralMarketing(referralMarketing),
			exceptions(exceptions),
			redirections(redirections)
		{}
		
		const std::regex get_url_pattern() const {
			return this -> urlPattern;
		}
		
		const bool get_complete_provider() const {
			return this -> completeProvider;
		}
		
		const std::vector<std::regex> get_rules() const {
			return this -> rules;
		}
		
		const std::vector<std::regex> get_raw_rules() const {
			return this -> rawRules;
		}
		
		const std::vector<std::regex> get_referral_marketing() const {
			return this -> referralMarketing;
		}
		
		const std::vector<std::regex> get_exceptions() const {
			return this -> exceptions;
		}
		
		const std::vector<std::regex> get_redirections() const {
			return this -> redirections;
		}
};

#pragma once
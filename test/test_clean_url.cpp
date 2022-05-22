#include <cassert>
#include <string>

#include "clean_url.hpp"

int main() {
	
	const bool ignore_referral_marketing = false;
	const bool ignore_rules = false;
	const bool ignore_exceptions = false;
	const bool ignore_raw_rules = false;
	const bool ignore_redirections = false;
	const bool skip_blocked = false;
	
	{
		const std::string unmodified_url = "https://deezer.com/track/891177062?utm_source=deezer";
		
		std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == "https://deezer.com/track/891177062");
		
		result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			true,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
	}
	
	{
		const std::string unmodified_url = "https://www.google.com/url?q=https://pypi.org/project/Unalix";
		
		std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == "https://pypi.org/project/Unalix");
		
		result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			true,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
	}
	
	// https://github.com/ClearURLs/Addon/issues/71)
	{
		const std::string unmodified_url = "https://www.google.com/amp/s/de.statista.com/infografik/amp/22496/anzahl-der-gesamten-positiven-corona-tests-und-positivenrate/";
		
		const std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == "http://de.statista.com/infografik/amp/22496/anzahl-der-gesamten-positiven-corona-tests-und-positivenrate/");
	}
	
	{
		const std::string unmodified_url = "https://www.amazon.com/gp/B08CH7RHDP/ref=as_li_ss_tl";
		
		std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == "https://www.amazon.com/gp/B08CH7RHDP");
		
		result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			true,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
	}
	
	{
		const std::string unmodified_url = "https://myaccount.google.com/?utm_source=google";
		
		std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
		
		result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			true,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == "https://myaccount.google.com/");
	}
	
	// URLs with duplicate parameters
	{
		const std::string unmodified_url = "http://example.com/?p1=value&p1=othervalue";
		
		const std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
	}
	
	// URLs with empty parameters
	{
		const std::string unmodified_url = "http://example.com/?&&&&";
		
		const std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == "http://example.com/");
	}
	
	// URLs whose parameters values are empty
	{
		const std::string unmodified_url = "http://example.com/?p1=&p2=";
		
		const std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
	}
	
	// https://github.com/AmanoTeam/Unalix-nim/issues/5
	{
		const std::string unmodified_url = "https://docs.julialang.org/en/v1/stdlib/REPL/#Key-bindings";
		
		const std::string result_url = clean_url(
			unmodified_url,
			ignore_referral_marketing,
			ignore_rules,
			ignore_exceptions,
			ignore_raw_rules,
			ignore_redirections,
			skip_blocked
		);
		
		assert (result_url == unmodified_url);
	}
	
}
	
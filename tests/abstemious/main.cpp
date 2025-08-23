#include <lexertl/enums.hpp>
#include <lexertl/generator.hpp>
#include <lexertl/iterator.hpp>
#include <lexertl/rules.hpp>
#include <lexertl/state_machine.hpp>

#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

struct test
{
	const char* regex;
	const char* text;
	std::vector<std::pair< std::size_t, std::size_t>> matches;
};

int main()
{
	// Tests from boost::regex (test\regress\test_non_greedy_repeats.cpp)
	test tests[] =
	{
		{"a*?", "aa", {{0, 0}, {1, 1}}},
		{"^a*?$", "aa", {{0, 2}}},
		{"^.*?$", "aa", {{0, 2}}},
		{"^(a)*?$", "aa", {{0, 2}}},
		{"^[ab]*?$", "aa", {{0, 2}}},
		{"a??", "aa", {{0, 0}, {1, 1}}},
		{"a+?", "aa", {{0, 1}, {1, 2}}},
		{"a{1,3}?", "aaa", {{0, 1}, {1, 2}, {2, 3}}},
		{"\\w+?w", "...ccccccwcccccw", {{3, 10}, {10, 16}}},
		{"\\W+\\w+?w", "...ccccccwcccccw", {{0, 10}}},
		// This generates the wrong state machine
		{"abc|\\w+?", "abd", {{0, 1}, {1, 2}, {2, 3}}},
		{"abc|\\w+?", "abcd", {{0, 3}, {3, 4}}},
		{"<\\s*tag[^>]*>(.*?)<\\s*[/]tag\\s*>",
			" <tag>here is some text</tag> <tag></tag>", {{1, 29}, {30, 41}}},
		{"<\\s*tag[^>]*>(.*?)<\\s*[/]tag\\s*>",
			" < tag attr=\"something\">"
			"here is some text< /tag > <tag></tag>", {{1, 49}, {50, 61}}},
		{"xx-{0,2}?(?:[-+][0-9])??$", "xx--", {{0, 4}}},
		{"xx.{0,2}?(?:[-+][0-9])??$", "xx--", {{0, 4}}},
		{"xx.{0,2}?(?:[-+][0-9])??$", "xx--", {{0, 4}}},
		{"xx[-/]{0,2}?(?:[-+][0-9])??$", "xx--", {{0, 4}}}
	};
	bool failures = false;

	for (const auto& t : tests)
	{
		auto matches_iter = t.matches.begin();
		auto matches_end = t.matches.end();
		lexertl::rules rules(*lexertl::regex_flags::dot_not_cr_lf |
			*lexertl::regex_flags::match_zero_len);
		lexertl::state_machine sm;

		rules.push(t.regex, 1);

		if (rules.regexes()[0][0].size() != 2)
			rules.push("(?s:.)", lexertl::rules::skip());

		lexertl::generator::build(rules, sm);

		lexertl::citerator iter(t.text, t.text + strlen(t.text), sm);

		for (; matches_iter != matches_end && iter->id; ++matches_iter)
		{
			if (matches_iter == matches_end ||
				iter->first - t.text != matches_iter->first ||
				iter->second - t.text != matches_iter->second)
			{
				failures = true;
				std::cout << t.regex << " failed to match " << t.text << "\n\n";
			}

			// If zero length match, then bump
			if (iter->first == iter->second)
				iter = lexertl::citerator(iter->second + 1, iter->eoi, sm);
			else
				++iter;
		}

		if (matches_iter != matches_end || iter->id)
		{
			failures = true;
			std::cout << t.regex << " failed to match " << t.text << "\n\n";
		}
	}

	if (failures)
		return 1;
	else
	{
		std::cout << "All abstemious tests passed\n";
		return 0;
	}
}

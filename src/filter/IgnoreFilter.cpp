#include "IgnoreFilter.hpp"
#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace
{
void AssertIsStreamOpen(const std::ifstream& stream)
{
	if (!stream.is_open())
	{
		throw std::runtime_error("Не удалось открыть файл .gitignore для чтения");
	}
}

std::string TrimWhitespace(std::string text)
{
	text.erase(text.begin(), std::ranges::find_if(text, [](const unsigned char ch) {
		return !std::isspace(ch);
	}));

	text.erase(std::find_if(text.rbegin(), text.rend(), [](const unsigned char ch) {
		return !std::isspace(ch);
	}).base(),
		text.end());

	return text;
}

bool IsCommentOrEmpty(const std::string& line)
{
	return line.empty() || line.starts_with('#');
}

std::vector<std::string> ReadRulesFromFile(const std::filesystem::path& path)
{
	std::vector<std::string> rules;
	std::ifstream file(path);

	AssertIsStreamOpen(file);

	std::string line;
	while (std::getline(file, line))
	{
		const auto cleanedLine = TrimWhitespace(line);
		if (!IsCommentOrEmpty(cleanedLine))
		{
			rules.push_back(cleanedLine);
		}
	}

	return rules;
}

bool MatchPattern(const std::string& pattern, const std::string& text)
{
	if (pattern.starts_with('*'))
	{
		const std::string suffix = pattern.substr(1);
		return text.ends_with(suffix);
	}

	if (pattern.ends_with('*'))
	{
		const std::string prefix = pattern.substr(0, pattern.length() - 1);
		return text.starts_with(prefix);
	}

	if (pattern.ends_with('/'))
	{
		const std::string dir = pattern.substr(0, pattern.length() - 1);
		return text == dir || text.ends_with("/" + dir) || text.starts_with(dir + "/")
			|| text.find("/" + dir + "/") != std::string::npos;
	}

	return text == pattern || text.ends_with("/" + pattern) || text.find("/" + pattern + "/") != std::string::npos;
}
} // namespace

IgnoreFilter::IgnoreFilter()
{
	AddRule(".git/");
}

IgnoreFilter::IgnoreFilter(const std::filesystem::path& gitignorePath)
{
	AddRule(".git/");

	if (std::filesystem::exists(gitignorePath) && std::filesystem::is_regular_file(gitignorePath))
	{
		const std::vector<std::string> fileRules = ReadRulesFromFile(gitignorePath);
		for (const auto& rule : fileRules)
		{
			AddRule(rule);
		}
	}
}

IgnoreFilter::~IgnoreFilter() = default;

void IgnoreFilter::AddRule(const std::string& rule)
{
	const std::string cleanedRule = TrimWhitespace(rule);

	if (!IsCommentOrEmpty(cleanedRule))
	{
		m_rules.push_back(cleanedRule);
	}
}

bool IgnoreFilter::ShouldIgnore(const std::filesystem::path& path) const
{
	const std::string pathString = path.generic_string();

	for (const auto& rule : m_rules)
	{
		if (MatchPattern(rule, pathString))
		{
			return true;
		}
	}

	return false;
}
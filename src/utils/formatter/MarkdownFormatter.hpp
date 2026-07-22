#pragma once

#include <string>

class MarkdownFormatter
{
public:
	MarkdownFormatter();
	~MarkdownFormatter();

	void AddHeader(int level, const std::string& text);
	void AddCodeBlock(const std::string& language, const std::string& code);
	void AddText(const std::string& text);
	void AddTree(const std::string& treeContent);

	[[nodiscard]] std::string Build() const;

private:
	std::string m_buffer;
};
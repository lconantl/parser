#pragma once

#include "app/IAppMode.hpp"
#include "filter/IgnoreFilter.hpp"
#include <filesystem>

class ParseMode final : public IAppMode
{
public:
	ParseMode(std::filesystem::path rootPath, const IgnoreFilter& filter);
	~ParseMode() override;

	void Execute(MarkdownFormatter& formatter) override;

private:
	std::filesystem::path m_rootPath;
	IgnoreFilter m_filter;
};
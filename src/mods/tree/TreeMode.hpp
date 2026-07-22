#pragma once

#include "app/IAppMode.hpp"
#include "filter/IgnoreFilter.hpp"
#include <filesystem>

class TreeMode final : public IAppMode
{
public:
	TreeMode(std::filesystem::path rootPath, const IgnoreFilter& filter);
	~TreeMode() override;

	void Execute(MarkdownFormatter& formatter) override;

private:
	std::filesystem::path m_rootPath;
	IgnoreFilter m_filter;
};
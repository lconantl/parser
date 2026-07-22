#pragma once

#include "app/IAppMode.hpp"
#include "filter/IgnoreFilter.hpp"
#include "mods/parse/ParseMode.hpp"
#include "mods/tree/TreeMode.hpp"
#include <filesystem>
#include <memory>

class DefaultMode final : public IAppMode
{
public:
	DefaultMode(const std::filesystem::path& rootPath, const IgnoreFilter& filter);
	~DefaultMode() override;

	void Execute(MarkdownFormatter& formatter) override;

private:
	std::unique_ptr<TreeMode> m_treeMode;
	std::unique_ptr<ParseMode> m_parseMode;
};
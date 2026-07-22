#include "DefaultMode.hpp"
#include <stdexcept>

namespace
{
void AssertModesInitialized(const TreeMode* treeMode, const ParseMode* parseMode)
{
	if (treeMode == nullptr || parseMode == nullptr)
	{
		throw std::runtime_error("Внутренняя ошибка: компоненты режима по умолчанию не инициализированы");
	}
}
} // namespace

DefaultMode::DefaultMode(const std::filesystem::path& rootPath, const IgnoreFilter& filter)
	: m_treeMode(std::make_unique<TreeMode>(rootPath, filter))
	, m_parseMode(std::make_unique<ParseMode>(rootPath, filter))
{
}

DefaultMode::~DefaultMode() = default;

void DefaultMode::Execute(MarkdownFormatter& formatter)
{
	AssertModesInitialized(m_treeMode.get(), m_parseMode.get());

	m_treeMode->Execute(formatter);
	m_parseMode->Execute(formatter);
}
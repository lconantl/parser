#include "TreeMode.hpp"
#include <stdexcept>

namespace
{
void AssertIsExistingDirectory(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		throw std::runtime_error("Указанный путь не существует или не является директорией");
	}
}

void AppendTreeContent(
	const std::filesystem::path& currentPath,
	const IgnoreFilter& filter,
	const std::string& prefix,
	std::string& buffer)
{
	for (const auto& entry : std::filesystem::directory_iterator(currentPath))
	{
		if (filter.ShouldIgnore(entry.path()))
		{
			continue;
		}

		buffer += prefix + "|-- " + entry.path().filename().string() + "\n";

		if (entry.is_directory())
		{
			AppendTreeContent(entry.path(), filter, prefix + "    ", buffer);
		}
	}
}
} // namespace

TreeMode::TreeMode(std::filesystem::path rootPath, IgnoreFilter filter)
	: m_rootPath(std::move(rootPath))
	, m_filter(std::move(filter))
{
	AssertIsExistingDirectory(m_rootPath);
}

TreeMode::~TreeMode() = default;

void TreeMode::Execute(MarkdownFormatter& formatter)
{
	std::string treeBuffer;
	treeBuffer += m_rootPath.filename().string() + "\n";
	AppendTreeContent(m_rootPath, m_filter, "", treeBuffer);

	formatter.AddHeader(2, "Структура проекта");
	formatter.AddTree(treeBuffer);
}
#include "TreeMode.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

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
	std::vector<std::filesystem::directory_entry> files;
	std::vector<std::filesystem::directory_entry> dirs;

	for (const auto& entry : std::filesystem::directory_iterator(currentPath, std::filesystem::directory_options::skip_permission_denied))
	{
		if (filter.ShouldIgnore(entry.path()))
		{
			continue;
		}

		if (entry.is_regular_file())
		{
			files.push_back(entry);
		}
		else if (entry.is_directory())
		{
			dirs.push_back(entry);
		}
	}

	auto sortByName = [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
		return a.path().filename().string() < b.path().filename().string();
	};
	std::ranges::sort(files, sortByName);
	std::ranges::sort(dirs, sortByName);

	for (const auto& file : files)
	{
		buffer += prefix + "|-- " + file.path().filename().string() + "\n";
	}

	for (const auto& dir : dirs)
	{
		buffer += prefix + "|-- " + dir.path().filename().string() + "\n";
		AppendTreeContent(dir.path(), filter, prefix + "    ", buffer);
	}
}
} // namespace

TreeMode::TreeMode(std::filesystem::path rootPath, const IgnoreFilter& filter)
	: m_rootPath(std::move(rootPath))
	, m_filter(filter)
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
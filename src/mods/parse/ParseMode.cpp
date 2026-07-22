#include "ParseMode.hpp"
#include "reader/Reader.hpp"
#include <stdexcept>

namespace
{
void AssertIsExistingDirectory(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		throw std::runtime_error("Указанный путь не существует или не является директорией для парсинга");
	}
}

void ProcessFile(const std::filesystem::path& filePath, MarkdownFormatter& formatter)
{
	formatter.AddHeader(3, filePath.filename().string());
	const std::string content = Reader::Read(filePath);
	formatter.AddCodeBlock("", content);
}

void TraverseAndProcess(
	const std::filesystem::path& root,
	const IgnoreFilter& filter,
	MarkdownFormatter& formatter)
{
	auto it = std::filesystem::recursive_directory_iterator(
		root, std::filesystem::directory_options::skip_permission_denied);
	auto end = std::filesystem::recursive_directory_iterator();

	for (; it != end; ++it)
	{
		if (filter.ShouldIgnore(it->path()))
		{
			if (it->is_directory())
			{
				it.disable_recursion_pending();
			}
			continue;
		}

		if (it->is_regular_file())
		{
			ProcessFile(it->path(), formatter);
		}
	}
}
} // namespace

ParseMode::ParseMode(std::filesystem::path rootPath, IgnoreFilter filter)
	: m_rootPath(std::move(rootPath))
	, m_filter(std::move(filter))
{
	AssertIsExistingDirectory(m_rootPath);
}

ParseMode::~ParseMode() = default;

void ParseMode::Execute(MarkdownFormatter& formatter)
{
	TraverseAndProcess(m_rootPath, m_filter, formatter);
}
#include "ParseMode.hpp"
#include "reader/Reader.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
void AssertIsExistingDirectory(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		throw std::runtime_error("Указанный путь не существует или не является директорией для парсинга");
	}
}

void ProcessDirectory(
	const std::filesystem::path& dir,
	const IgnoreFilter& filter,
	MarkdownFormatter& formatter,
	size_t& outFilesCount,
	size_t& outLinesCount)
{
	std::vector<std::filesystem::directory_entry> files;
	std::vector<std::filesystem::directory_entry> dirs;

	for (const auto& entry : std::filesystem::directory_iterator(dir, std::filesystem::directory_options::skip_permission_denied))
	{
		if (filter.ShouldIgnore(entry.path())) continue;

		if (entry.is_regular_file())
			files.push_back(entry);
		else if (entry.is_directory())
			dirs.push_back(entry);
	}

	auto sortByName = [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
		return a.path().filename().string() < b.path().filename().string();
	};
	std::ranges::sort(files, sortByName);
	std::ranges::sort(dirs, sortByName);

	for (const auto& file : files)
	{
		formatter.AddHeader(3, file.path().filename().string());
		const std::string content = Reader::Read(file.path());
		formatter.AddCodeBlock("", content);

		outFilesCount++;

		const size_t newLines = std::ranges::count(content, '\n');
		outLinesCount += newLines;

		if (!content.empty() && content.back() != '\n')
		{
			outLinesCount++;
		}
	}

	for (const auto& subDir : dirs)
	{
		ProcessDirectory(subDir.path(), filter, formatter, outFilesCount, outLinesCount);
	}
}
} // namespace

ParseMode::ParseMode(std::filesystem::path rootPath, const IgnoreFilter& filter)
	: m_rootPath(std::move(rootPath))
	, m_filter(filter)
{
	AssertIsExistingDirectory(m_rootPath);
}

ParseMode::~ParseMode() = default;

void ParseMode::Execute(MarkdownFormatter& formatter)
{
	size_t filesCount = 0;
	size_t linesCount = 0;

	ProcessDirectory(m_rootPath, m_filter, formatter, filesCount, linesCount);

	std::cout << "Статистика проекта" << std::endl;
	std::cout << "Всего файлов: " + std::to_string(filesCount) << std::endl;
	std::cout << "Всего строк кода: " + std::to_string(linesCount) << std::endl;
}
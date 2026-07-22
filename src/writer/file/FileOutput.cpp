#include "FileOutput.hpp"
#include <fstream>
#include <stdexcept>

namespace
{
void AssertIsDirectoryValid(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		throw std::runtime_error("Корневая директория для сохранения файла не существует");
	}
}

void AssertFileStreamOpen(const std::ofstream& stream)
{
	if (!stream.is_open())
	{
		throw std::runtime_error("Не удалось открыть файл dump.md для записи");
	}
}
} // namespace

FileOutput::FileOutput(std::filesystem::path rootPath)
	: m_outputPath(std::move(rootPath))
{
	AssertIsDirectoryValid(m_outputPath);
	m_outputPath /= "dump.md";
}

FileOutput::~FileOutput() = default;

void FileOutput::Write(const std::string& content)
{
	std::ofstream file(m_outputPath, std::ios::out | std::ios::binary | std::ios::trunc);
	AssertFileStreamOpen(file);

	file.write(content.c_str(), static_cast<std::streamsize>(content.size()));
}
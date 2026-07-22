#include "Reader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace
{
constexpr std::uintmax_t MAX_FILE_SIZE = 5 * 1024 * 1024; // 5 МБ
constexpr std::streamsize BINARY_CHECK_SIZE = 512;

void AssertIsFileExists(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		throw std::runtime_error("File does not exist: " + path.string());
	}
}

void AssertIsRegularFile(const std::filesystem::path& path)
{
	if (!std::filesystem::is_regular_file(path))
	{
		throw std::runtime_error("Path is not a file: " + path.string());
	}
}

void AssertIsFileOpen(const std::ifstream& stream)
{
	if (!stream.is_open())
	{
		throw std::runtime_error("Couldn't open the file for reading");
	}
}

std::string ReadStreamContent(const std::ifstream& stream)
{
	std::ostringstream buffer;
	buffer << stream.rdbuf();
	return buffer.str();
}

bool IsFileTooLarge(const std::filesystem::path& path)
{
	return std::filesystem::file_size(path) > MAX_FILE_SIZE;
}

bool IsBinaryFile(const std::filesystem::path& path)
{
	std::ifstream stream(path, std::ios::binary);
	if (!stream.is_open())
	{
		return false;
	}

	char buffer[BINARY_CHECK_SIZE];
	stream.read(buffer, sizeof(buffer));
	const std::streamsize bytesRead = stream.gcount();

	for (std::streamsize i = 0; i < bytesRead; ++i)
	{
		if (buffer[i] == '\0')
		{
			return true;
		}
	}

	return false;
}
} // namespace

namespace Reader
{
std::string Read(const std::filesystem::path& filePath)
{
	AssertIsFileExists(filePath);
	AssertIsRegularFile(filePath);

	if (IsFileTooLarge(filePath))
	{
		return "// [Файл пропущен: превышен лимит размера (слишком большой)]\n";
	}

	if (IsBinaryFile(filePath))
	{
		return "// [Файл пропущен: обнаружен бинарный формат]\n";
	}

	const std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
	AssertIsFileOpen(fileStream);

	return ReadStreamContent(fileStream);
}
} // namespace Reader
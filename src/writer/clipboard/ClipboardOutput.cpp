#include "ClipboardOutput.hpp"
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdio>
#endif

namespace
{
#ifdef _WIN32
void AssertClipboardOpened(const BOOL success)
{
	if (!success)
	{
		throw std::runtime_error("Не удалось открыть буфер обмена Windows");
	}
}

void AssertClipboardEmptied(const BOOL success)
{
	if (!success)
	{
		throw std::runtime_error("Не удалось очистить буфер обмена Windows");
	}
}

void AssertMemoryAllocated(const HGLOBAL handle)
{
	if (handle == nullptr)
	{
		throw std::runtime_error("Не удалось выделить память для буфера обмена");
	}
}

void AssertPointerValid(const void* ptr)
{
	if (ptr == nullptr)
	{
		throw std::runtime_error("Ошибка блокировки памяти буфера обмена");
	}
}

std::wstring Utf8ToWide(const std::string& utf8Str)
{
	if (utf8Str.empty()) return L"";
	const int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], static_cast<int>(utf8Str.size()), nullptr, 0);
	std::wstring wstrTo(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], static_cast<int>(utf8Str.size()), &wstrTo[0], sizeNeeded);
	return wstrTo;
}

void WriteToWindowsClipboard(const std::string& content)
{
	AssertClipboardOpened(OpenClipboard(nullptr));
	AssertClipboardEmptied(EmptyClipboard());

	const std::wstring wideContent = Utf8ToWide(content);

	const size_t sizeInBytes = (wideContent.size() + 1) * sizeof(wchar_t);
	const HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
	AssertMemoryAllocated(handle);

	void* data = GlobalLock(handle);
	AssertPointerValid(data);

	std::memcpy(data, wideContent.c_str(), sizeInBytes);
	GlobalUnlock(handle);

	SetClipboardData(CF_UNICODETEXT, handle);
	CloseClipboard();
}
#else
void AssertPipeOpened(FILE* pipe)
{
	if (pipe == nullptr)
	{
		throw std::runtime_error("Не удалось открыть процесс для записи в буфер обмена");
	}
}

void WriteToPosixClipboard(const std::string& content)
{
#ifdef __APPLE__
	const char* command = "pbcopy";
#else
	const char* command = "xclip -selection clipboard";
#endif
	FILE* pipe = popen(command, "w");
	AssertPipeOpened(pipe);

	std::fwrite(content.c_str(), 1, content.size(), pipe);
	pclose(pipe);
}
#endif
} // namespace

ClipboardOutput::ClipboardOutput() = default;

ClipboardOutput::~ClipboardOutput() = default;

void ClipboardOutput::Write(const std::string& content)
{
#ifdef _WIN32
	WriteToWindowsClipboard(content);
#else
	WriteToPosixClipboard(content);
#endif
}
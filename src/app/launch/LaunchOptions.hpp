#pragma once

#include <filesystem>

struct LaunchOptions
{
	std::filesystem::path rootPath;
	bool isHelpRequested = false;
	bool useClipboard = false;
	bool requiresConfirmation = false;
};
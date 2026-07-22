#pragma once

#include "IAppMode.hpp"
#include <filesystem>
#include <memory>

class Application
{
public:
	Application(int argc, char* argv[]);
	~Application();

	void Run() const;

private:
	std::filesystem::path m_rootPath;
	bool m_useClipboard;
	bool m_isHelpRequested;
	bool m_requiresConfirmation;
	std::unique_ptr<IAppMode> m_mode;
};
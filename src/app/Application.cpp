#include "Application.hpp"
#include "app/launch/LaunchCommandLine.hpp"
#include "filter/IgnoreFilter.hpp"
#include "mods/default/DefaultMode.hpp"
#include "utils/cli/Cli.hpp"
#include "utils/formatter/MarkdownFormatter.hpp"
#include "writer/clipboard/ClipboardOutput.hpp"
#include "writer/file/FileOutput.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
void AssertModeAssigned(const IAppMode* mode)
{
	if (mode == nullptr)
	{
		throw std::runtime_error("Внутренняя ошибка: режим работы не инициализирован");
	}
}

std::unique_ptr<IAppMode> CreateDefaultMode(const std::filesystem::path& rootPath)
{
	const IgnoreFilter filter(rootPath / ".gitignore");
	return std::make_unique<DefaultMode>(rootPath, filter);
}

bool IsPositiveAnswer(const std::string& answer)
{
	return answer == "y" || answer == "Y" || answer == "н" || answer == "Н";
}

bool AskConfirmation()
{
	std::cout << "Будет просканирована текущая директория" << std::endl
			  << "Продолжить? (y/n): " << std::flush;

	std::string answer;
	std::getline(std::cin, answer);

	return IsPositiveAnswer(answer);
}

std::unique_ptr<IOutputTarget> CreateOutputTarget(const LaunchOptions& options)
{
	if (options.useClipboard)
	{
		return std::make_unique<ClipboardOutput>();
	}

	return std::make_unique<FileOutput>(options.rootPath);
}

void PrintSuccessMessage(const bool useClipboard)
{
	if (useClipboard)
	{
		std::cout << "Проект успешно скопирован в буфер обмена" << std::endl;
	}
	else
	{
		std::cout << "Проект успешно собран в файл dump.md" << std::endl;
	}
}
} // namespace

Application::Application(const int argc, char* argv[])
	: m_options(ParseLaunchOptions(cli::ReadArguments(argc, argv)))
{
	if (!m_options.isHelpRequested)
	{
		m_mode = CreateDefaultMode(m_options.rootPath);
	}
}

Application::~Application() = default;

void Application::Run() const
{
	if (m_options.isHelpRequested)
	{
		std::cout << BuildHelpMessage() << std::endl;
		return;
	}

	if (m_options.requiresConfirmation && !AskConfirmation())
	{
		std::cout << "Сборка отменена" << std::endl;
		return;
	}

	AssertModeAssigned(m_mode.get());

	MarkdownFormatter formatter;
	formatter.AddHeader(1, "Project Dump");
	m_mode->Execute(formatter);

	CreateOutputTarget(m_options)->Write(formatter.Build());
	PrintSuccessMessage(m_options.useClipboard);
}
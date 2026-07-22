#include "Application.hpp"
#include "filter/IgnoreFilter.hpp"
#include "mods/default/DefaultMode.hpp"
#include "utils/formatter/MarkdownFormatter.hpp"
#include "writer/clipboard/ClipboardOutput.hpp"
#include "writer/file/FileOutput.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
void AssertHasArguments(const int argc)
{
	if (argc <= 0)
	{
		throw std::invalid_argument("Недопустимое количество аргументов командной строки");
	}
}

void AssertModeAssigned(const IAppMode* mode)
{
	if (mode == nullptr)
	{
		throw std::runtime_error("Внутренняя ошибка: режим работы не инициализирован");
	}
}

std::vector<std::string> ConvertArguments(const int argc, char* argv[])
{
	std::vector<std::string> args;
	args.reserve(static_cast<size_t>(argc));

	for (int i = 0; i < argc; ++i)
	{
		args.emplace_back(argv[i]);
	}

	return args;
}

bool ShouldShowHelp(const std::vector<std::string>& args)
{
	for (const auto& arg : args)
	{
		if (arg == "--help" || arg == "-h")
		{
			return true;
		}
	}

	return false;
}

bool HasClipboardFlag(const std::vector<std::string>& args)
{
	for (const auto& arg : args)
	{
		if (arg == "--clipboard" || arg == "-c")
		{
			return true;
		}
	}
	return false;
}

std::filesystem::path ExtractRootPath(const std::vector<std::string>& args)
{
	if (args.size() > 1 && !args[1].starts_with("-"))
	{
		return {args[1]};
	}

	return std::filesystem::current_path();
}

void PrintHelpMessage()
{
	std::cout << "Project Dumper - CLI утилита для сборки проекта в Markdown\n\n"
			  << "Использование:\n"
			  << "  dumper [путь_к_проекту] [опции]\n\n"
			  << "Опции:\n"
			  << "  -h, --help       Показать эту справку и выйти\n"
			  << "  -c, --clipboard  Скопировать результат в буфер обмена\n\n"
			  << "Примеры:\n"
			  << "  dumper           Собрать текущую папку в файл dump.md (спросит подтверждение)\n"
			  << "  dumper .         Собрать текущую папку (без подтверждения)\n"
			  << "  dumper -c        Собрать текущую папку в буфер обмена\n"
			  << "  dumper C:\\App    Собрать проект в файл C:\\App\\dump.md\n"
			  << std::endl;
}
} // namespace

Application::Application(const int argc, char* argv[])
	: m_useClipboard(false)
	, m_isHelpRequested(false)
	, m_requiresConfirmation(false)
{
	AssertHasArguments(argc);
	const std::vector<std::string> args = ConvertArguments(argc, argv);

	m_isHelpRequested = ShouldShowHelp(args);
	if (m_isHelpRequested)
	{
		return;
	}

	if (args.size() == 1)
	{
		m_requiresConfirmation = true;
	}

	m_rootPath = ExtractRootPath(args);
	m_useClipboard = HasClipboardFlag(args);

	IgnoreFilter filter(m_rootPath / ".gitignore");
	m_mode = std::make_unique<DefaultMode>(m_rootPath, filter);
}

Application::~Application() = default;

void Application::Run() const
{
	if (m_isHelpRequested)
	{
		PrintHelpMessage();
		return;
	}

	if (m_requiresConfirmation)
	{
		std::cout << "Будет просканирована директория:\n  "
				  << m_rootPath.string() << "\n"
				  << "Продолжить? (y/n): ";

		std::string answer;
		std::getline(std::cin, answer);

		if (answer != "y" && answer != "Y" && answer != "н" && answer != "Н")
		{
			std::cout << "Сборка отменена\n";
			return;
		}
	}

	AssertModeAssigned(m_mode.get());

	std::unique_ptr<IOutputTarget> target;
	if (m_useClipboard)
	{
		target = std::make_unique<ClipboardOutput>();
	}
	else
	{
		target = std::make_unique<FileOutput>(m_rootPath);
	}

	MarkdownFormatter formatter;
	formatter.AddHeader(1, "Project Dump");

	m_mode->Execute(formatter);

	target->Write(formatter.Build());

	if (!m_useClipboard)
	{
		std::cout << "Проект успешно собран в файл dump.md\n";
	}
	else
	{
		std::cout << "Проект успешно скопирован в буфер обмена\n";
	}
}
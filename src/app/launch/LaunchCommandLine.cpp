#include "LaunchCommandLine.hpp"
#include "utils/cli/Cli.hpp"
#include <sstream>
#include <stdexcept>

namespace
{
using LaunchDispatcher = cli::FlagDispatcher<LaunchOptions>;

LaunchDispatcher CreateDispatcher()
{
	LaunchDispatcher dispatcher;

	dispatcher.BindSwitch(
		{ 'h', "help", "Показать эту справку и выйти" },
		&LaunchOptions::isHelpRequested);

	dispatcher.BindSwitch(
		{ 'c', "clipboard", "Скопировать результат в буфер обмена" },
		&LaunchOptions::useClipboard);

	return dispatcher;
}

void AssertHasAtMostOnePath(const std::vector<std::string>& positionals)
{
	if (positionals.size() > 1)
	{
		throw std::invalid_argument("Ожидается не более одного пути к проекту");
	}
}

std::filesystem::path ResolveRootPath(const std::vector<std::string>& positionals)
{
	AssertHasAtMostOnePath(positionals);

	if (positionals.empty())
	{
		return std::filesystem::current_path();
	}

	return { positionals.front() };
}
} // namespace

LaunchOptions ParseLaunchOptions(const std::vector<std::string>& arguments)
{
	LaunchOptions options;

	const std::vector<std::string> positionals = CreateDispatcher().Dispatch(arguments, options);
	options.rootPath = ResolveRootPath(positionals);
	options.requiresConfirmation = arguments.empty();

	return options;
}

std::string BuildHelpMessage()
{
	std::ostringstream stream;

	stream << "Project Parser - CLI утилита для сборки проекта в Markdown" << std::endl
		   << std::endl
		   << "Использование:" << std::endl
		   << "  parser [путь_к_проекту] [опции]" << std::endl
		   << std::endl
		   << "Опции:" << std::endl
		   << CreateDispatcher().BuildHelp() << std::endl
		   << "Примеры:" << std::endl
		   << "  parser           Собрать текущую папку в dump.md (спросит подтверждение)" << std::endl
		   << "  parser .         Собрать текущую папку (без подтверждения)" << std::endl
		   << "  parser -c        Собрать текущую папку в буфер обмена" << std::endl
		   << "  parser -c C:\\App Собрать проект C:\\App в буфер обмена" << std::endl;

	return stream.str();
}
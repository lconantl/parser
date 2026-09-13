#include "app/Application.hpp"
#include "app/launch/LaunchCommandLine.hpp"
#include "utils/cli/Cli.hpp"
#include "utils/console/ConsoleEncoding.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding console;
		const Application app(argc, argv);
		app.Run();
	}
	catch (const cli::UsageError& e)
	{
		std::cerr << "[Error] \t" << e.what() << std::endl;
		std::cerr << BuildHelpMessage() << std::endl;
		return cli::UsageError::EXIT_CODE;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[Error] \t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
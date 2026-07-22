#include "app/Application.hpp"
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
	catch (const std::exception& e)
	{
		std::cerr << "[Error] \t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
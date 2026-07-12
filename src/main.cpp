#include <exception>
#include <iostream>
#include <ostream>
#include <vector>

int main(const int argc, char** argv)
{
	try
	{
		std::vector<std::string> args(argv, argv + argc);
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
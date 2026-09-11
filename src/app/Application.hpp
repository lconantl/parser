#pragma once

#include "IAppMode.hpp"
#include "app/launch/LaunchOptions.hpp"
#include <memory>

class Application
{
public:
	Application(int argc, char* argv[]);
	~Application();

	void Run() const;

private:
	LaunchOptions m_options;
	std::unique_ptr<IAppMode> m_mode;
};
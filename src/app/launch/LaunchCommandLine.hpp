#pragma once

#include "app/launch/LaunchOptions.hpp"
#include <string>
#include <vector>

LaunchOptions ParseLaunchOptions(const std::vector<std::string>& arguments);

std::string BuildHelpMessage();
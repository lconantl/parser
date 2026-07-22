#pragma once

#include <filesystem>
#include <string>

namespace Reader
{
std::string Read(const std::filesystem::path& filePath);
}
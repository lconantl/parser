#pragma once

#include "../IOutputTarget.hpp"
#include <filesystem>

class FileOutput final : public IOutputTarget
{
public:
	explicit FileOutput(std::filesystem::path rootPath);
	~FileOutput() override;

	void Write(const std::string& content) override;

private:
	std::filesystem::path m_outputPath;
};
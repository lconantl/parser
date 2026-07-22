#pragma once

#include "../IOutputTarget.hpp"

class ClipboardOutput final : public IOutputTarget
{
public:
	ClipboardOutput();
	~ClipboardOutput() override;

	void Write(const std::string& content) override;
};
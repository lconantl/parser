#pragma once

#include <string>

class IOutputTarget
{
public:
	virtual ~IOutputTarget() = default;
	virtual void Write(const std::string& content) = 0;
};
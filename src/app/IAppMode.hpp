#pragma once

#include "utils/formatter/MarkdownFormatter.hpp"

class IAppMode
{
public:
	virtual ~IAppMode() = default;
	virtual void Execute(MarkdownFormatter& formatter) = 0;
};
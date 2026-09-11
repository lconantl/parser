#pragma once

#include <algorithm>
#include <cctype>
#include <functional>
#include <iomanip>
#include <memory>
#include <ostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cli
{
struct FlagSpec
{
	char shortName;
	std::string longName;
	std::string description;
};

template <typename Receiver>
class ICommand
{
public:
	virtual ~ICommand() = default;

	virtual void Execute(Receiver& receiver) const = 0;
};

namespace detail
{
template <typename Receiver>
struct Binding
{
	FlagSpec spec;
	std::unique_ptr<ICommand<Receiver>> command;
};

template <typename Receiver>
using Bindings = std::vector<Binding<Receiver>>;

struct ParseResult
{
	std::vector<bool> triggered;
	std::vector<std::string> positionals;
};

inline bool HasValueSeparator(const std::string_view text) noexcept
{
	return text.find('=') != std::string_view::npos;
}

inline std::string_view StripValue(const std::string_view text) noexcept
{
	return text.substr(0, text.find('='));
}

inline std::string FormatFlag(const FlagSpec& spec)
{
	return std::string("-") + spec.shortName + ", --" + spec.longName;
}

inline bool SharesName(const FlagSpec& left, const FlagSpec& right) noexcept
{
	return left.shortName == right.shortName || left.longName == right.longName;
}

inline void AssertIsArgumentCountPositive(const int argc)
{
	if (argc <= 0)
	{
		throw std::invalid_argument("Недопустимое количество аргументов командной строки");
	}
}

inline void AssertIsArgumentArrayPresent(const char* const* argv)
{
	if (argv == nullptr)
	{
		throw std::invalid_argument("Массив аргументов командной строки отсутствует");
	}
}

template <typename Pointer>
void AssertIsNotNull(const Pointer& pointer)
{
	if (pointer == nullptr)
	{
		throw std::invalid_argument("Обработчик флага не задан");
	}
}

inline void AssertIsValidShortName(const char name)
{
	if (std::isalnum(static_cast<unsigned char>(name)) == 0)
	{
		throw std::invalid_argument("Короткое имя флага должно быть латинской буквой или цифрой");
	}
}

inline void AssertIsValidLongName(const std::string_view name)
{
	if (name.empty() || name.starts_with('-') || HasValueSeparator(name))
	{
		throw std::invalid_argument("Некорректное длинное имя флага: " + std::string(name));
	}
}

template <typename Receiver>
void AssertIsUnique(const Bindings<Receiver>& bindings, const FlagSpec& spec)
{
	const bool isTaken = std::ranges::any_of(bindings, [&spec](const auto& binding) {
		return SharesName(binding.spec, spec);
	});

	if (isTaken)
	{
		throw std::logic_error("Флаг уже зарегистрирован: " + FormatFlag(spec));
	}
}

inline void AssertIsKnownFlag(const bool isKnown, const std::string& flag)
{
	if (!isKnown)
	{
		throw std::invalid_argument("Неизвестный флаг: " + flag);
	}
}

inline void AssertHasNoValue(const std::string_view name)
{
	if (HasValueSeparator(name))
	{
		throw std::invalid_argument(
			"Флаг --" + std::string(StripValue(name)) + " не принимает значение");
	}
}

inline bool IsWhitespace(const char symbol) noexcept
{
	return std::isspace(static_cast<unsigned char>(symbol)) != 0;
}

inline std::string Trim(const std::string_view text)
{
	const auto begin = std::find_if_not(text.begin(), text.end(), IsWhitespace);
	const auto end = std::find_if_not(text.rbegin(), text.rend(), IsWhitespace).base();

	return begin < end ? std::string(begin, end) : std::string();
}

inline bool IsEndOfFlags(const std::string_view argument) noexcept
{
	return argument == "--";
}

inline bool IsLongFlag(const std::string_view argument) noexcept
{
	return argument.size() > 2 && argument.starts_with("--");
}

inline bool IsShortFlagGroup(const std::string_view argument) noexcept
{
	return argument.size() > 1 && argument.starts_with('-') && !argument.starts_with("--");
}

template <typename Receiver>
std::size_t FindByLongName(const Bindings<Receiver>& bindings, const std::string_view name)
{
	const auto it = std::ranges::find_if(bindings, [name](const auto& binding) {
		return binding.spec.longName == name;
	});

	AssertIsKnownFlag(it != bindings.end(), "--" + std::string(name));
	return static_cast<std::size_t>(it - bindings.begin());
}

template <typename Receiver>
std::size_t FindByShortName(const Bindings<Receiver>& bindings, const char name)
{
	const auto it = std::ranges::find_if(bindings, [name](const auto& binding) {
		return binding.spec.shortName == name;
	});

	AssertIsKnownFlag(it != bindings.end(), std::string("-") + name);
	return static_cast<std::size_t>(it - bindings.begin());
}

template <typename Receiver>
void MarkLongFlag(
	const Bindings<Receiver>& bindings, const std::string_view name, std::vector<bool>& triggered)
{
	AssertHasNoValue(name);
	triggered[FindByLongName(bindings, name)] = true;
}

template <typename Receiver>
void MarkShortFlagGroup(
	const Bindings<Receiver>& bindings, const std::string_view group, std::vector<bool>& triggered)
{
	for (const char name : group)
	{
		triggered[FindByShortName(bindings, name)] = true;
	}
}

template <typename Receiver>
void ParseArgument(
	const Bindings<Receiver>& bindings, const std::string& argument, ParseResult& result)
{
	const std::string_view view = argument;

	if (IsLongFlag(view))
	{
		MarkLongFlag(bindings, view.substr(2), result.triggered);
	}
	else if (IsShortFlagGroup(view))
	{
		MarkShortFlagGroup(bindings, view.substr(1), result.triggered);
	}
	else
	{
		result.positionals.push_back(argument);
	}
}

template <typename Receiver>
ParseResult Parse(const Bindings<Receiver>& bindings, const std::vector<std::string>& arguments)
{
	ParseResult result{std::vector<bool>(bindings.size(), false), {}};

	for (auto it = arguments.begin(); it != arguments.end(); ++it)
	{
		if (IsEndOfFlags(*it))
		{
			result.positionals.insert(result.positionals.end(), std::next(it), arguments.end());
			break;
		}
		ParseArgument(bindings, *it, result);
	}

	return result;
}

template <typename Receiver>
void ExecuteTriggered(
	const Bindings<Receiver>& bindings, const std::vector<bool>& triggered, Receiver& receiver)
{
	for (std::size_t index = 0; index < bindings.size(); ++index)
	{
		if (triggered[index])
		{
			bindings[index].command->Execute(receiver);
		}
	}
}

template <typename Receiver>
std::size_t CalculateNameWidth(const Bindings<Receiver>& bindings)
{
	std::size_t width = 0;
	for (const auto& binding : bindings)
	{
		width = std::max(width, binding.spec.longName.size());
	}
	return width;
}

inline void WriteHelpLine(std::ostream& stream, const FlagSpec& spec, const std::size_t nameWidth)
{
	stream << "  -" << spec.shortName << ", --" << std::left
		   << std::setw(static_cast<int>(nameWidth)) << spec.longName << "  " << spec.description
		   << std::endl;
}

template <typename Receiver>
std::string BuildHelp(const Bindings<Receiver>& bindings)
{
	const std::size_t nameWidth = CalculateNameWidth(bindings);
	std::ostringstream stream;

	for (const auto& binding : bindings)
	{
		WriteHelpLine(stream, binding.spec, nameWidth);
	}

	return stream.str();
}
} // namespace detail

template <typename Receiver>
class SetSwitchCommand final : public ICommand<Receiver>
{
public:
	explicit SetSwitchCommand(bool Receiver::* field)
		: m_field(field)
	{
		detail::AssertIsNotNull(m_field);
	}

	void Execute(Receiver& receiver) const override
	{
		receiver.*m_field = true;
	}

private:
	bool Receiver::* m_field;
};

template <typename Receiver>
class ActionCommand final : public ICommand<Receiver>
{
public:
	using Action = std::function<void(Receiver&)>;

	explicit ActionCommand(Action action)
		: m_action(std::move(action))
	{
		detail::AssertIsNotNull(m_action);
	}

	void Execute(Receiver& receiver) const override
	{
		m_action(receiver);
	}

private:
	Action m_action;
};

template <typename Receiver>
class FlagDispatcher
{
public:
	using Action = typename ActionCommand<Receiver>::Action;

	void Bind(FlagSpec spec, std::unique_ptr<ICommand<Receiver>> command)
	{
		detail::AssertIsNotNull(command);
		detail::AssertIsValidShortName(spec.shortName);
		detail::AssertIsValidLongName(spec.longName);
		detail::AssertIsUnique(m_bindings, spec);

		m_bindings.push_back({std::move(spec), std::move(command)});
	}

	void BindSwitch(FlagSpec spec, bool Receiver::* field)
	{
		Bind(std::move(spec), std::make_unique<SetSwitchCommand<Receiver>>(field));
	}

	void BindAction(FlagSpec spec, Action action)
	{
		Bind(std::move(spec), std::make_unique<ActionCommand<Receiver>>(std::move(action)));
	}

	std::vector<std::string> Dispatch(
		const std::vector<std::string>& arguments, Receiver& receiver) const
	{
		detail::ParseResult result = detail::Parse(m_bindings, arguments);
		detail::ExecuteTriggered(m_bindings, result.triggered, receiver);
		return std::move(result.positionals);
	}

	std::string BuildHelp() const
	{
		return detail::BuildHelp(m_bindings);
	}

private:
	detail::Bindings<Receiver> m_bindings;
};

inline std::vector<std::string> ReadArguments(const int argc, const char* const* argv)
{
	detail::AssertIsArgumentCountPositive(argc);
	detail::AssertIsArgumentArrayPresent(argv);

	std::vector<std::string> arguments;
	arguments.reserve(static_cast<std::size_t>(argc));

	for (const char* rawArgument : std::span(argv, static_cast<std::size_t>(argc)).subspan(1))
	{
		std::string argument = detail::Trim(rawArgument);
		if (!argument.empty())
		{
			arguments.push_back(std::move(argument));
		}
	}

	return arguments;
}
} // namespace cli
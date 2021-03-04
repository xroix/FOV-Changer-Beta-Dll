#pragma once

#include "pch.h"

#include "utils/Logger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


enum OptionT : int
{
	stringT = 0, intT, groupT, keybindT
};

class IOption
{
public:
	std::string m_Id;
	std::string	m_Label;

	void* m_Data_p = nullptr;  // Pointer to the real data
	OptionT m_OptionT;

	IOption() {}

	virtual json dump()
	{
		return {};
	}
};
 
class Group : public IOption
{
private:
	// Data will save options
	std::map<std::string, std::shared_ptr<IOption>> m_Data;

public:
	Group(const std::string id, const std::string label, std::map<std::string, std::shared_ptr<IOption>> options);

	json dump() override;

	// Note: That's not a virtual function!
	auto& get();
};

class TextInput : public IOption
{
private:
	std::string m_Data;

public:
	TextInput(const std::string id, const std::string label);

	json dump() override;

	// Note: That's not a virtual function!
	const std::string& get();

	// Note: That's not a virtual function!
	bool set(const std::string& value);
};

class IntInput : public IOption
{
private:
	int m_Data;

public:
	IntInput(const std::string id, const std::string label);

	json dump() override;

	// Note: That's not a virtual function!
	int get();
};

// We need the wrapper for GROUP
inline auto _wrapper(std::map<std::string, std::shared_ptr<IOption>> options)
{
	return options;
}

#define GROUP(id, label, ...) {label, std::make_shared<Group>(id, label, _wrapper(__VA_ARGS__))}
#define TEXTINPUT(id, label) {label, std::make_shared<TextInput>(id, label)}
#define INTINPUT(id, label) {label, std::make_shared<IntInput>(id, label)}


class ModuleSettings
{
public:
	std::map<std::string, std::shared_ptr<IOption>> m_Options;

	ModuleSettings(std::map<std::string, std::shared_ptr<IOption>> options)
		: m_Options(options)
	{}

	template <OptionT optionType>
	auto getOption(const std::string& optionId);

	json dump();

private:
	std::shared_ptr<IOption> getIOption(std::string id);
};

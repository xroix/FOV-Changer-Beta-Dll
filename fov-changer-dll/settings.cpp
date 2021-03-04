#include "settings.h"


////////////////////////////////// Group //////////////////////////////////

Group::Group(const std::string id, const std::string label, std::map<std::string, std::shared_ptr<IOption>> options)
{
	m_Id = id;
	m_Label = label;

	m_OptionT = groupT;

	m_Data = options;
	m_Data_p = (void*)&m_Data;
}

json Group::dump()
{
	json j = {
			{"id", m_Id},
			{"label", m_Label},
			{"type", m_OptionT},
			{"data", {}}
	};

	for (auto& it : m_Data)
	{
		j["data"].push_back(it.second->dump());
	}

	return j;
}

auto& Group::get()
{
	return m_Data;
}


////////////////////////////////// TextInput //////////////////////////////////

TextInput::TextInput(const std::string id, const std::string label)
{
	m_Id = id;
	m_Label = label;

	m_OptionT = stringT;

	m_Data = "hello";
	m_Data_p = (void*)&m_Data;
}

json TextInput::dump()
{
	std::string data;

	if (!m_Data_p)
		data = "";

	else
		data = *(std::string*)m_Data_p;

	return {
		{"id", m_Id},
		{"label", m_Label},
		{"type", m_OptionT},
		{"data", data}
	};
}

const std::string& TextInput::get()
{
	return m_Data;
}

bool TextInput::set(const std::string& value)
{
	m_Data = value;
	return true;
}


////////////////////////////////// IntInput //////////////////////////////////

IntInput::IntInput(const std::string id, const std::string label)
{
	m_Id = id;
	m_Label = label;

	m_OptionT = intT;

	m_Data = 12;
	m_Data_p = (void*)&m_Data;
}

json IntInput::dump()
{
	int data;

	if (!m_Data_p)
		data = 0;

	else
		data = *(int*)m_Data_p;

	return {
		{"id", m_Id},
		{"label", m_Label},
		{"type", m_OptionT},
		{"data", data}
	};

	//return std::to_string(*(int*)m_Data_p);
}

int IntInput::get()
{
	return m_Data;
}


////////////////////////////////// ModuleSettings //////////////////////////////////

template <OptionT optionType>
auto ModuleSettings::getOption(const std::string& optionId)
{
	auto before = getIOption(optionId);

	// String
	if constexpr (optionType == stringT)
	{
		return std::dynamic_pointer_cast<TextInput>(before);
	}
	// Int
	else if constexpr (optionType == intT)
	{
		return std::dynamic_pointer_cast<IntInput>(before);
	}
}

json ModuleSettings::dump()
{
	json j;

	for (auto& it : m_Options)
	{
		j.push_back(it.second->dump());
	}

	return j;
}

std::shared_ptr<IOption> ModuleSettings::getIOption(std::string id)
{
	return m_Options[id];
}
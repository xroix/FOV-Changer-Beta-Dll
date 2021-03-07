#include "settings.h"


namespace Core::Settings
{
	////////////////////////////////// Group //////////////////////////////////

	Core::Settings::Group::Group(const std::string id, const std::string label, std::map<std::string, std::shared_ptr<IOption>> options)
	{
		m_id = id;
		m_label = label;

		m_optionT = OptionT::groupT;

		m_data = options;
		m_data_p = (void*)&m_data;
	}

	json Group::DumpJSON()
	{
		json j = {
				{"id", m_id},
				{"label", m_label},
				{"type", m_optionT},
				{"data", {}}
		};

		for (auto& it : m_data)
		{
			j["data"].push_back(it.second->DumpJSON());
		}

		return j;
	}

	auto& Group::get()
	{
		return m_data;
	}


	////////////////////////////////// TextInput //////////////////////////////////

	TextInput::TextInput(const std::string id, const std::string label)
	{
		m_id = id;
		m_label = label;

		m_optionT = OptionT::stringT;

		m_data = "hello";
		m_data_p = (void*)&m_data;
	}

	json TextInput::DumpJSON()
	{
		std::string data;

		if (!m_data_p)
			data = "";

		else
			data = *(std::string*)m_data_p;

		return {
			{"id", m_id},
			{"label", m_label},
			{"type", m_optionT},
			{"data", data}
		};
	}

	const std::string& TextInput::get()
	{
		return m_data;
	}

	bool TextInput::set(const std::string& value)
	{
		m_data = value;
		return true;
	}


	////////////////////////////////// IntInput //////////////////////////////////

	IntInput::IntInput(const std::string id, const std::string label)
	{
		m_id = id;
		m_label = label;

		m_optionT = OptionT::intT;

		m_data = 12;
		m_data_p = (void*)&m_data;
	}

	json IntInput::DumpJSON()
	{
		int data;

		if (!m_data_p)
			data = 0;

		else
			data = *(int*)m_data_p;

		return {
			{"id", m_id},
			{"label", m_label},
			{"type", m_optionT},
			{"data", data}
		};

		//return std::to_string(*(int*)m_Data_p);
	}

	int IntInput::get()
	{
		return m_data;
	}


	////////////////////////////////// ModuleSettings //////////////////////////////////

	template <OptionT optionType>
	auto ModuleSettings::GetOption(const std::string& optionId)
	{
		auto before = GetIOption(optionId);

		// String
		if constexpr (optionType == OptionT::stringT)
		{
			return std::dynamic_pointer_cast<TextInput>(before);
		}
		// Int
		else if constexpr (optionType == OptionT::intT)
		{
			return std::dynamic_pointer_cast<IntInput>(before);
		}
	}

	json ModuleSettings::DumpJSON()
	{
		json j;

		for (auto& it : m_Options)
		{
			j.push_back(it.second->DumpJSON());
		}

		return j;
	}

	std::shared_ptr<IOption> ModuleSettings::GetIOption(std::string id)
	{
		return m_Options[id];
	}
}
#pragma once

#include "../pch.h"

#include "../utils/Logger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace Core::Settings
{
	enum class OptionT : int
	{
		stringT = 0, intT, groupT, keybindT
	};

	class IOption
	{
	public:
		std::string			m_id;
		std::string			m_label;

		void* m_data_p = nullptr;  // Pointer to the real data
		OptionT				m_optionT;

		IOption() : m_optionT(OptionT::stringT) {}

		virtual json DumpJSON()
		{
			return {};
		}
	};

	class Group : public IOption
	{
	private:
		// Data will save options
		std::map<std::string, std::shared_ptr<IOption>> m_data;

	public:
		Group(const std::string id, const std::string label, std::map<std::string, std::shared_ptr<IOption>> options);

		json DumpJSON() override;

		// Note: That's not a virtual function!
		auto& get();
	};

	class TextInput : public IOption
	{
	private:
		std::string m_data;

	public:
		TextInput(const std::string id, const std::string label);

		json DumpJSON() override;

		// Note: That's not a virtual function!
		const std::string& get();

		// Note: That's not a virtual function!
		bool set(const std::string& value);
	};

	class IntInput : public IOption
	{
	private:
		int m_data;

	public:
		IntInput(const std::string id, const std::string label);

		json DumpJSON() override;

		// Note: That's not a virtual function!
		int get();
	};

	// We need the wrapper for GROUP
	inline auto _wrapper(std::map<std::string, std::shared_ptr<IOption>> options)
	{
		return options;
	}

	#define GROUP(id, label, ...) {label, std::make_shared<Core::Settings::Group>(id, label, Core::Settings::_wrapper(__VA_ARGS__))}
	#define TEXTINPUT(id, label) {label, std::make_shared<Core::Settings::TextInput>(id, label)}
	#define INTINPUT(id, label) {label, std::make_shared<Core::Settings::IntInput>(id, label)}


	class ModuleSettings
	{
	public:
		std::map<std::string, std::shared_ptr<IOption>> m_Options;

		ModuleSettings(std::map<std::string, std::shared_ptr<IOption>> options)
			: m_Options(options)
		{
		}

		template <OptionT optionType>
		auto GetOption(const std::string& optionId);

		json DumpJSON();

	private:
		std::shared_ptr<IOption> GetIOption(std::string id);
	};
}
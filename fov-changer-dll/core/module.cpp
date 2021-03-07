#include "module.h"
#include "../utils/Logger.h"

#include "../modules/zoom.h"
#include "../modules/nametag.h"
#include "../modules/freelook.h"


namespace Core
{
	Signature::Signature(const std::string& signature)
	{
		this->m_mode = 0;
		this->m_signature = signature;
		this->m_aob1 = "";
		this->m_aob2 = "";
		this->m_aobLength = 0;
	}

	Signature::Signature(const std::string& signature, int aobLength, const char* aob)
	{
		this->m_mode = 1;
		this->m_signature = signature;
		this->m_aob1 = aob;
		this->m_aob2 = "";
		this->m_aobLength = aobLength;
	}

	Signature::Signature(const std::string& signature, int aobLength, const char* aob1, const char* aob2)
	{
		this->m_mode = 2;
		this->m_signature = signature;
		this->m_aob1 = aob1;
		this->m_aob2 = aob2;
		this->m_aobLength = aobLength;
	}

	Signature::~Signature()
	{
		this->Deactivate();
	}

	void Signature::initGameData()
	{
		// Get location of signature
		m_addr = mem::GetAddressFromSignature("Minecraft.Windows.exe", m_signature.c_str());

		if (m_mode == 2)
		{
			// Set aob1
			mem::Patch((BYTE*)m_addr, (BYTE*)m_aob1, m_aobLength);
		}
	}

	void Signature::Activate()
	{
		switch (m_mode)
		{
		case 0:
			break;

		case 1:
			mem::Nop((BYTE*)m_addr, m_aobLength);
			break;

		case 2:
			mem::Patch((BYTE*)m_addr, (BYTE*)m_aob2, m_aobLength);
			break;
		}
	}

	void Signature::Deactivate()
	{
		switch (m_mode)
		{
		case 0:
			break;

		case 1:
			mem::Patch((BYTE*)m_addr, (BYTE*)m_aob1, m_aobLength);
			break;

		case 2:
			mem::Patch((BYTE*)m_addr, (BYTE*)m_aob1, m_aobLength);
			break;
		}
	}


	namespace Module
	{
		ModuleManager::ModuleManager()
		{
			// Add here modules
			this->m_modules.push_back(std::make_shared<Zoom>());
			//this->m_modules.push_back(std::make_shared<Nametag>());
			this->m_modules.push_back(std::make_shared<Freelook>());
		}

		ModuleManager::~ModuleManager()
		{
			// Call onStop for all modules
			LOG(L"Calling onStop() for all modules");
			for (auto const& module_ : this->m_modules)
			{
				module_->onStop();
			}
		}

		void ModuleManager::initGameData()
		{
			// Call initGameData for all modules
			LOG(L"Calling initGameData() for all modules");
			for (auto const& module_ : m_modules)
			{
				module_->initGameData();
			}

			// Call onStart for all modules after previous step is complete
			LOG(L"Calling onStart() for all modules");
			for (auto const& module_ : m_modules)
			{
				module_->onStart();
			}
		}

		void ModuleManager::loopHotkeys()
		{
			for (auto const& module_ : m_modules)
			{
				if (!module_->m_hotkey) continue;

				if (*module_->m_hotkey)
				{
					if (!module_->m_pressed)
					{
						module_->onPress();
						module_->m_pressed = true;
					}

				} else
				{
					if (module_->m_pressed)
					{
						module_->onRelease();
						module_->m_pressed = false;
					}
				}
			}
		}
	}
}
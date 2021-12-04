 #pragma once

#include "../pch.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Globalization::DateTimeFormatting;


class Logger
{
public:
    std::wstring    m_fileName = L"FOV-Changer-Logs.txt";
    StorageFile     m_file = nullptr;

    Logger()
    {
        this->m_file = ApplicationData::Current().LocalFolder().CreateFileAsync(this->m_fileName, CreationCollisionOption::ReplaceExisting).get();
        FileIO::AppendTextAsync(m_file, L"--------------------- " + this->getTimePrefix(L"month day year") + L"--------------------\n").get();
    }

    const std::wstring& getTimePrefix(const std::wstring& format)
    {
        DateTimeFormatter formatter{ format };
        DateTime time = winrt::clock::now();

        return formatter.Format(time).c_str();
    }

    void log(const std::wstring& msg)
    {
        std::wostringstream formated_msg;
        formated_msg << L"[" << this->getTimePrefix(L"hour minute second") << L"] " << msg << L"\n";

        OutputDebugString(formated_msg.str().c_str());
        FileIO::AppendTextAsync(m_file, formated_msg.str()).get();
    }

    void log(const std::string& msg)
    {
        log(winrt::to_hstring(msg).c_str());
    }

    const std::wstring& getCurrentPath()
    {
        return std::filesystem::current_path().wstring();
    }

    const std::wstring& getAppDataLocalPath()
    {
        return ApplicationData::Current().LocalFolder().Path().c_str();
    }
};

const std::shared_ptr<Logger> logger = std::make_shared<Logger>();
#define LOG(msg) logger->log(msg)


//void loadFileData()
//{
//    StorageFile file = ApplicationData::Current().LocalFolder().CreateFileAsync(L"settings.json", CreationCollisionOption::OpenIfExists).get();
//    std::wstring content = FileIO::ReadTextAsync(file).get().c_str();
//
//
//    using json = nlohmann::json;
//    json j = json::parse(content);
//
//    LOG(j["test"]);
//}

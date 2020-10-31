#include "xmlInfo.h"
#include <filesystem>
#include <fstream>

using namespace std;
using namespace winrt;
using namespace Windows::Data::Xml::Dom;

const wstring XmlInfo::GetConfigFile()
{
    int nArgs;
    const auto szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (nullptr == szArglist) return L"";
	wstring wstr{ szArglist[0] };
    LocalFree(szArglist);
    
	wstr.replace(wstr.end() - 3, wstr.end(), L"xml");
    return wstr;
}

const string XmlInfo::ConfigFile()
{
    wstring configFile{ XmlInfo::GetConfigFile() };
    
    const auto size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &configFile[0], static_cast<int>(configFile.size()), nullptr, 0, nullptr, nullptr);
	string configFileName = string(size, 0);
	WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &configFile[0], static_cast<int>(configFile.size()), &configFileName[0], size, nullptr, nullptr);

    return configFileName;
}

const std::string XmlInfo::Target() const
{
    wstring target{ _target.c_str() };
    
    const auto size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &target[0], static_cast<int>(target.size()), nullptr, 0, nullptr, nullptr);
	string result = string(size, 0);
	WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &target[0], static_cast<int>(target.size()), &result[0], size, nullptr, nullptr);

    return result;
}

XmlInfo::XmlInfo(const wstring& filename)
{
    if (filesystem::exists(filename))
    {
        try {
            std::ifstream toRead(filename);
            std::string fileXML(std::istreambuf_iterator<char>{toRead}, {});
            if (!fileXML.empty()) {
                
                XmlDocument infoDoc;
                infoDoc.LoadXml(to_hstring(fileXML.c_str()));
                if (infoDoc.HasChildNodes()) {
                    auto target = infoDoc.SelectNodes(L"//Target");
                    if (target.Size() == 1)
                    {
                        _target = target.GetAt(0).InnerText();
                    }
                }
            }
        }
        catch (hresult_error const&)
        {
        }
        catch (...)
        {
        }
    }
}
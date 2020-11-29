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

const wstring XmlInfo::Target() const
{
    return _target.c_str();
}

const int XmlInfo::Port() const
{
    return _port;
}

const std::wstring XmlInfo::Message() const
{
    return _message;
}

void XmlInfo::Message(const std::wstring msg)
{
    _message = msg;
}

const std::wstring XmlInfo::URL() const
{
    return _url;
}

XmlInfo::XmlInfo(const wstring& filename)
    : _target(L"empty"), _port(3000), _message(L"Ready"), _url(L"localhost")
{
    if (filesystem::exists(filename))
    {
        try {
            ifstream toRead(filename);
            string fileXML(istreambuf_iterator<char>{toRead}, {});
            if (!fileXML.empty()) {
                
                XmlDocument infoDoc;
                infoDoc.LoadXml(to_hstring(fileXML.c_str()));
                if (infoDoc.HasChildNodes()) {
                    auto target = infoDoc.SelectNodes(L"//Target");
                    if (target.Size() == 1)
                    {
                        _target = target.GetAt(0).InnerText();
                    }
                    auto port = infoDoc.SelectNodes(L"//ServerPort");
                    if (port.Size() == 1)
                    {
                        StrToIntEx(port.GetAt(0).InnerText().c_str(), STIF_DEFAULT, &_port);
                    }
                    auto message = infoDoc.SelectNodes(L"//HTTPMessage");
                    if (message.Size() == 1)
                    {
                        _message = message.GetAt(0).InnerText();
                    }
                    auto url = infoDoc.SelectNodes(L"//URL");
                    if (url.Size() == 1)
                    {
                        _url = url.GetAt(0).InnerText();
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
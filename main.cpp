#include "gui.h"
#include "xmlInfo.h"
#include "httpServer.h"

using namespace winrt;

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    try
    {
        init_apartment();
        
        XmlInfo info;
        HTTPServer server(info);
        GUI gui(info);
    }
    catch (hresult_error const&)
    {
    }
    catch (...)
    {
    }

    return 0;
}



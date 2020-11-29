#include "httpServer.h"

using namespace winrt;

#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )    \
    do                                                      \
    {                                                       \
        RtlZeroMemory( (resp), sizeof(*(resp)) );           \
        (resp)->StatusCode = (status);                      \
        (resp)->pReason = (reason);                         \
        (resp)->ReasonLength = (USHORT) strlen(reason);     \
    } while (FALSE)

#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)               \
    do                                                               \
    {                                                                \
        (Response).Headers.KnownHeaders[(HeaderId)].pRawValue =      \
                                                          (RawValue);\
        (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength = \
            (USHORT) strlen(RawValue);                               \
    } while(FALSE)

#define ALLOC_MEM(cb) HeapAlloc(GetProcessHeap(), 0, (cb))

#define FREE_MEM(ptr) HeapFree(GetProcessHeap(), 0, (ptr))

bool HTTPServer::isOK() const
{
    return (_retCode == NO_ERROR);
}

ULONG HTTPServer::retCode() const
{
    return _retCode;
}

std::wstring HTTPServer::errDescription() const
{
    return _errDescription;
}

void HTTPServer::CleanUp()
{
    HttpRemoveUrl(_hReqQueue, _url.c_str());
    if (_hReqQueue)
    {
        CloseHandle(_hReqQueue);
    }
    HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
}

HTTPServer::~HTTPServer()
{
    CleanUp();
}

std::wstring HTTPServer::ErrorMessage(const std::wstring& header, const ULONG code)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = code;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)header.c_str()) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        header.c_str(), code, lpMsgBuf);

    std::wstring result = (LPCTSTR) lpDisplayBuf;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

    return result;
}

HTTPServer::HTTPServer(XmlInfo& info)
   : _retCode(NO_ERROR), _errDescription(L""), _hReqQueue(NULL), _url(L"http://localhost:3500")
{
	HTTPAPI_VERSION HttpApiVersion = HTTPAPI_VERSION_2;
    _url = info.URL();

    if (_retCode = HttpInitialize(HttpApiVersion, HTTP_INITIALIZE_SERVER, NULL); _retCode != NO_ERROR)
    {
        _errDescription = ErrorMessage(L"HttpInitialize", _retCode);
        info.Message(_errDescription);
        return;
    }
    
    if (_retCode = HttpCreateHttpHandle(&_hReqQueue, 0); _retCode != NO_ERROR)
    {
        _errDescription = ErrorMessage(L"HttpCreateHttpHandle", _retCode);
        info.Message(_errDescription);
        CleanUp();
        return;
    }

    if (_retCode = HttpAddUrl(_hReqQueue, _url.c_str(), NULL); _retCode != NO_ERROR)
    {
        _errDescription = ErrorMessage(L"HttpAddUrl", _retCode);
        info.Message(_errDescription);
        CleanUp();
        return;
    }

    info.Message(L"Listening to requests");
}


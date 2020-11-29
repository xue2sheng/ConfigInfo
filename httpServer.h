#pragma once

#include "xmlInfo.h"

class HTTPServer {
public:
	explicit HTTPServer(XmlInfo& info);
	virtual ~HTTPServer();

    bool isOK() const;
    ULONG retCode() const;
    std::wstring errDescription() const;

private:
    DWORD DoReceiveRequests(HANDLE hReqQueue);

    DWORD SendHttpResponse(
            IN HANDLE        hReqQueue,
            IN PHTTP_REQUEST pRequest,
            IN USHORT        StatusCode,
            IN PSTR          pReason,
            IN PSTR          pEntity
        );

    DWORD SendHttpPostResponse(
            IN HANDLE        hReqQueue,
            IN PHTTP_REQUEST pRequest
        );

    void CleanUp();
    std::wstring ErrorMessage(const std::wstring& header, const ULONG code);

    ULONG _retCode;
    std::wstring _errDescription;
    HANDLE _hReqQueue;
    std::wstring _url;
};
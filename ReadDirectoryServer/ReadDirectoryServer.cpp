#include "stdafx.h"
#include "ReadDirectoryServer.h"
#include "ReadDirectoryChanges.h"
#include "ReadDirectoryRequest/ReadDirectoryRequest.h"

namespace rdc
{

CReadDirectoryServer::CReadDirectoryServer(CReadDirectoryChanges* pHost) :
    m_pHost(pHost),
    m_hThread(NULL)
{

}

CReadDirectoryServer::~CReadDirectoryServer(void)
{
    _ASSERT(m_mapRequests.empty());
}

bool CReadDirectoryServer::Start(void)
{
    if (IsServiceRunning())
    {
        return true;
    }

    m_hThread = (HANDLE)_beginthreadex(NULL, 0, StartThreadProc, this, 0, NULL);
    return (m_hThread != NULL);
}

CReadDirectoryChanges* CReadDirectoryServer::GetHost(void)
{
    return m_pHost;
}

unsigned int CReadDirectoryServer::StartThreadProc(LPVOID pArg)
{
    CReadDirectoryServer* pServer = (CReadDirectoryServer*)pArg;
    _ASSERT(pServer);
    pServer->Run();
    return 0;
}

HANDLE CReadDirectoryServer::GetThreadHandle()
{
    return m_hThread;
}

void CReadDirectoryServer::CloseThreadHandle(void)
{
    if (IsServiceRunning())
    {
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

BOOL CReadDirectoryServer::IsServiceRunning(void)
{
    return (m_hThread != NULL);
}

bool CReadDirectoryServer::AddRequest(CReadDirectoryRequest* pRequest)
{    
    _ASSERT(pRequest);
    CString strDirectory = pRequest->GetDirectory();
    ReadDirectoryRequestMap::iterator it = m_mapRequests.find(strDirectory);
    if (it != m_mapRequests.end())
    {
        return false;
    }

    if (!pRequest->OpenDirectory())
    {
        return false;
    }

    m_mapRequests.insert(std::make_pair(strDirectory, pRequest));
    return true;
}

void CReadDirectoryServer::CancelRequest(const CString& strDirectory)
{
    ReadDirectoryRequestMap::iterator it = m_mapRequests.find(strDirectory);
    if (it != m_mapRequests.end())
    {
        ReadDirectoryRequestPtr pRequest = it->second;
        pRequest->CancelRequest(); // Asynchronous cancel IO
    }
}

void CReadDirectoryServer::CancelAllRequests(void)
{
    if (!HasOutstandingRequests())
    {
        return;
    }

    std::vector<CString> vecDirectory;
    for (auto it = m_mapRequests.begin(); it != m_mapRequests.end(); ++it)
    {
        vecDirectory.push_back(it->first);
    }
    for (size_t i = 0; i < vecDirectory.size(); ++i)
    {
        CancelRequest(vecDirectory[i]);
    }
}

void CReadDirectoryServer::RemoveRequest(CReadDirectoryRequest* pRequest)
{
    if (pRequest)
    {
        CString strDirectory = pRequest->GetDirectory();
        ReadDirectoryRequestMap::iterator it = m_mapRequests.find(strDirectory);
        if (it != m_mapRequests.end())
        {
            m_mapRequests.erase(it);
        }
    }
}

bool CReadDirectoryServer::HasOutstandingRequests(void)
{
    return !m_mapRequests.empty();
}

}
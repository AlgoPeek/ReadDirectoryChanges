#include "stdafx.h"
#include "IoCompletionPortServer.h"
#include "ReadDirectoryRequest/IoCompletionPortReqeust.h"

namespace rdc
{
CIoCompletionPortServer::CIoCompletionPortServer(CReadDirectoryChanges* pHost) :
    CReadDirectoryServer(pHost)
{
    m_iocp.Create();
}

CIoCompletionPortServer::~CIoCompletionPortServer(void)
{
    m_iocp.Close();
}

void CIoCompletionPortServer::Stop(void)
{
    if (!IsServiceRunning())
    {
        return;
    }

    m_iocp.PostStatus(KEY_TERMINITE, 0, NULL);
    ::WaitForSingleObject(GetThreadHandle(), INFINITE);
    CloseThreadHandle();
}

bool CIoCompletionPortServer::AddDirectory(const CString& strDirectory, BOOL bWatchSubtree,
    DWORD dwNotifyFilter, DWORD dwBufferSize)
{
    if (strDirectory.IsEmpty())
    {
        return false;
    }

    bool bRet = true;
    if (!IsServiceRunning()) 
    {
        bRet = Start();
    }
    if (bRet)
    {
        CIoCompletionPortRequest* pRequest = 
            new CIoCompletionPortRequest(this, strDirectory, bWatchSubtree, dwNotifyFilter, dwBufferSize);
        bRet = !!m_iocp.PostStatus(KEY_ADD_DIRECTORY, 0, pRequest);
        if (!bRet)
        {
            delete pRequest;
            pRequest = NULL;
        }
    }
    return bRet;
}

bool CIoCompletionPortServer::RemoveDirectory(const CString& strDirectory)
{
    if (strDirectory.IsEmpty())
    {
        return false;
    }

    if (!IsServiceRunning())
    {
        return false;
    }

    RemoveDirectoryIocpArg* pArg = new RemoveDirectoryIocpArg(this, strDirectory);
    if (!pArg)
    {
        return false;
    }
    return !!m_iocp.PostStatus(KEY_REMOVE_DIRECTORY, 0, pArg);
}

void CIoCompletionPortServer::Run()
{
    while (1)
    {
        ULONG_PTR pCompKey = NULL;
        DWORD dwNumberOfBytes = 0;
        OVERLAPPED* pOverlapped = NULL;
        BOOL bRet = m_iocp.GetStatus(&pCompKey, &dwNumberOfBytes, &pOverlapped);
        DWORD dwLastError = ::GetLastError();
        if (bRet)
        {
            ProcessIocpSuccess(pCompKey, dwNumberOfBytes, pOverlapped);
        }
        else
        {
            if (!ProcessIocpError(dwLastError, pOverlapped))
            {
                break;
            }
        }
    }
}

void CIoCompletionPortServer::ProcessIocpSuccess(ULONG_PTR pCompKey, DWORD dwNumberOfBytes, 
    OVERLAPPED* pOverlapped)
{
    if (pCompKey == KEY_ADD_DIRECTORY)
    {
        CReadDirectoryRequest* pRequest = (CReadDirectoryRequest*)pOverlapped;
        ProcessKeyAddDirectory(pRequest);
    }
    else if (pCompKey == KEY_READ_DIRECTORY)
    {
        CReadDirectoryRequest* pRequest = (CReadDirectoryRequest*)pOverlapped;
        ProcessKeyReadDirectory(pRequest, dwNumberOfBytes);
    }
    else if (pCompKey == KEY_REMOVE_DIRECTORY)
    {
        RemoveDirectoryIocpArg* pArg = (RemoveDirectoryIocpArg*)pOverlapped;
        ProcessKeyRemoveDirectory(pArg);
    }
    else if (pCompKey == KEY_TERMINITE)
    {
        ProcessKeyTerminite();
    }
    else
    {
        _ASSERT(false);
    }
}

bool CIoCompletionPortServer::ProcessIocpError(DWORD dwLastError, OVERLAPPED* pOverlapped)
{
    bool bRet = true;
    if (pOverlapped)
    {
        if (dwLastError == ERROR_OPERATION_ABORTED)
        {
            CReadDirectoryRequest* pRequest = (CReadDirectoryRequest*)pOverlapped;
            _ASSERT(pRequest);
            RemoveRequest(pRequest);
            if (!HasOutstandingRequests())
            {
                bRet = false;
            }
        }
    }
    else
    {
        if (dwLastError != WAIT_TIMEOUT)
        {
            _ASSERT(false);
        }
    }
    return bRet;
}

void CIoCompletionPortServer::ProcessKeyAddDirectory(CReadDirectoryRequest* pRequest)
{
    if (!pRequest)
    {
        _ASSERT(false);
        return;
    }

    if (AddRequest(pRequest))
    {
        m_iocp.AssociateDevice(pRequest->GetDirectoryHandle(), KEY_READ_DIRECTORY);
        pRequest->BeginReqeust();
    }  
    else
    {
        delete pRequest;
        pRequest = NULL;
    }
}
void CIoCompletionPortServer::ProcessKeyReadDirectory(CReadDirectoryRequest* pRequest,
    DWORD dwNumberOfBytes)
{
    if (!pRequest)
    {
        _ASSERT(false);
        return;
    }

    if (!dwNumberOfBytes)
    {
        return;
    }
    pRequest->BackupBuffer(dwNumberOfBytes);
    pRequest->BeginReqeust();
    pRequest->ProcessNotification();
}

void CIoCompletionPortServer::ProcessKeyRemoveDirectory(RemoveDirectoryIocpArg* pArg)
{
    if (!pArg)
    {
        _ASSERT(false);
        return;
    }

    ON_LEAVE_1(delete pArg, RemoveDirectoryIocpArg*, pArg);
    CancelRequest(pArg->strDirectory);
}

void CIoCompletionPortServer::ProcessKeyTerminite(void)
{
    CancelAllRequests();
}

}
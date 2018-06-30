#include "stdafx.h"
#include "CompletionRoutineServer.h"
#include "ReadDirectoryRequest/CompletionRoutineRequest.h"

namespace rdc
{

CCompletionRoutineServer::CCompletionRoutineServer(CReadDirectoryChanges* pHost) :
    CReadDirectoryServer(pHost),
    m_bTerminate(false)
{

}

CCompletionRoutineServer::~CCompletionRoutineServer(void)
{

}

void CCompletionRoutineServer::Stop(void)
{
    if (!IsServiceRunning())
    {
        return;
    }

    m_bTerminate = true;
    ::QueueUserAPC(CCompletionRoutineServer::TerminateProc, 
        GetThreadHandle(), (ULONG_PTR)this);
    ::WaitForSingleObject(GetThreadHandle(), INFINITE);
    CloseThreadHandle();
}

bool CCompletionRoutineServer::AddDirectory(const CString& strDirectory, BOOL bWatchSubtree,
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
        CReadDirectoryRequest* pRequest =
            new CCompletionRoutineRequest(this, strDirectory, bWatchSubtree, dwNotifyFilter, dwBufferSize);
        bRet = !!::QueueUserAPC(CCompletionRoutineServer::AddDirectoryProc, 
            GetThreadHandle(), (ULONG_PTR)pRequest);
        if (!bRet)
        {
            delete pRequest;
            pRequest = NULL;
        }
    }
    return bRet;
}

bool CCompletionRoutineServer::RemoveDirectory(const CString& strDirectory)
{
    if (strDirectory.IsEmpty())
    {
        return false;
    }

    if (!IsServiceRunning())
    {
        return false;
    }

    RemoveDirectoryApcArg* pArg = new RemoveDirectoryApcArg(this, strDirectory);
    if (!pArg)
    {
        return false;
    }

    bool bRet = !!::QueueUserAPC(CCompletionRoutineServer::AddDirectoryProc,
        GetThreadHandle(), (ULONG_PTR)pArg);
    if (!bRet)
    {
        delete pArg;
        pArg = NULL;
    }
    return bRet;
}

void CCompletionRoutineServer::Run()
{
    while (!m_bTerminate || HasOutstandingRequests())
    {
        ::SleepEx(INFINITE, true);
    }
}

void CCompletionRoutineServer::AddDirectoryProc(ULONG_PTR pArg)
{
    CReadDirectoryRequest* pRequest = (CReadDirectoryRequest*)pArg;
    if (pRequest)
    {
        CCompletionRoutineServer* pServer =
            dynamic_cast<CCompletionRoutineServer*>(pRequest->GetServer());
        _ASSERT(pServer);
        if (pServer->AddRequest(pRequest))
        {
            pRequest->BeginReqeust();
        }
        else
        {
            delete pRequest;
            pRequest = NULL;
        }
    }   
}

void CCompletionRoutineServer::RemoveDirectoryProc(ULONG_PTR pArg)
{
    RemoveDirectoryApcArg* pRemoveDirectoryArg = (RemoveDirectoryApcArg*)pArg;
    if (pRemoveDirectoryArg)
    {
        ON_LEAVE_1(delete pRemoveDirectoryArg, RemoveDirectoryApcArg*, pRemoveDirectoryArg);
        CCompletionRoutineServer* pServer = pRemoveDirectoryArg->pServer;
        _ASSERT(pServer);
        pServer->CancelRequest(pRemoveDirectoryArg->strDirectory);
    }
}

void CCompletionRoutineServer::TerminateProc(ULONG_PTR pArg)
{
    CCompletionRoutineServer* pServer = (CCompletionRoutineServer*)pArg;
    if (pServer)
    {
        pServer->CancelAllRequests();
    }
}

}
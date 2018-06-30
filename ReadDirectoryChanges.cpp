#include "stdafx.h"
#include "ReadDirectoryChanges.h"
#include "ReadDirectoryServer/CompletionRoutineServer.h"
#include "ReadDirectoryServer/IoCompletionPortServer.h"

namespace rdc
{

CReadDirectoryChanges::CReadDirectoryChanges(EIoModelType eIoModel /*= eIoModel_CompletionRoutine*/,
    int nMaxChanges /*= 4096*/) :
    m_queue(nMaxChanges)
{
    if (eIoModel == eIoModel_CompletionRoutine)
    {
        m_pServer.reset(new CCompletionRoutineServer(this));
    }
    else if (eIoModel == eIoModel_IoCompletionPort)
    {
        m_pServer.reset(new CIoCompletionPortServer(this));
    }
    else
    {
        // Unsupported IO Model, use default
        _ASSERT(false);
        m_pServer.reset(new CCompletionRoutineServer(this));
    }
}

CReadDirectoryChanges::~CReadDirectoryChanges()
{
    Terminate();
}

bool CReadDirectoryChanges::Init()
{
    return m_pServer->Start();
}

void CReadDirectoryChanges::Terminate()
{
    m_pServer->Stop();
}

bool CReadDirectoryChanges::AddDirectory(const CString& strDirectory, BOOL bWatchSubtree,
    DWORD dwNotifyFilter, DWORD dwBufferSize)
{
    return m_pServer->AddDirectory(strDirectory, bWatchSubtree, dwNotifyFilter, dwBufferSize);
}

bool CReadDirectoryChanges::RemoveDirectory(const CString& strDirectory)
{
    return m_pServer->RemoveDirectory(strDirectory);
}

bool CReadDirectoryChanges::Pop(DWORD& dwAction, CString& strFilename)
{
    TDirectoryChangeNotification pair;
    if (!m_queue.pop(pair))
    {
        return false;
    }

    dwAction = pair.first;
    strFilename = pair.second;
    return true;
}

void CReadDirectoryChanges::Push(DWORD dwAction, CString& strFilename)
{
    TDirectoryChangeNotification dcn(dwAction, strFilename);
    m_queue.push(dcn);
}

bool CReadDirectoryChanges::CheckOverflow()
{
    bool bRet = m_queue.overflow();
    if (bRet)
    {
        m_queue.clear();
    }
    return bRet;
}

HANDLE CReadDirectoryChanges::GetWaitHandle()
{
    return m_queue.GetWaitHandle();
}

}
#include "stdafx.h"
#include "CompletionRoutineRequest.h"

namespace rdc
{

CCompletionRoutineRequest::CCompletionRoutineRequest(CReadDirectoryServer* pServer, 
    const CString& strDirectory, BOOL bWatchSubTree, DWORD dwNotifyFilter, DWORD dwBufSize) :
    CReadDirectoryRequest(pServer, strDirectory, bWatchSubTree, dwNotifyFilter, dwBufSize)
{
    hEvent = this;
}

CCompletionRoutineRequest::~CCompletionRoutineRequest()
{

}

bool CCompletionRoutineRequest::BeginReqeust(void)
{
    if (!IsDirectoryOpened())
    {
        return false;
    }

    // This call needs to be reissued after every APC.
    BOOL success = ::ReadDirectoryChangesW(
        GetDirectoryHandle(),			    // handle to directory
        GetBuffer(),                        // read results buffer
        GetBufferSize(),                    // length of buffer
        IsWatchSubTree(),                   // monitoring option
        GetNotifyFilter(),                  // filter conditions
        NULL,                               // bytes returned
        this,                               // overlapped buffer
        &FileIoCompletionRoutine);          // completion routine
    return !!success;
}

VOID CCompletionRoutineRequest::FileIoCompletionRoutine(DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
    CCompletionRoutineRequest* pRequest = (CCompletionRoutineRequest*)lpOverlapped->hEvent;
    _ASSERT(pRequest);

    if (dwErrorCode == ERROR_OPERATION_ABORTED)
    {
        pRequest->RemoveRequest();
        return;
    }

    // Can't use sizeof(FILE_NOTIFY_INFORMATION) because
    // the structure is padded to 16 bytes.
    _ASSERTE(dwNumberOfBytesTransfered >= offsetof(FILE_NOTIFY_INFORMATION, FileName) + sizeof(WCHAR));

    // This might mean overflow? Not sure.
    if (!dwNumberOfBytesTransfered)
    {
        return;
    }

    pRequest->BackupBuffer(dwNumberOfBytesTransfered);

    // Get the new read issued as fast as possible. The documentation
    // says that the original OVERLAPPED structure will not be used
    // again once the completion routine is called.
    pRequest->BeginReqeust();

    pRequest->ProcessNotification();
}

}
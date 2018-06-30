#include "stdafx.h"
#include "IoCompletionPortReqeust.h"

namespace rdc
{

CIoCompletionPortRequest::CIoCompletionPortRequest(CReadDirectoryServer* pServer,
    const CString& strDirectory, BOOL bWatchSubTree, DWORD dwNotifyFilter, DWORD dwBufSize) :
    CReadDirectoryRequest(pServer, strDirectory, bWatchSubTree, dwNotifyFilter, dwBufSize)
{
    
}

CIoCompletionPortRequest::~CIoCompletionPortRequest(void)
{

}

bool CIoCompletionPortRequest::BeginReqeust(void)
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
        NULL);                              // completion routine
    return !!success;
}

}
#pragma once

#include "ReadDirectoryRequest.h"

namespace rdc
{

class CCompletionRoutineRequest :
    public CReadDirectoryRequest
{
public:
    CCompletionRoutineRequest(CReadDirectoryServer* pServer, const CString& strDirectory,
        BOOL bWatchSubTree, DWORD dwNotifyFilter, DWORD dwBufSize);
    virtual ~CCompletionRoutineRequest();

    virtual bool BeginReqeust(void) OVERRIDE;
private:
    static VOID CALLBACK FileIoCompletionRoutine(DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
};

}
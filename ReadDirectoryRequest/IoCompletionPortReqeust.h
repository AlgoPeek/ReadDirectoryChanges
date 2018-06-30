#pragma once

#include "ReadDirectoryRequest.h"

namespace rdc
{

class CIoCompletionPortRequest :
    public CReadDirectoryRequest
{
public:
    CIoCompletionPortRequest(CReadDirectoryServer* pServer, const CString& strDirectory, 
        BOOL bWatchSubTree, DWORD dwNotifyFilter, DWORD dwBufSize);
    ~CIoCompletionPortRequest(void);

    virtual bool BeginReqeust(void) OVERRIDE;
};

}
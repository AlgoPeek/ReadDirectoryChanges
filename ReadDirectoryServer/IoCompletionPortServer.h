#pragma once

#include "ReadDirectoryServer.h"

namespace rdc
{
class CIoCompletionPortServer :
    public CReadDirectoryServer
{
    struct RemoveDirectoryIocpArg;
public:
    CIoCompletionPortServer(CReadDirectoryChanges* pHost);
    virtual ~CIoCompletionPortServer(void);

    virtual void Stop(void) OVERRIDE;
    virtual bool AddDirectory(const CString& strDirectory, BOOL bWatchSubtree,
        DWORD dwNotifyFilter, DWORD dwBufferSize) OVERRIDE;
    virtual bool RemoveDirectory(const CString& strDirectory) OVERRIDE;
protected:
    virtual void Run() OVERRIDE;
private:
    // running in work server thread
    void ProcessIocpSuccess(ULONG_PTR pCompKey, DWORD dwNumberOfBytes, OVERLAPPED* pOverlapped);
    bool ProcessIocpError(DWORD dwLastError, OVERLAPPED* pOverlapped);
    void ProcessKeyAddDirectory(CReadDirectoryRequest* pRequest);
    void ProcessKeyReadDirectory(CReadDirectoryRequest* pRequest, DWORD dwNumberOfBytes);
    void ProcessKeyRemoveDirectory(RemoveDirectoryIocpArg* pArg);
    void ProcessKeyTerminite(void);
private:
    struct RemoveDirectoryIocpArg :
        public OVERLAPPED
    {
        CIoCompletionPortServer* pServer;
        CString strDirectory;
        RemoveDirectoryIocpArg(CIoCompletionPortServer* server, const CString& directory) :
            pServer(server), strDirectory(directory) 
        {
            ZeroMemory(this, sizeof(OVERLAPPED));
        }
    };

    CIOCP m_iocp;

    const ULONG_PTR KEY_ADD_DIRECTORY = 1;
    const ULONG_PTR KEY_READ_DIRECTORY = 2;
    const ULONG_PTR KEY_REMOVE_DIRECTORY = 3;
    const ULONG_PTR KEY_TERMINITE = 4;
};

}
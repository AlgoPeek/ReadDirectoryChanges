#pragma once

namespace rdc
{

class CReadDirectoryChanges;
class CReadDirectoryRequest;
class CReadDirectoryServer
{
    typedef std::shared_ptr<CReadDirectoryRequest> ReadDirectoryRequestPtr;
    typedef std::map<CString, ReadDirectoryRequestPtr> ReadDirectoryRequestMap;
    friend class CReadDirectoryRequest;
public:
    CReadDirectoryServer(CReadDirectoryChanges* pHost);
    virtual ~CReadDirectoryServer(void);
    
    virtual bool Start(void);
    virtual void Stop(void) = 0;
    virtual bool AddDirectory(const CString& strDirectory, BOOL bWatchSubtree, 
        DWORD dwNotifyFilter, DWORD dwBufferSize) = 0;
    virtual bool RemoveDirectory(const CString& strDirectory) = 0;
    CReadDirectoryChanges* GetHost(void);
protected:
    static unsigned int CALLBACK StartThreadProc(LPVOID pArg);
    virtual void Run() = 0;

    HANDLE GetThreadHandle(void);
    void CloseThreadHandle(void);
    BOOL IsServiceRunning(void);
    bool AddRequest(CReadDirectoryRequest* pRequest);
    void CancelRequest(const CString& strDirectory);
    void CancelAllRequests(void);
    void RemoveRequest(CReadDirectoryRequest* pRequest);
    bool HasOutstandingRequests(void);
private:
    CReadDirectoryChanges* m_pHost;
    HANDLE m_hThread;

    // Can only access at work thread 
    ReadDirectoryRequestMap m_mapRequests;
};

}
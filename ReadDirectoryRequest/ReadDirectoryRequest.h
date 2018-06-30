#pragma once

namespace rdc
{

class CReadDirectoryServer;
class CReadDirectoryRequest :
    public OVERLAPPED
{
public:
    CReadDirectoryRequest(CReadDirectoryServer* pServer, const CString& strDirectory,
        BOOL bWatchSubTree, DWORD dwNotifyFilter, DWORD dwBufSize);
    virtual ~CReadDirectoryRequest();

    virtual bool OpenDirectory();
    virtual bool BeginReqeust(void) = 0;
    virtual void CancelRequest(void);
    CReadDirectoryServer* GetServer(void);
    HANDLE GetDirectoryHandle(void);
    CString GetDirectory(void);
    void BackupBuffer(DWORD dwSize);
    void ProcessNotification();
protected:
    bool IsDirectoryOpened(void) const;
    void RemoveRequest(void);

    BYTE* GetBuffer(void);
    DWORD GetBufferSize(void);
    BOOL IsWatchSubTree(void);
    DWORD GetNotifyFilter(void);
private:
    CReadDirectoryServer* m_pServer;
    HANDLE m_hDirectory;

    CString m_strDirectory;
    BOOL m_bWatchSubTree;
    DWORD m_dwNotitfyFilter;
    std::vector<BYTE> m_vecBuffer;
    std::vector<BYTE> m_vecBackupBuffer;
};

}

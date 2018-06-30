#pragma once

namespace rdc
{

enum EIoModelType
{
    eIoModel_CompletionRoutine,
    eIoModel_IoCompletionPort,
};

class CReadDirectoryServer;
class CReadDirectoryChanges
{
public:
    CReadDirectoryChanges(EIoModelType eIoModel = eIoModel_CompletionRoutine,
        int nMaxChanges = 4096);
    ~CReadDirectoryChanges();

    bool Init();
    void Terminate();
    bool AddDirectory(const CString& strDirectory, BOOL bWatchSubtree, DWORD dwNotifyFilter, 
        DWORD dwBufferSize);
    bool RemoveDirectory(const CString& strDirectory);
    bool Pop(DWORD& dwAction, CString& strFilename);
    void Push(DWORD dwAction, CString& strFilename);
    bool CheckOverflow();
    HANDLE GetWaitHandle();
private:
    typedef std::pair<DWORD, CString> TDirectoryChangeNotification;

    std::shared_ptr<CReadDirectoryServer> m_pServer;
    CThreadSafeQueue<TDirectoryChangeNotification> m_queue;
};

}
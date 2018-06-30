// ReadDirectoryChanges.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ReadDirectoryChanges.h"

LPCWSTR ExplainAction(DWORD dwAction);
bool TryGetKeyboardInput(HANDLE hStdIn, bool &bTerminate, char* buf, int len);

//
// When the application starts, it immediately starts monitoring your home
// directory, including children, as well as C:\, not including children.
// The application exits when you hit Esc.
// You can add a directory to the monitoring list by typing the directory
// name and hitting Enter. Notifications will pause while you type.
//

int main()
{
    using namespace rdc;

    EIoModelType eIoModel = eIoModel_CompletionRoutine;
    DWORD dwMaxChanges = 4096;
    DWORD dwBufSize = 16 * 1024;
    const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_LAST_WRITE
        | FILE_NOTIFY_CHANGE_CREATION
        | FILE_NOTIFY_CHANGE_FILE_NAME
        | FILE_NOTIFY_CHANGE_DIR_NAME;

    // Create the monitor and add two directories.
    CReadDirectoryChanges changes(eIoModel, dwMaxChanges);
    //changes.AddDirectory(_tgetenv(_T("USERPROFILE")), true, dwNotificationFlags);
    changes.AddDirectory(_T("C:\\"), true, dwNotificationFlags, dwBufSize);

    HANDLE hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
    const HANDLE handles[] = { hStdIn, changes.GetWaitHandle() };

    char buf[MAX_PATH] = { 0 };
    bool bTerminate = false;
    while (!bTerminate)
    {
        DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, INFINITE, true);
        switch (rc)
        {
        case WAIT_OBJECT_0 + 0:
            // hStdIn was signaled. This can happen due to mouse input, focus change,
            // Shift keys, and more.  Delegate to TryGetKeyboardInput().
            // TryGetKeyboardInput sets bTerminate to true if the user hits Esc.
            if (TryGetKeyboardInput(hStdIn, bTerminate, buf, MAX_PATH))
            {
                changes.AddDirectory(CStringW(buf), false, dwNotificationFlags, dwBufSize);
            }
            break;
        case WAIT_OBJECT_0 + 1:
            // We've received a notification in the queue.
            {
                DWORD dwAction;
                CString strFilename;
                if (changes.CheckOverflow())
                {
                    wprintf(L"Queue overflowed.\n");
                }
                else
                {
                    changes.Pop(dwAction, strFilename);
                    wprintf(L"%s %s\n", ExplainAction(dwAction), strFilename.GetBuffer());
                }
            }
            break;
        case WAIT_IO_COMPLETION:
            // Nothing to do.
            break;
        }
    }

    // Just for sample purposes. The destructor will
    // call Terminate() automatically.
    changes.Terminate();
    return EXIT_SUCCESS;
}


LPCWSTR ExplainAction(DWORD dwAction)
{
    switch (dwAction)
    {
    case FILE_ACTION_ADDED:
        return L"[Add]";
    case FILE_ACTION_REMOVED:
        return L"[Delete]";
    case FILE_ACTION_MODIFIED:
        return L"[Modified]";
    case FILE_ACTION_RENAMED_OLD_NAME:
        return L"[Renamed From]";
    case FILE_ACTION_RENAMED_NEW_NAME:
        return L"[Renamed To]";
    default:
        return L"[BAD DATA]";
    }
}

bool TryGetKeyboardInput(HANDLE hStdIn, bool &bTerminate, char* buf, int len)
{
    DWORD dwNumberOfEventsRead = 0;
    INPUT_RECORD rec = { 0 };

    if (!::PeekConsoleInput(hStdIn, &rec, 1, &dwNumberOfEventsRead))
    {
        return false;
    }

    if (rec.EventType == KEY_EVENT)
    {
        if (rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
        {
            bTerminate = true;
        }
        else if (rec.Event.KeyEvent.wVirtualKeyCode > VK_HELP)
        {
            if (!gets_s(buf, len))	// End of file, usually Ctrl-Z
            {
                bTerminate = true;
            }
            else
            {
                return true;
            }
        }
    }

    ::FlushConsoleInputBuffer(hStdIn);
    return false;
}
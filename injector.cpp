#include "injector.h"
#include <QDebug>

Injector::Injector()
{

}

int Injector::Inject()
{
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tokenPriv;
    LUID luidDebug;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE)
    {
        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug) != FALSE)
        {
            tokenPriv.PrivilegeCount = 1;
            tokenPriv.Privileges[0].Luid = luidDebug;
            tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            if (AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, 0, NULL, NULL) != FALSE)
            {
                // Always successful, even in the cases which lead to OpenProcess failure
                qDebug() << "SUCCESSFULLY CHANGED TOKEN PRIVILEGES";
            }
            else
            {
                qDebug() << "FAILED TO CHANGE TOKEN PRIVILEGES, CODE: " << GetLastError();
            }
        }
    }
    CloseHandle(hToken);

    //Getting pID and dllPath
    int PID = this->pID;
    LPCSTR DllPath = this->dllPath.toStdString().c_str();
    DWORD hLibModule;


    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (hProcess == 0)
    {
        return ErrorCode::E_OPEN_PROCESS_FAILED;
    }

    LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath) + 1,
        MEM_COMMIT, PAGE_READWRITE);

    if (pDllPath == 0)
    {
        return ErrorCode::E_MEM_ALLOC_FAILED;
    }

    WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath,
        strlen(DllPath) + 1, 0);

    HANDLE hLoadThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
            "LoadLibraryA"), pDllPath, 0, 0);

    if (hLoadThread == nullptr)
    {
        return ErrorCode::E_CREATE_REMOTE_THREAD_FAILED;
        ::CloseHandle(hLoadThread);
    }

    WaitForSingleObject(hLoadThread, INFINITE);

    ::GetExitCodeThread(hLoadThread, &hLibModule);

    //std::cout << "Dll path allocated at: " << pDllPath << std::endl;
    this->pdll = pDllPath;

    ::CloseHandle(hLoadThread);
    VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);

    return ErrorCode::E_SUCCESS;
}

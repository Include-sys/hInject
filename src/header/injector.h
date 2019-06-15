#ifndef INJECTOR_H
#define INJECTOR_H

#include "Windows.h"
#include <QString>



class Injector
{
public:
    DWORD pID;
    QString dllPath;
    //ErrorCode error;
    LPVOID pdll;
    enum ErrorCode{
        E_SUCCESS = 1,
        E_OPEN_PROCESS_FAILED = 0,
        E_MEM_ALLOC_FAILED = -1,
        E_CREATE_REMOTE_THREAD_FAILED = -2,
    };

public:
    Injector();
    int Inject();

};

#endif // INJECTOR_H

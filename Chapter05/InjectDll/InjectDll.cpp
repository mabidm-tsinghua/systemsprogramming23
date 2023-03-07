/******************************************************************************
Module:  InjLib.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre
******************************************************************************/


#include "stdafx.h"     




///////////////////////////////////////////////////////////////////////////////


#ifdef UNICODE
#define InjectLib InjectLibW
#define EjectLib  EjectLibW
#else
#define InjectLib InjectLibA
#define EjectLib  EjectLibA
#endif   // !UNICODE


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {

    BOOL bOk = FALSE; // Assume that the function fails
    HANDLE hProcess = NULL, hThread = NULL;
    PWSTR pszLibFileRemote = NULL;

    __try {
        // Get a handle for the target process.
        hProcess = OpenProcess(
            //The access to the process object
            PROCESS_QUERY_INFORMATION |   // Required to retrieve certain information about a process, such as its token, exit code, and priority class
            PROCESS_CREATE_THREAD |   // For CreateRemoteThread
            PROCESS_VM_OPERATION |   // For VirtualAllocEx/VirtualFreeEx
            PROCESS_VM_WRITE,             // For WriteProcessMemory
            FALSE, //handle cannot be inherited
            dwProcessId);
        if (hProcess == NULL) { 
            ReportException(_T("OpenProcess failed\n"), 1);
            __leave; }

        // Calculate the number of bytes needed for the DLL's pathname
        int cch = 1 + lstrlenW(pszLibFile);//len in characters
        int cb = cch * sizeof(wchar_t);//len in bytes

        // Allocate space in the remote process for the dll pathname
        pszLibFileRemote = (PWSTR)
            VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
        if (pszLibFileRemote == NULL) { 
            ReportException(_T("VirtualAllocEx failed\n"), 1); 
            __leave; }

        // Copy the DLL's pathname to the remote process' address space
        if (!WriteProcessMemory(hProcess, pszLibFileRemote,
            (PVOID)pszLibFile, cb, NULL)) {
            ReportException(_T("WriteProcessMemory failed\n"), 1); 
            __leave;
        }

        // Get the real address of LoadLibraryW in Kernel32.dll
        PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
            GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
        if (pfnThreadRtn == NULL) { 
            ReportException(_T("GetProcAddress failed\n"), 1);
            __leave; }

        // Create a remote thread that calls LoadLibraryW(DLLPathname)
        hThread = CreateRemoteThread(hProcess, NULL, 0,
            pfnThreadRtn, pszLibFileRemote, 0, NULL);
        if (hThread == NULL) { 
            ReportException(_T("CreateRemoteThread failed\n"), 1); 
           __leave; }

        // Wait for the remote thread to terminate
        WaitForSingleObject(hThread, INFINITE);

        bOk = TRUE; // Everything executed successfully
    }
    __finally { // Now, we can clean everything up

       // Free the remote memory that contained the DLL's pathname
        if (pszLibFileRemote != NULL)
            VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

        if (hThread != NULL)
            CloseHandle(hThread);

        if (hProcess != NULL)
            CloseHandle(hProcess);
    }

    return(bOk);
}


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI InjectLibA(DWORD dwProcessId, PCSTR pszLibFile) {

    // Allocate a (stack) buffer for the Unicode version of the pathname
    SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
    PWSTR pszLibFileW = (PWSTR)
        _alloca(cchSize * sizeof(wchar_t));

    // Convert the ANSI pathname to its Unicode equivalent
    StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);

    // Call the Unicode version of the function to actually do the work.
    return(InjectLibW(dwProcessId, pszLibFileW));
}


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI EjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {

    BOOL bOk = FALSE; // Assume that the function fails
    HANDLE hthSnapshot = NULL;
    HANDLE hProcess = NULL, hThread = NULL;

    __try {
        // Grab a new snapshot of the process
        hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
        if (hthSnapshot == INVALID_HANDLE_VALUE) __leave;

        /* //The modBaseAddr and hModule members are valid only in the context of the process specified by th32ProcessID.
        typedef struct tagMODULEENTRY32 {
    DWORD   dwSize;//size of this struc
    DWORD   th32ModuleID;//no longer used, and is always set to one.
    DWORD   th32ProcessID;//The identifier of the process whose modules are to be examined.
    DWORD   GlblcntUsage;//not used
    DWORD   ProccntUsage;//not used
    BYTE    *modBaseAddr;//The base address of the module in the context of the owning process.
    DWORD   modBaseSize;//The size of the module, in bytes.
    HMODULE hModule;//A handle to the module in the context of the owning process.
    char    szModule[MAX_MODULE_NAME32 + 1];//The module name.
    char    szExePath[MAX_PATH];//The module path
  } MODULEENTRY32;*/

  // Get the HMODULE of the desired library
        MODULEENTRY32W me = { sizeof(me) };
        BOOL bFound = FALSE;
        //Retrieves information about the first module associated with a process.
        BOOL bMoreMods = Module32FirstW(hthSnapshot, &me);
        for (; bMoreMods; bMoreMods = Module32NextW(hthSnapshot, &me)) {
            //Performs a case-insensitive comparison of strings
            bFound = (_wcsicmp(me.szModule, pszLibFile) == 0) ||
                (_wcsicmp(me.szExePath, pszLibFile) == 0);
            if (bFound) break;
        }
        if (!bFound) __leave;

        // Get a handle for the target process.
        hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION |
            PROCESS_CREATE_THREAD |
            PROCESS_VM_OPERATION,  // For CreateRemoteThread
            FALSE, dwProcessId);
        if (hProcess == NULL) __leave;

        // Get the real address of FreeLibrary in Kernel32.dll
        PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
            GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "FreeLibrary");
        if (pfnThreadRtn == NULL) __leave;

        // Create a remote thread that calls FreeLibrary()
        hThread = CreateRemoteThread(hProcess, NULL, 0,
            pfnThreadRtn, me.modBaseAddr, 0, NULL);
        if (hThread == NULL) __leave;

        // Wait for the remote thread to terminate
        WaitForSingleObject(hThread, INFINITE);

        bOk = TRUE; // Everything executed successfully
    }
    __finally { // Now we can clean everything up

        if (hthSnapshot != NULL)
            CloseHandle(hthSnapshot);

        if (hThread != NULL)
            CloseHandle(hThread);

        if (hProcess != NULL)
            CloseHandle(hProcess);
    }

    return(bOk);
}


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI EjectLibA(DWORD dwProcessId, PCSTR pszLibFile) {

    // Allocate a (stack) buffer for the Unicode version of the pathname
    SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
    PWSTR pszLibFileW = (PWSTR)
        _alloca(cchSize * sizeof(wchar_t));

    // Convert the ANSI pathname to its Unicode equivalent
    StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);

    // Call the Unicode version of the function to actually do the work.
    return(EjectLibW(dwProcessId, pszLibFileW));
}


///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////


void inject(DWORD pid) {

    
        DWORD dwProcessId = pid;
        if (dwProcessId == 0) {
            // A process ID of 0 causes everything to take place in the 
            // local process; this makes things easier for debugging.
            dwProcessId = GetCurrentProcessId();
        }

        TCHAR szLibFile[MAX_PATH];
        /*Retrieves the fully qualified path for the file that contains the 
        specified module. The module must have been loaded by the current
        process.*/
        /*_countof is implemented as a function-like preprocessor macro.
        The C++ version has extra template machinery to detect at compile
        time if a pointer is passed instead of a statically declared array.
        Ensure that array is actually an array, not a pointer. In C,
        _countof produces erroneous results if array is a pointer. 
        In C++, _countof fails to compile if array is a pointer.*/
        //retrieves the path of the executable file of the current process
        GetModuleFileName(NULL, szLibFile, _countof(szLibFile)); //retrieves the path of the executable file of the current process
        //Scans a string for the last occurrence of a character.
        PTSTR pFilename = _tcsrchr(szLibFile, TEXT('\\')) + 1;
        //complete filename of dll
        _tcscpy_s(pFilename, _countof(szLibFile) - (pFilename - szLibFile),//(pFilename - szLibFile) is the number of chars used
            TEXT("cci_fDll.DLL"));
        if (InjectLib(dwProcessId, szLibFile)) {
            if(EjectLib(dwProcessId, szLibFile))
             _tprintf(_T("DLL Injection/Ejection successful."));
        }
        else {
            _tprintf(_T("DLL Injection/Ejection failed."));
        }
      
    
}


///////////////////////////////////////////////////////////////////////////////


int _tmain(int argc, LPTSTR argv[]) {

    if (argc < 2)
        ReportException(_T("Usage: InjectDll pid"), 1);
    inject(_ttoi(argv[1]));
    
    return(0);
}


//////////////////////////////// End of File //////////////////////////////////

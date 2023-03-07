// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"



inline void display(PCSTR szMsg) {
    char szTitle[MAX_PATH];
    GetModuleFileNameA(NULL, szTitle, _countof(szTitle));
    printf("Title: %s, Message: %s \n", szTitle,szMsg);
   // _tprintf(_T("DLL ..."));
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if  (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
	
        char szBuf[MAX_PATH * 100] = { 0 };

        PBYTE pb = NULL;
        MEMORY_BASIC_INFORMATION mbi;
        while (VirtualQuery(pb, &mbi, sizeof(mbi)) == sizeof(mbi)) {

            int nLen;
            char szModName[MAX_PATH];

            if (mbi.State == MEM_FREE)
                mbi.AllocationBase = mbi.BaseAddress;

            if ((mbi.AllocationBase == hModule) ||
                (mbi.AllocationBase != mbi.BaseAddress) ||
                (mbi.AllocationBase == NULL)) {
                // Do not add the module name to the list
                // if any of the following is true:
                // 1. If this region contains this DLL
                // 2. If this block is NOT the beginning of a region
                // 3. If the address is NULL
                nLen = 0;
            }
            else {
                nLen = GetModuleFileNameA((HINSTANCE)mbi.AllocationBase,
                    szModName, _countof(szModName));
            }

            if (nLen > 0) {
                wsprintfA(strchr(szBuf, 0), "\n%p-%s",
                    mbi.AllocationBase, szModName);
            }

            pb += mbi.RegionSize;
        }

        // NOTE: Normally, you should not display a message box in DllMain
        // due to the loader lock described in Chapter 20. However, to keep
        // this sample application simple, I am violating this rule.
        display(&szBuf[1]);
    }
	return TRUE;
}


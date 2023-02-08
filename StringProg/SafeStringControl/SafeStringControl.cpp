// UnSafeString.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"






VOID getMessage(TCHAR* szInput) {

    TCHAR szData1[10];//this will overrun into following buffer
    
    
     //typedef long HRESULT

    //HRESULT StringCchCopy(PTSTR pszDest, size_t cchDest, PCTSTR pszSrc);
    //return val STRSAFE_E_INSUFFICIENT_BUFFER means: copy with truncation; SUCCEEDED ret false for it 
    //ret val S_OK means everything is fine
   
    

    if (SUCCEEDED(StringCchCopy(szData1, _countof(szData1), szInput))) { //we just need 10 characters
        _tprintf(_T("All data copied\n"));
        _tprintf(_T("Complete Data : %s\n"), szData1);
    }
    else {
        _tprintf(_T("Not All data copied\n"));
        _tprintf(_T("Partial Data : %s\n"), szData1);
    
    }
    //print
    
    

    

}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc != 2) {
        _tprintf(_T("Enter message (Only 9 characters will stored)\n"));
        return -1;
    }

    getMessage(argv[1]);

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

// UnSafeString.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

#define PASSWORD "UxoaT7x"

bool exploit(TCHAR *szInput) {
    
    TCHAR szData1[10];//this will overrun into following buffer
    TCHAR szData2[8];

    //
    _tcscpy(szData2, _T(PASSWORD)); //8
    _tcscpy(szData1, szInput); //szData1 is of 10 char
    //print
    _tprintf(_T("szData1 value is: %s\n"), szData1);
    _tprintf(_T("szData2 value is: %s\n"), szData2);

    return _tcsncmp(szData1, szData2,9); //0 means equal

}

int _tmain(int argc, TCHAR *argv[])
{
    if (argc != 2) {
        _tprintf(_T("Please provide the password as an argument (maximum 9 characters)\n"));
        return -1;    
    }

    if (exploit(argv[1]) == 0) {
        _tprintf(_T("Correct Password: %s"), _T(PASSWORD));
    }else {
        _tprintf(_T("Incorrect Password!"));
    }

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

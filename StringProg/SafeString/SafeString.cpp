// UnSafeString.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

#define PASSWORD "UxoaT7x"


/*
* Ref: windows via c/c++
All of the secure (_s) functions validate their arguments as the first thing they do. Checks are 
performed to make sure that pointers are not NULL, that integers are within a valid range, that 
enumeration values are valid, and that buffers are large enough to hold the resulting data. 
If any of these checks fail, the functions set the thread-local C run-time variable errno and the 
function returns an errno_t value to indicate success or failure. However, these functions don't
actually return; instead, in a debug build, they display a user-unfriendly assertion dialog box 
similar to that shown in Figure 2-1. Then your application is terminated. The release builds 
directly auto-terminate.

*/

bool exploit(TCHAR* szInput) {

    TCHAR szData1[10];//this will overrun into following buffer
    TCHAR szData2[8];

    //
    _tcscpy_s(szData2,_countof(szData2), _T(PASSWORD)); //secure fun
    _tcscpy_s(szData1, _countof(szData1), szInput); //secure
    //print
    _tprintf(_T("szData1 value is: %s\n"), szData1);
    _tprintf(_T("szData2 value is: %s\n"), szData2);

    return _tcsncmp(szData1, szData2, 9); //0 means equal

}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc != 2) {
        _tprintf(_T("Please provide the password as and argument (maximum 9 characters)\n"));
        return -1;
    }

    if (exploit(argv[1]) == 0) {
        _tprintf(_T("Correct Password: %s"), _T(PASSWORD));
    }
    else {
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

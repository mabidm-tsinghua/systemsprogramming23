// UnSafeString.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "StringReverse.h"


#define DATA "My name is Muhammad Abid"

int _tmain(int argc, TCHAR* argv[])
{
    TCHAR data[] = _T(DATA);
    //data[_countof(data) - 1] = _T('Z');
    _tprintf(_T("Before reverse: %s\n"),data);
    StringReverse(data,_countof(data));
    //StringReverse(data, 100);
    _tprintf(_T("After reverse: %s\n"), data);
    
    return 0;

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

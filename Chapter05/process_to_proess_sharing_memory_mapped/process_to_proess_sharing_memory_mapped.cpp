/******************************************************************************
Module:  MMFShare.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre
******************************************************************************/


#include "stdafx.h"
#define CREATE_FILE_MAP 1
#define CLOSE_FILE_MAP 2
#define OPEN_FILE_MAP 3
#define RETURN 4
void OnCommand(int command);

///////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, LPTSTR argv[]) {
    int command = 4;
    _tprintf(_T("\n\t 1 to create file mapping object." \
                                "\n\t 2 to close file mapping object." \
                                "\n\t 3 to Read shared data " \
                                "\n\t 4 to return "));
    _tprintf(_T("\nEnter command: "));
    _tscanf(_T("%d"),&command);
    /*  Call the function. */
    
    while (TRUE) __try{
        if (command == RETURN) { return 2;}
        OnCommand(command);
        _tprintf(_T("\nEnter command: "));
        _tscanf(_T("%d"), &command);
    }__except (EXCEPTION_EXECUTE_HANDLER) {
        _tprintf(_T("\nException Generated: "));
        _tprintf(_T("\nEnter command: "));
        _tscanf(_T("%d"), &command);
    }

    
    return 0;
}



///////////////////////////////////////////////////////////////////////////////



void OnCommand(int command) {

    // Handle of the open memory-mapped file
    static HANDLE s_hFileMap = NULL;

    switch (command) {
    
    case CREATE_FILE_MAP:
        
        // Create a paging file-backed MMF to contain the edit control text.
        // The MMF is 4 KB at most and is named MMFSharedData.
        s_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
            PAGE_READWRITE, 0, 4 * 1024, TEXT("MMFSharedData"));

        if (s_hFileMap != NULL) {

            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                _tprintf(_T("Mapping already exists - not created."));
                CloseHandle(s_hFileMap);

            }
            else {

               // File mapping created successfully.

                // Map a view of the file into the address space.
                LPTSTR pView = (LPTSTR)MapViewOfFile(s_hFileMap,
                    FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

                if (pView != NULL) {
                    // Put text into the MMF.
                    _stprintf(pView, _T("%s"), _T("Writing Data to MMFSharedData"));
                    _tprintf(_T("Data Written\n"));
                    // Protect the MMF storage by unmapping it.
                    UnmapViewOfFile(pView);
                }
                else {
                    ReportException(_T("Can't map view of file."),1);
                }
            }

        }
        else {
            ReportException(_T("Can't create file mapping."),1);
        }
        break;

    case CLOSE_FILE_MAP:
        
        if (CloseHandle(s_hFileMap)) { _tprintf(_T("File mapping object closed \n")); };
        break;

    case OPEN_FILE_MAP:
        
        // See if a memory-mapped file named MMF SharedData already exists.
        HANDLE hFileMapT = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
            FALSE, TEXT("MMFSharedData"));

        if (hFileMapT != NULL) {
            // The MMF does exist, map it into the process's address space.
            LPTSTR pView = (LPTSTR)MapViewOfFile(hFileMapT,
                FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

            if (pView != NULL) {

                // Put the contents of the MMF into the edit control.
                _tprintf(_T(" Reading Data: %s\n"),pView);
                UnmapViewOfFile(pView);
            }
            else {
                ReportException(_T("Can't map view of file."), 1);
            }

            CloseHandle(hFileMapT);

        }
        else {
            ReportException(_T("Can't Open file mapping."), 1);
        }
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// End of File //////////////////////////////////
/*#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define BUFFER_SIZE 1024
#define COPY_SIZE 512*/

/*
   MyCopyMemory - A wrapper for CopyMemory

   buf     - destination buffer
   pbData  - source buffer
   cbData  - size of block to copy, in bytes
   bufsize - size of the destination buffer
*/
/*
void MyCopyMemory(TCHAR* buf, TCHAR* pbData, SIZE_T cbData, SIZE_T bufsize)
{
    CopyMemory(buf, pbData, min(cbData, bufsize));
}

void main()
{
    TCHAR buf[BUFFER_SIZE] = TEXT("This is the destination");
    TCHAR pbData[BUFFER_SIZE] = TEXT("This is the source");

    MyCopyMemory(buf, pbData, COPY_SIZE * sizeof(TCHAR), BUFFER_SIZE * sizeof(TCHAR));

    _tprintf(TEXT("Destination buffer contents: %s\n"), buf);
}*/

/*#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256
TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[]=TEXT("Message from first process.");

int _tmain()
{
   HANDLE hMapFile;
   LPCTSTR pBuf;

   hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 BUF_SIZE,                // maximum object size (low-order DWORD)
                 szName);                 // name of mapping object

   if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not create file mapping object (%d).\n"),
             GetLastError());
      return 1;
   }
   pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        BUF_SIZE);

   if (pBuf == NULL)
   {
      _tprintf(TEXT("Could not map view of file (%d).\n"),
             GetLastError());

       CloseHandle(hMapFile);

      return 1;
   }


   CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
    _getch();

   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);

   return 0;
}*/
/*#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
TCHAR szName[]=TEXT("Global\\MyFileMappingObject");

int _tmain()
{
   HANDLE hMapFile;
   LPCTSTR pBuf;

   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not open file mapping object (%d).\n"),
             GetLastError());
      return 1;
   }

   pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,
               0,
               BUF_SIZE);

   if (pBuf == NULL)
   {
      _tprintf(TEXT("Could not map view of file (%d).\n"),
             GetLastError());

      CloseHandle(hMapFile);

      return 1;
   }

   MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);

   return 0;
}*/
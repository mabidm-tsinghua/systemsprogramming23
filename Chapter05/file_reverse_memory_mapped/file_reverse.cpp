
#include "stdafx.h"
BOOL FileReverse(LPCTSTR outFile, PBOOL pbIsTextUnicode);

int _tmain(int argc, LPTSTR argv[]) {
    if (argc < 2)
        ReportException(_T("Usage: file_reverse filename"), 1);
    BOOL bIsTextUnicode;
    TCHAR out_file[256];
    _stprintf(out_file, _T("%s_reverse"), argv[1]);
    if (!CopyFile(argv[1], out_file, FALSE)) {
        ReportException(_T("New file could not be created."),1);
    }
    if (FileReverse(out_file, &bIsTextUnicode)) {
        _tprintf(_T("Type of File: %s"), bIsTextUnicode ? TEXT("Unicode") : TEXT("ANSI")        );

        // Spawn Notepad to see the fruits of our labors.
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        TCHAR sz[256];
        _stprintf(sz, _T("Notepad %s"), out_file);
       if (CreateProcess(NULL, sz,
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

    return 0;
}

BOOL FileReverse(LPCTSTR inFile, PBOOL pbIsTextUnicode) {

    *pbIsTextUnicode = FALSE;  // Assume text is ANSI

    // Open the file for reading and writing.
    HANDLE hFile = CreateFile(inFile, GENERIC_WRITE | GENERIC_READ, 0,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        ReportException(_T("File could not be opened."),1);
        return(FALSE);
    }

    // Get the size of the file (I assume the whole file can be mapped).
    DWORD dwFileSize = GetFileSize(hFile, NULL);

    // Create the file-mapping object. The file-mapping object is 1 character 
    // bigger than the file size so that a zero character can be placed at the 
    // end of the file to terminate the string (file). Because I don't yet know
    // if the file contains ANSI or Unicode characters, I assume worst case
    // and add the size of a WCHAR instead of CHAR.
    HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
        0, dwFileSize + sizeof(WCHAR), NULL);

    if (hFileMap == NULL) {
        ReportException(_T("File map could not be opened."),1);
        CloseHandle(hFile);
        return(FALSE);
    }

    // Get the address where the first byte of the file is mapped into memory.
    PVOID pvFile = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

    if (pvFile == NULL) {
        ReportException(_T("Could not map view of file."),1);
        CloseHandle(hFileMap);
        CloseHandle(hFile);
        return(FALSE);
    }

    // Does the buffer contain ANSI or Unicode?
    int iUnicodeTestFlags = -1;   // Try all tests
    *pbIsTextUnicode = IsTextUnicode(pvFile, dwFileSize, &iUnicodeTestFlags);

    if (!*pbIsTextUnicode) {
        // For all the file manipulations below, we explicitly use ANSI 
        // functions because we are processing an ANSI file.

        // Put a zero character at the very end of the file.
        PSTR pchANSI = (PSTR)pvFile;
        pchANSI[dwFileSize / sizeof(CHAR)] = 0;

        // Reverse the contents of the file.
        _strrev(pchANSI);

        // Convert all "\n\r" combinations back to "\r\n" to 
        // preserve the normal end-of-line sequence.
        pchANSI = strstr(pchANSI, "\n\r"); // Find first "\r\n".

        while (pchANSI != NULL) {
            // We have found an occurrence....
            *pchANSI++ = '\r';   // Change '\n' to '\r'.
            *pchANSI++ = '\n';   // Change '\r' to '\n'.
            pchANSI = strstr(pchANSI, "\n\r"); // Find the next occurrence.
        }

    }
    else {
        // For all the file manipulations below, we explicitly use Unicode
        // functions because we are processing a Unicode file.

        // Put a zero character at the very end of the file.
        PWSTR pchUnicode = (PWSTR)pvFile;
        pchUnicode[dwFileSize / sizeof(WCHAR)] = 0;

        if ((iUnicodeTestFlags & IS_TEXT_UNICODE_SIGNATURE) != 0) {
            // If the first character is the Unicode BOM (byte-order-mark), 
            // 0xFEFF, keep this character at the beginning of the file.
            pchUnicode++;
        }

        // Reverse the contents of the file.
        _wcsrev(pchUnicode);

        // Convert all "\n\r" combinations back to "\r\n" to 
        // preserve the normal end-of-line sequence.
        pchUnicode = wcsstr(pchUnicode, L"\n\r"); // Find first '\n\r'.

        while (pchUnicode != NULL) {
            // We have found an occurrence....
            *pchUnicode++ = L'\r';   // Change '\n' to '\r'.
            *pchUnicode++ = L'\n';   // Change '\r' to '\n'.
            pchUnicode = wcsstr(pchUnicode, L"\n\r"); // Find the next occurrence.
        }
    }

    // Clean up everything before exiting.
    UnmapViewOfFile(pvFile);
    CloseHandle(hFileMap);
    /*If CreateFileMapping is called to create a file mapping object for hFile,
    UnmapViewOfFile must be called first to unmap all views and call CloseHandle
    to close the file mapping object before you can call SetEndOfFile.*/
    
    // Remove trailing zero character added earlier.
    SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);//Moves the file pointer of the specified file.
    SetEndOfFile(hFile);//Sets the physical file size for the specified file to the current position of the file pointer.
    CloseHandle(hFile);

    return(TRUE);
}


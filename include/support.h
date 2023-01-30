
#pragma once

#define EMPTY _T ("")
#define YES _T ("y")
#define NO _T("n")
#define CR _T('\r')
#define LF _T('\n')
#define TSIZE sizeof (TCHAR)

/* Limits and constants. */

#define TYPE_FILE 1		/* Used in ls, rm, and lsFP */
#define TYPE_DIR 2
#define TYPE_DOT 3

#define MAX_OPTIONS 20	/* Max # command line options */
#define MAX_ARG 1000	/* Max # of command line arguments */
#define MAX_COMMAND_LINE MAX_PATH+50 /* Max size of a command line */
#define MAX_PATH_LONG 32767  /* Very long path names with \\?\ prefix. */
#define MAX_NAME 256    /* Name length - users and groups */

/* Commonly used functions. */

 BOOL ConsolePrompt (LPCTSTR, LPTSTR, DWORD, BOOL);
 BOOL PrintStrings (HANDLE, ...);
 BOOL PrintMsg (HANDLE, LPCTSTR);
 VOID ReportError (LPCTSTR, DWORD, BOOL);
 VOID ReportException (LPCTSTR, DWORD);
 DWORD Options (int, LPCTSTR *, LPCTSTR, ...);
 LPTSTR SkipArg (LPTSTR, int, int, LPTSTR argv[]);
 BOOL WindowsVersionOK (DWORD, DWORD);
 VOID GetArgs (LPCTSTR, int *, LPTSTR *);




#ifdef _UNICODE /* This declaration had to be added. */
#define _tstrrchr wcsrchr
#else
#define _tstrrchr strrchr
#endif

#ifdef _UNICODE /* This declaration had to be added. */
#define _tstrstr wcsstr
#else
#define _tstrstr strstr
#endif

#ifdef _UNICODE /* This declaration had to be added. */
#define _memtchr wmemchr
#else
#define _memtchr memchr
#endif



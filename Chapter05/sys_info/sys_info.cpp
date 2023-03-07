/* Chapter 6. version. Display system version information  */
#include "stdafx.h"

int _tmain (int argc, LPTSTR argv[])
{
	OSVERSIONINFO OSVer;
	SYSTEM_INFO SysInfo;
	DWORD SysAffinMask = 0, ProcAffinMask = 0;
	DWORD sectorsPerCluster, bytesPerSector, numberOfFreeClusters, totalNumberOfClusters;
/* For refereence -- this is what the OSVERSIONINFO structure looks like.
typedef struct _OSVERSIONINFO{ 
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    TCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO; 
*/ 

	/* Determine version information.  */
	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx (&OSVer)) 
		_tprintf (_T("Cannot get OS Version info. %d"), GetLastError());

	_tprintf (_T("Major Version:      %d\n"), OSVer.dwMajorVersion);
	_tprintf (_T("Minor Version:      %d\n"), OSVer.dwMinorVersion);
	_tprintf (_T("Build Number:       %d\n"), OSVer.dwBuildNumber);
	_tprintf (_T("Platform ID:        %d\n"), OSVer.dwPlatformId);
	_tprintf (_T("Platorm is NT?(0/1) %d\n"), (OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT)); //specify Windows NT or operating systems descended from it
	_tprintf (_T("Service Pack:       %s\n"), OSVer.szCSDVersion);
	_tprintf (_T("\n"));

/* For reference -- Here is the SYSTEM_INFO structure.
typedef struct _SYSTEM_INFO { // sinf 
    union { 
        DWORD  dwOemId;  //// Obsolete field...do not use
        struct { 
            WORD wProcessorArchitecture; //use this; 0 means PROCESSOR_ARCHITECTURE_INTEL
            WORD wReserved; 
        }; 
    }; 
    DWORD  dwPageSize; 
    LPVOID lpMinimumApplicationAddress; 
    LPVOID lpMaximumApplicationAddress; 
    DWORD  dwActiveProcessorMask; 
    DWORD  dwNumberOfProcessors; 
    DWORD  dwProcessorType; 
    DWORD  dwAllocationGranularity; 
    WORD  wProcessorLevel; 
    WORD  wProcessorRevision; 
} SYSTEM_INFO; 
*/
	GetSystemInfo (&SysInfo);
	_tprintf (_T("OEM Id:             %d\n"), SysInfo.dwOemId); //retained for compatibility
	_tprintf (_T("Processor Arch:     %d\n"), SysInfo.wProcessorArchitecture); // 0 for x86; 9 for x64 (AMD or Intel)
	_tprintf (_T("Page Size:          %x\n"), SysInfo.dwPageSize);
	_tprintf (_T("Min appl addr:      %p\n"), SysInfo.lpMinimumApplicationAddress);//lowest VA for apps and dlls; depends on build win32 or x64
	_tprintf (_T("Max appl addr:      %p\n"), SysInfo.lpMaximumApplicationAddress);//highest VA for apps and dlls;depends on build win32 or x64
	/*A mask representing the set of processors configured into the system. Bit 0 is processor 0;bit 31 is processor 31.*/
	_tprintf (_T("ActiveProcMask:     %x\n"), SysInfo.dwActiveProcessorMask);//also include thread per core
	_tprintf (_T("Number processors:  %d\n"), SysInfo.dwNumberOfProcessors);//number of logical processors.
	/*An obsolete member that is retained for compatibility. Use the wProcessorArchitecture, wProcessorLevel, and wProcessorRevision members to determine the type of processor*/
	_tprintf (_T("Processor type:     %d\n"), SysInfo.dwProcessorType);
	/*The granularity for the starting address at which virtual memory can be allocated. See VirtualAlloc()*/
	_tprintf (_T("Alloc grnrty:       %x\n"), SysInfo.dwAllocationGranularity);
	_tprintf (_T("Processor level:    %d\n"), SysInfo.wProcessorLevel);
	_tprintf (_T("Processor rev:      %d\n"), SysInfo.wProcessorRevision);
	

	_tprintf (_T("\n"));
	/*
	A process affinity mask is a bit vector in which each bit represents the processors that a process is allowed to run on. 
	A system affinity mask is a bit vector in which each bit represents the processors that are configured into a system.
	A process affinity mask is a subset of the system affinity mask. A process is only allowed to run on the processors configured into a system.
	Therefore, the process affinity mask cannot specify a 1 bit for a processor when the system affinity mask specifies a 0 bit for that processor.
	*/
	//also check out SetProcessAffinityMask
	GetProcessAffinityMask (GetCurrentProcess(), (PDWORD_PTR)&ProcAffinMask, (PDWORD_PTR)&SysAffinMask);
	_tprintf (_T("Sys  Affinity Mask  %x\n"), SysAffinMask);
	_tprintf (_T("Proc Affinity Mask  %x\n"), ProcAffinMask);

	/* Now get Disk information (for the current disk) */
	/* Caution: the MSND page suggests using GetDiskFreeSpaceEx for volumes larger than 2GB, which would be nearly any */
	/* hard disk volume. However, the Ex function does not return the same cluster and sector information. */
	/* Regardless, this seems to work properly under Vista and W7 on a large disk (640 GB was the largest I tried). */
	GetDiskFreeSpace(NULL, &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters);
	_tprintf (_T("\n"));
	_tprintf (_T("Information about current disk.\n"));
	_tprintf (_T("Sectors per cluster:      %d.\n"), sectorsPerCluster);
	_tprintf (_T("Bytes per sector:         %d.\n"), bytesPerSector);
	_tprintf (_T("Number of free clusters:  %d.\n"), numberOfFreeClusters);
	_tprintf (_T("Total number of clusters: %d.\n"), totalNumberOfClusters);

	return 0;
}

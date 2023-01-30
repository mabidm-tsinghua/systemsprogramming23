#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

//following three lines added to build application for windows 7 version 6.1


#include <WinSDKVer.h>
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

//if we don't define above three lines then follwoing line will choose version 6.2 as windows platform
#include <SDKDDKVer.h>


//When you use the Windows SDK, you can specify which versions of Windows your code can run on
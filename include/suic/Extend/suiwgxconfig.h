
#ifndef _SUIWGXCONFIG_H_
#define _SUIWGXCONFIG_H_

#define __OS_WIN32

#ifdef _MSC_VER
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4482) // enum
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4748) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4251) // function or variable may be unsafe (deprecated)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#endif // _MSC_VER
#ifdef __BORLANDC__
#pragma option -w-8027		   // function not expanded inline
#endif

#pragma comment(lib, "msimg32.lib")

#ifndef SUIWGX_LIB

#ifdef SUIWGX_EXPORTS
#define SUIWGX_API __declspec(dllexport)
#else
#define SUIWGX_API __declspec(dllimport)
#endif

#else

#define SUIWGX_API

#endif

#include <windows.h>
#include <commctrl.h>
#include <stddef.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>

#endif
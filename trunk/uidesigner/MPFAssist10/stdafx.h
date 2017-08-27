// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>

#include <Config\suicoreinc.h>
#include <Extend\suiwgxinc.h>

#pragma warning( disable : 4278 )
#pragma warning( disable : 4146 )
	//The following #import imports the IDTExtensibility2 interface
	#import <MSADDNDR.DLL> raw_interfaces_only named_guids

	//The following #import imports VS Command Bars
	#import <vsmso.olb> raw_interfaces_only named_guids

	//The following #import imports DTE
	#import <dte80a.olb> raw_interfaces_only rename("LONG_PTR","LONG_PTRDTE") rename("ULONG_PTR","ULONG_PTRDTE") 

	//The following #import imports DTE80
	#import <dte80.olb> raw_interfaces_only named_guids rename("LONG_PTR","LONG_PTRDTE") rename("ULONG_PTR","ULONG_PTRDTE") 

	//The following #import imports DTE90
	#import <dte90.olb> raw_interfaces_only named_guids

	//The following #import imports DTE90a
	#import <dte90a.olb> raw_interfaces_only named_guids

	//The following #import imports DTE100
	#import <dte100.olb> raw_interfaces_only named_guids

#pragma warning( default : 4146 )
#pragma warning( default : 4278 )

#define IfFailGo(x) { hr=(x); if (FAILED(hr)) goto Error; }
#define IfFailGoCheck(x, p) { hr=(x); if (FAILED(hr)) goto Error; if(!p) {hr = E_FAIL; goto Error; } }

class DECLSPEC_UUID("E1202578-F265-4699-9E25-216B4583962A") MPFAssistLib;

using namespace ATL;

class CAddInModule : public CAtlDllModuleT< CAddInModule >
{
public:
	CAddInModule()
	{
		m_hInstance = NULL;
	}

	DECLARE_LIBID(__uuidof(MPFAssistLib))

	inline HINSTANCE GetResourceInstance()
	{
		return m_hInstance;
	}

	inline void SetResourceInstance(HINSTANCE hInstance)
	{
		m_hInstance = hInstance;
	}

	virtual HRESULT AddCommonRGSReplacements(_Inout_ IRegistrarBase* pRegistrar) throw()
	{
		TCHAR strPath[256];

		strPath[0] = _T('\0');
		::GetModuleFileName(m_hInstance, strPath, 255);

		for (int i = _tcslen(strPath); i >= 0; --i)
		{
			if (strPath[i] == _T('\\') || strPath[i] == _T('/'))
			{
				strPath[i + 1] = _T('\0');
				break;
			}
		}

		pRegistrar->AddReplacement(_T("MODULE_DIR"), strPath);
		return CAtlDllModuleT< CAddInModule >::AddCommonRGSReplacements(pRegistrar);
	}

private:
	HINSTANCE m_hInstance;
};

extern CAddInModule _AtlModule;
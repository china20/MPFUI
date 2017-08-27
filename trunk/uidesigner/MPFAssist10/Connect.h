// Connect.h : Declaration of the CConnect

#pragma once
#include "resource.h"       // main symbols

#include <uidcore/uidframe.h>

using namespace AddInDesignerObjects;
using namespace EnvDTE;
using namespace EnvDTE80;
using namespace Microsoft_VisualStudio_CommandBars;

// 按钮信息结构体  
struct stCommandInfo  
{
    LPCTSTR m_id;
    LPCTSTR m_name; // 按钮名字  
    LPCTSTR m_strTip; // 按钮提示  
    BOOL    m_hasIcon; // 是否有图标  
    UINT   m_bitmapID; // 对应的位图ID  
    UINT   m_cmdID; // 命令ID  
};

/// <summary>The object for implementing an Add-in.</summary>
/// <seealso class='IDTExtensibility2' />
class ATL_NO_VTABLE CConnect : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnect, &CLSID_Connect>,

    public IDispatchImpl<EnvDTE::IDTCommandTarget, &__uuidof(EnvDTE::IDTCommandTarget), &EnvDTE80::LIBID_EnvDTE80, 8, 0>,
	public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects, 1, 0>
    //public CComObjectRootEx<CComSingleThreadModel>,
    //public CComCoClass<CConnect, &CLSID_Connect>,
    //public IDispatchImpl<EnvDTE::IDTCommandTarget, &__uuidof(EnvDTE::IDTCommandTarget), &EnvDTE::LIBID_EnvDTE, 8, 0>,
    //public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects, 1, 0>
{
public:
	/// <summary>Implements the constructor for the Add-in object. Place your initialization code within this method.</summary>
	CConnect()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ADDIN)
DECLARE_NOT_AGGREGATABLE(CConnect)


BEGIN_COM_MAP(CConnect)
	COM_INTERFACE_ENTRY2(IDispatch, IDTExtensibility2)
    COM_INTERFACE_ENTRY(EnvDTE::IDTCommandTarget)
	COM_INTERFACE_ENTRY(IDTExtensibility2)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:
//IDTExtensibility2 implementation:

	/// <summary>Implements the OnConnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being loaded.</summary>
	/// <param term='application'>Root object of the host application.</param>
	/// <param term='connectMode'>Describes how the Add-in is being loaded.</param>
	/// <param term='addInInst'>Object representing this Add-in.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnConnection)(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch *AddInInst, SAFEARRAY **custom);

	/// <summary>Implements the OnDisconnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being unloaded.</summary>
	/// <param term='disconnectMode'>Describes how the Add-in is being unloaded.</param>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY **custom );

	/// <summary>Implements the OnAddInsUpdate method of the IDTExtensibility2 interface. Receives notification when the collection of Add-ins has changed.</summary>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />	
	STDMETHOD(OnAddInsUpdate)(SAFEARRAY **custom );

	/// <summary>Implements the OnStartupComplete method of the IDTExtensibility2 interface. Receives notification that the host application has completed loading.</summary>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnStartupComplete)(SAFEARRAY **custom );

	/// <summary>Implements the OnBeginShutdown method of the IDTExtensibility2 interface. Receives notification that the host application is being unloaded.</summary>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnBeginShutdown)(SAFEARRAY **custom );
	
    STDMETHOD(QueryStatus) (
        /*[in]*/ BSTR CmdName,
        /*[in]*/ enum vsCommandStatusTextWanted NeededText,
        /*[in,out]*/ enum vsCommandStatus * StatusOption,
        /*[in,out]*/ VARIANT * CommandText );
    STDMETHOD(Exec) (
        /*[in]*/ BSTR CmdName,
        /*[in]*/ enum vsCommandExecOption ExecuteOption,
        /*[in]*/ VARIANT * VariantIn,
        /*[in,out]*/ VARIANT * VariantOut,
        /*[in,out]*/ VARIANT_BOOL * Handled );

    suic::String GetCmdName(LPCTSTR name);
    suic::String GetActiveFilePath();

public:

    void InitUIDesigner();

    void UnregisterCommand(CComPtr<EnvDTE::Commands>& pCommands, LPCWSTR commandName);
    HRESULT FindToolbarByName(_CommandBars* pCommandBars, LPCWSTR szToolbarName, CommandBar** pOut);
    HRESULT InitCommandBar(IDispatch *pApplication, ext_ConnectMode ConnectMode, IDispatch *pAddInInst);

private:
	CComPtr<DTE2> m_pDTE;
	CComPtr<AddIn> m_pAddInInstance;

    UIDFramePtr _uiFrame;
};

OBJECT_ENTRY_AUTO(__uuidof(Connect), CConnect)

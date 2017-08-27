// Connect.cpp : Implementation of CConnect
#include "stdafx.h"
#include "AddIn.h"
#include "Connect.h"

extern CAddInModule _AtlModule;

static LPCTSTR TOOLBAR_NAME = _T("MPFAssist");
static LPCTSTR MODULE_NAME = _T("MPFAssist.Connect.");

static LPCTSTR UI_OPENFILEID = _T("OpenFile");
static LPCTSTR UI_OPENMAINWNDID = _T("OpenMainWnd");
static LPCTSTR UI_OPENSETTINGID = _T("OpenSetting");

static LPCTSTR UI_OPENFILENAME = _T("F");
static LPCTSTR UI_OPENMAINWNDNAME = _T("M");
static LPCTSTR UI_OPENSETTINGNAME = _T("S");

static int UI_OPENFILECMD = 8001;
static int UI_OPENMAINWNDCMD = 8002;
static int UI_OPENSETTINGCMD = 8003;

static int UI_OPENFILE_BMP = 101;
static int UI_OPENMAINWND_BMP = 103;
static int UI_OPENSETTING_BMP = 102;

static stCommandInfo s_commandList[] = {
    {
        UI_OPENFILEID,
        UI_OPENFILENAME,            // 按钮名字  
        _T("使用MPF编辑当前文件"),  // 按钮提示  
        TRUE,                       // 是否有图标  
        UI_OPENFILE_BMP,            // 对应的位图ID  
        UI_OPENFILECMD,             // 命令ID  
    },
    {
        UI_OPENMAINWNDID,
        UI_OPENMAINWNDNAME,         // 按钮名字  
        _T("打开MPF设计界面"),      // 按钮提示  
        TRUE,                       // 是否有图标  
        UI_OPENMAINWND_BMP,         // 对应的位图ID  
        UI_OPENMAINWNDCMD,          // 命令ID  
    },
    {
        UI_OPENSETTINGID,
        UI_OPENSETTINGNAME,         // 按钮名字  
        _T("打开MPF助手设置"),      // 按钮提示  
        TRUE,                       // 是否有图标  
        UI_OPENSETTING_BMP,         // 对应的位图ID  
        UI_OPENSETTINGCMD,          // 命令ID  
    },
};

extern HINSTANCE _hInstance;

// CConnect
STDMETHODIMP CConnect::OnConnection(IDispatch *pApplication, ext_ConnectMode ConnectMode, IDispatch *pAddInInst, SAFEARRAY ** /*custom*/ )
{
	HRESULT hr = S_OK;
	pApplication->QueryInterface(__uuidof(DTE2), (LPVOID*)&m_pDTE);
	pAddInInst->QueryInterface(__uuidof(AddIn), (LPVOID*)&m_pAddInInstance);

    InitCommandBar(pApplication, ConnectMode, pAddInInst);
	
	return hr;
}

STDMETHODIMP CConnect::OnDisconnection(ext_DisconnectMode /*RemoveMode*/, SAFEARRAY ** /*custom*/ )
{
    if (NULL != _uiFrame.get())
    {
        _uiFrame = NULL;
        //
        // MPF环境清理
        //
        suic::ExitUIWgx();
    }

	m_pDTE = NULL;
	m_pAddInInstance = NULL;

	return S_OK;
}

suic::String CConnect::GetCmdName(LPCTSTR name)
{
    suic::String strName = _U("MPFAssist.Connect.");
    strName += name;
    return strName;
}

suic::String CConnect::GetActiveFilePath()
{
    suic::String strPath;
    EnvDTE::Document* pDoc = NULL;

    if (m_pDTE != NULL)
    {
        m_pDTE->get_ActiveDocument(&pDoc);

        if (NULL != pDoc)
        {
            CComBSTR bsFullName; 
            pDoc->get_FullName(&bsFullName);
            strPath = (LPCTSTR)bsFullName;
        }
    }

    return strPath;
}

void CConnect::InitUIDesigner()
{
    if (m_pDTE != NULL && _uiFrame.get() == NULL)
    {
        //
        // 初始化MPF环境
        //
        suic::InitUIWgx();
        suic::InitUILogLevel(suic::LogLevel::Trace);

        //
        // 获取设计器核心接口
        //
        UIDGetFrame(_uiFrame);
        _uiFrame->SetUIDMode(IUIDFrame::UIDMode::umAddin);
    }
}

STDMETHODIMP CConnect::OnAddInsUpdate (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnStartupComplete (SAFEARRAY ** /*custom*/ )
{
    EnvDTE::Events* pEvt = NULL;
    EnvDTE::_SolutionEventsPtr solEvt;

    m_pDTE->get_Events(&pEvt);
    if (NULL != pEvt)
    {
        pEvt->get_SolutionEvents(&solEvt);
        pEvt->Release();
    }

    InitUIDesigner();

	return S_OK;
}

STDMETHODIMP CConnect::OnBeginShutdown (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::QueryStatus (
        /*[in]*/ BSTR CmdName,
        /*[in]*/ enum vsCommandStatusTextWanted NeededText,
        /*[in,out]*/ enum vsCommandStatus * StatusOption,
        /*[in,out]*/ VARIANT * CommandText )
{
    suic::String strName(CmdName);

    InitUIDesigner();

    if (strName.Equals(GetCmdName(UI_OPENFILEID)))
    {
        suic::String strPath = GetActiveFilePath();
        strPath.ToLower();
        if (strPath.IndexOf(_U(".xml")) == -1 && strPath.IndexOf(_U(".xaml")) == -1)
        {
            *StatusOption = (vsCommandStatus)0;//(vsCommandStatus)(vsCommandStatus::vsCommandStatusSupported);
            return S_OK;
        }
    }
    
    *StatusOption = (vsCommandStatus)(vsCommandStatus::vsCommandStatusEnabled | vsCommandStatus::vsCommandStatusSupported);

    return S_OK;
}

static void OpenAndSelectFile(const suic::String& strPath)
{
    suic::String strSelect;
    suic::String strTemp = strPath;

    strTemp.Replace(_U("/"), _U("\\"));
    strTemp.Replace(_U("\\\\"), _U("\\"));
    strSelect.Format(_U("/select,%s"), strTemp.c_str()) ;

    ShellExecute(NULL, _U("open"), _U("explorer.exe"), strSelect.c_str(), NULL, SW_NORMAL) ;
}

STDMETHODIMP CConnect::Exec (
        /*[in]*/ BSTR CmdName,
        /*[in]*/ enum vsCommandExecOption ExecuteOption,
        /*[in]*/ VARIANT * VariantIn,
        /*[in,out]*/ VARIANT * VariantOut,
        /*[in,out]*/ VARIANT_BOOL * Handled )
{
    suic::String strName(CmdName);

    InitUIDesigner();

    if (strName.Equals(GetCmdName(UI_OPENFILEID)))
    {
        if (_uiFrame.get() != NULL)
        {
            _uiFrame->UpdateWindow(GetActiveFilePath());
        }
    }
    else if (strName.Equals(GetCmdName(UI_OPENMAINWNDID)))
    {
        if (_uiFrame.get() != NULL)
        {
            _uiFrame->StartWindow(_U(""));
        }
    }
    else if (strName.Equals(GetCmdName(UI_OPENSETTINGID)))
    {
		if (_uiFrame.get() != NULL)
        {
			long lCount = 0;
			CComBSTR strVer;
			CreateVSInfo vsInfo;
            _Solution* _sln;
            m_pDTE->get_Solution(&_sln);

            if (NULL == _sln)
            {
                suic::Toast::Show(_U("获取解决方案失败！"), suic::Toast::DelayClose::ShortTime);
                return S_OK;
            }

            _sln->get_Count(&lCount);
			m_pDTE->get_Version(&strVer);

            vsInfo.needOpenSln = true;
            vsInfo.needAddSln = false;
            vsInfo.needOpenDir = lCount > 0;

			vsInfo.targetVs = (LPCTSTR)strVer;
            _uiFrame->ShowCreateVS(vsInfo);

			if (vsInfo.needOpenSln)
			{
				if (vsInfo.createVs.Equals(vsInfo.targetVs))
				{
                    if (lCount > 0)
                    {
                        suic::Toast::Show(_U("当前已经有工程打开！"), suic::Toast::DelayClose::ShortTime);
                    }
                    else
                    {
                        CComBSTR bstrPath(vsInfo.slnPath.c_str());
                        _sln->Open(bstrPath);
                    }
                }
			}

            if (vsInfo.needOpenDir && !vsInfo.createVs.Empty())
            {
                OpenAndSelectFile(vsInfo.slnPath.c_str());
            }

        }
    }

    return S_OK;
}

HRESULT CConnect::FindToolbarByName( _CommandBars* pCommandBars, LPCWSTR szToolbarName, CommandBar** pOut )  
{  
    if (pCommandBars == NULL || pOut == NULL)  
    {
        return E_FAIL;  
    }
  
    CComVariant varToolbarName(szToolbarName);  
    int nToolBars = 0;  
    HRESULT hr = pCommandBars->get_Count(&nToolBars);

    for(int idx = 1; idx <= nToolBars; idx++)  
    {  
        CComPtr<CommandBar> pCommandBar;  
        hr = pCommandBars->get_Item(CComVariant(idx), &pCommandBar);  
        if (FAILED(hr))  
        {
            continue;  
        }
  
        BSTR bsName = NULL;  

        if (pCommandBar != NULL)  
        {
            pCommandBar->get_Name(&bsName); 
        }

        if (CComVariant(bsName) == varToolbarName)  
        {
            *pOut = pCommandBar;  
            (*pOut)->AddRef();  
            break;  
        }  

        SysFreeString(bsName);  
        hr = E_FAIL;  
    }  
    return hr;  
}

void CConnect::UnregisterCommand(CComPtr<EnvDTE::Commands>& pCommands, LPCWSTR commandName)  
{  
    CComPtr<EnvDTE::Command> pCommand;  
  
    //  
    // COMMAND_NAME_FULL must be the module name plus the COMMAND_NAME.  
    // For this case, the module name can be found at the CvsInVC7.rgs and  
    // CvsInVC8.rgs files.  
    //  
    WCHAR item[256];  
    wcscpy_s(item, 256, MODULE_NAME);  
    wcscat_s(item, 256, commandName);  
  
    HRESULT hr = pCommands->Item(CComVariant(item), -1, &pCommand);  
  
    if (SUCCEEDED(hr))  
    {  
        pCommand->Delete();  
    }  
}

HRESULT CConnect::InitCommandBar(IDispatch *pApplication, ext_ConnectMode ConnectMode, IDispatch *pAddInInst)  
{  
    HRESULT hr = S_OK;  

    if (ConnectMode == AddInDesignerObjects::ext_cm_CommandLine)  
    {  
        //::MessageBox(GetActiveWindow(),_T("CConnect::OnConnection, CommandLine Mode"),_T("DevAssist"),MB_OK);  
        return S_OK;  
    }  
  
    CComPtr<IDispatch> pDisp;  
    CComPtr<EnvDTE::Commands> pCommands;  
    CComPtr<Commands2> pCommands2;  
    CComQIPtr<_CommandBars> pCommandBars;  
    CComPtr<CommandBar> pCommandBar;  
  
    IfFailGoCheck(m_pDTE->get_Commands(&pCommands), pCommands);  
    pCommands2 = pCommands;  
  
    // Get the set of command bars for the application.  
    IfFailGoCheck(m_pDTE->get_CommandBars(&pDisp), pDisp);  
    pCommandBars = pDisp;  
  
    // See if the toolbar has been created.  
    BOOL bRecreateToolbar = FALSE;  
    hr = FindToolbarByName(pCommandBars, TOOLBAR_NAME, &pCommandBar);

    if (SUCCEEDED(hr))  
    {
        //pCommands->RemoveCommandBar(pCommandBar);
        //bRecreateToolbar = true;

        CComPtr<CommandBarControls> pCommandBarControls;  
        pCommandBar->get_Controls(&pCommandBarControls);  
        int count = 0;  
        pCommandBarControls->get_Count(&count);  
  
        if (count == 0)  
        {  
           bRecreateToolbar = true;  
        }

        pCommandBar = NULL;  
    }  
    else  
    {  
        bRecreateToolbar = TRUE;  
    }
  
    if (ConnectMode == 5 || bRecreateToolbar) //5 == ext_cm_UISetup  
    {  
        // See if the CodeLib toolbar has been created.  
        hr = FindToolbarByName(pCommandBars, TOOLBAR_NAME, &pCommandBar);  
        if(FAILED(hr))  
        {  
            pDisp = NULL;  
            // The toolbar hasn't been created yet.  Add it.  
            hr = pCommands->AddCommandBar(CComBSTR(TOOLBAR_NAME),  
                EnvDTE::vsCommandBarTypeToolbar,  
                NULL,  
                1,  
                &pDisp);  
  
            // Yes, this code is unnecessary, but it serves to prove that  
            // the command bar creation actually worked.  
            hr = FindToolbarByName(pCommandBars, TOOLBAR_NAME, &pCommandBar);  
        }  
  
        int curToolbarPosition = 1;  
        int nMaxCommand = sizeof(s_commandList) / sizeof(s_commandList[0]);  

        for (int curCommand = 0; curCommand < nMaxCommand; ++curCommand)  
        {  
            CComPtr<EnvDTE::Command> pCreatedCommand;  
            const stCommandInfo* commandInfo = &s_commandList[curCommand];  
            UnregisterCommand(pCommands, commandInfo->m_id);  
  
            HRESULT hr2 = pCommands2->AddNamedCommand2(m_pAddInInstance,  
                CComBSTR(commandInfo->m_id),  
                CComBSTR(commandInfo->m_name),  
                CComBSTR(commandInfo->m_strTip),  
                VARIANT_FALSE,  
                CComVariant(commandInfo->m_bitmapID),  
                NULL,  
                (EnvDTE::vsCommandStatusSupported + EnvDTE::vsCommandStatusEnabled),  
                EnvDTE80::vsCommandStylePict,  
                EnvDTE80::vsCommandControlTypeButton,  
                &pCreatedCommand);

            if (SUCCEEDED(hr2) && (pCreatedCommand) &&  commandInfo->m_hasIcon)  
            {  
                //Add the control:  
                pDisp = NULL;  
                IfFailGoCheck(pCreatedCommand->AddControl(pCommandBar, curToolbarPosition, &pDisp),pDisp);  
                curToolbarPosition++;  
            }  
        }  
    }  
  
Error:  
  
    return hr;  
}

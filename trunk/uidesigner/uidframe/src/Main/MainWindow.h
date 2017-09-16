// 华勤科技版权所有 2010-2011
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _MAINWINDOW_H_
# define _MAINWINDOW_H_

using namespace suic;

#include "StartWindow.h"
#include "SlnTreeManager.h"

#include <uidcore/uidframe.h>
#include <main/EncryResource.h>
#include <Editor/ControlAssetsWindow.h>

#define PREVIEWAPP _U("uidpreviewu.exe")

class LoadPrjThread : public suic::Thread
{
public:

    LoadPrjThread(suic::InvokeProxy* h)
    {
        _handler = h;
        _project = NULL;
    }

    void Run();
    void OnInvoker(Object* sender, InvokerArg* e);

protected:

    Project* _project;
    suic::InvokeProxy* _handler;
};

class LockShowHandler : public suic::AssignerInvoker
{
public:

    LockShowHandler()
    {
        _assigner = NULL;
    }

    ~LockShowHandler()
    {
    }

    void Init(Element* target)
    {
        _assigner = target->GetAssigner();
    }

    void Invoke()
    {
        if (_notifyLocked)
        {
            _target->NotifyLockedChanged();
        }
        else
        {
            _target->NotifyShowChanged();
        }
    }

    void Notify(DesignElement* target, bool notify)
    {
        _target = target;
        _notifyLocked= notify;
        _assigner->BeginInvoke(this);
    }

private:

    bool _notifyLocked;
    DesignElement* _target;
    suic::Assigner* _assigner;
};

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    /// <summary>
    ///   打开指定路径的工程
    /// </summary>
    /// <param name="path">工程路径</param>
    /// <returns>工程对象</returns>
    Project* OpenProject(const suic::String& path);

    /// <summary>
    ///   根据工程名称和路径创建新的工程
    /// </summary>
    /// <param name="name">工程名称</param>
    /// <param name="path">工程路径</param>
    /// <returns>工程对象</returns>
    Project* CreateProject(const suic::String& name, const suic::String& path);

    void ExecutePreview(ElementRootItem* rootElem);

    Project* OpenOrLoadProject();
    void OpenOrLoadProjectSuccess(Project* prj);

    void ShowStartWindow(bool bExitMain);

    void SwitchToThemeView(suic::Window* themeElem, const suic::String& strTitle);
    void SwitchToMainView();

    void SetMainTitle(const String& strTitle);
    void SetSkipStartWindow(bool bSkip);

    void OpenWindow(const suic::String& strUri);

    void SetUIDMode(IUIDFrame::UIDMode mode);

protected:

    void OnMainMenuClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnHyperlinkClick(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnShowWindow();

    void OnClosing(suic::CancelEventArg* e);
    void OnAccessKey(suic::AccessKeyEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

    void OnRedoUndoButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnPreviewButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnExportResButtonClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnMoreExpanderClick(suic::Element* sender, suic::RoutedEventArg* e);

    void OnMenuButtonClick(suic::Element* sender, suic::RoutedEventArg* e);

    void OnApplyTemplate();
    void InitTreeDoc();
    void InitToolbarEvents();

    bool StartProcess(wchar_t* path, int timeout);

    void ExecuteRedo();
    void ExecuteUndo();

protected:

    HMODULE _uidCore;
    String _mainTitle;

    bool _skipStartWindow;
    IUIDFrame::UIDMode _mode;

    // 元素对象树
    ElementTree* _eleTree;
    // 工程视图管理对象
    SlnTreeManager* _docMana;

    // 主窗口下拉菜单
    suic::ContextMenu* _mainMenu;
    suic::CheckBox* _menuBtn;

    // 开始界面，用于打开，创建工程
    StartWindow* _startWindow;
    // 控件资源管理窗口
    ControlAssetsWindow* _ctrlAssetsWnd;

    LoadPrjThread* _loadThread;
    suic::Handler* _loadPrjHandler;

    Window* _previewWnd;
    EncryResource* _encryWnd;
};

#endif


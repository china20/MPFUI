// 华勤科技版权所有 2008-2022
// 
// 文件名：Application.h
// 功  能：应用程序对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIAPPLICATION_H_
#define _UIAPPLICATION_H_

#include <Framework/Controls/Window.h>
#include <System/Windows/Assigner.h>
#include <System/Resources/ResourceDictionary.h>

namespace suic {

class Application;
typedef shared<Application> AppPtr;

class SUICORE_API Application : public DpObject
{
public:

    // 系统加载完毕的通知事件
    EventHandler LoadCompleted;
    // 系统启动的通知事件
    EventHandler Startup;
    // 系统退出时的通知事件
    EventHandler Exit;
    // 应用系统激活时通知事件
    EventHandler Activated;
    // 系统非激活时通知对象
    EventHandler Deactivated;

    Application();
    virtual ~Application();

    RTTIOfClass(Application)

    void Dispose();

    void Run(Window* mainWnd);
    void Run(Window* mainWnd, const String& path);
    void Run();

    /// <summary>
    ///  关闭系统
    /// </summary>
    /// <remarks>
    ///  关闭系统应用，退出整个消息循环。
    /// </remarks>
    /// <returns>无</returns> 
    void Shutdown();

    /// <summary>
    ///  关闭系统
    /// </summary>
    /// <remarks>
    ///  关闭系统应用，退出整个消息循环。
    /// </remarks>
    /// <param name="exitCode">退出码</param>
    /// <returns>无</returns> 
    void Shutdown(int exitCode);

    static Application* Current();
    static void SetCurrent(Application* pApp);

    /// <summary>
    ///  播放声音
    /// </summary>
    /// <remarks>
    ///  播放指定的系统声音
    /// </remarks>
    /// <param name="soundName">声音名称</param>
    /// <returns>无</returns> 
    static void PlaySound(String soundName);

    /// <summary>
    ///  获取应用级别的样式资源
    /// </summary>
    /// <remarks>
    ///  此资源对于应用系统是全局的，所有窗口共享。
    /// </remarks>
    /// <returns>系统资源对象</returns> 
    ResourceDictionary* GetResources();
    void SetResources(ResourceDictionary* resources);
    void SetResources(const String& res);

    /// <summary>
    ///  查找资源
    /// </summary>
    /// <remarks>
    ///  在系统资源中查找指定名称的资源
    /// </remarks>
    /// <param name="name">资源名称</param>
    /// <returns>资源对象</returns> 
    Object* FindRes(const String& name);

    void InvalidateResourceReference();

    static bool LoadApp(Window* comp, const String& path);
    static void RunApp(Window* main, const String& path);

public:

    virtual bool OnBuild(IXamlNode* pNode, ObjectPtr& obj);
    virtual bool OnEvent(Object* sender, MessageParam* mp);

protected:

    virtual void DoShutdown();

// 属性访问定义
public:

    /// <summary>
    ///  取得当前应用的主窗口
    /// </summary>
    Window* GetMainWindow();
    void SetMainWindow(Window* val);

    Window* LookupWindow(String name);

    ResourceUri GetStartupUri() const;
    void SetStartupUri(const ResourceUri& val);

    bool IsShuttingDown();

protected:

    ResourceUri _startupUri;
    Window* _mainWindow;
    Assigner* _assigner;
   
    ResourceDictionary* _resources;
};

inline ResourceDictionary* Application::GetResources()
{
    return _resources;
}

}

#endif

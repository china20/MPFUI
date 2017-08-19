// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Application.h
// ��  �ܣ�Ӧ�ó������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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

    // ϵͳ������ϵ�֪ͨ�¼�
    EventHandler LoadCompleted;
    // ϵͳ������֪ͨ�¼�
    EventHandler Startup;
    // ϵͳ�˳�ʱ��֪ͨ�¼�
    EventHandler Exit;
    // Ӧ��ϵͳ����ʱ֪ͨ�¼�
    EventHandler Activated;
    // ϵͳ�Ǽ���ʱ֪ͨ����
    EventHandler Deactivated;

    Application();
    virtual ~Application();

    RTTIOfClass(Application)

    void Dispose();

    void Run(Window* mainWnd);
    void Run(Window* mainWnd, const String& path);
    void Run();

    /// <summary>
    ///  �ر�ϵͳ
    /// </summary>
    /// <remarks>
    ///  �ر�ϵͳӦ�ã��˳�������Ϣѭ����
    /// </remarks>
    /// <returns>��</returns> 
    void Shutdown();

    /// <summary>
    ///  �ر�ϵͳ
    /// </summary>
    /// <remarks>
    ///  �ر�ϵͳӦ�ã��˳�������Ϣѭ����
    /// </remarks>
    /// <param name="exitCode">�˳���</param>
    /// <returns>��</returns> 
    void Shutdown(int exitCode);

    static Application* Current();
    static void SetCurrent(Application* pApp);

    /// <summary>
    ///  ��������
    /// </summary>
    /// <remarks>
    ///  ����ָ����ϵͳ����
    /// </remarks>
    /// <param name="soundName">��������</param>
    /// <returns>��</returns> 
    static void PlaySound(String soundName);

    /// <summary>
    ///  ��ȡӦ�ü������ʽ��Դ
    /// </summary>
    /// <remarks>
    ///  ����Դ����Ӧ��ϵͳ��ȫ�ֵģ����д��ڹ���
    /// </remarks>
    /// <returns>ϵͳ��Դ����</returns> 
    ResourceDictionary* GetResources();
    void SetResources(ResourceDictionary* resources);
    void SetResources(const String& res);

    /// <summary>
    ///  ������Դ
    /// </summary>
    /// <remarks>
    ///  ��ϵͳ��Դ�в���ָ�����Ƶ���Դ
    /// </remarks>
    /// <param name="name">��Դ����</param>
    /// <returns>��Դ����</returns> 
    Object* FindRes(const String& name);

    void InvalidateResourceReference();

    static bool LoadApp(Window* comp, const String& path);
    static void RunApp(Window* main, const String& path);

public:

    virtual bool OnBuild(IXamlNode* pNode, ObjectPtr& obj);
    virtual bool OnEvent(Object* sender, MessageParam* mp);

protected:

    virtual void DoShutdown();

// ���Է��ʶ���
public:

    /// <summary>
    ///  ȡ�õ�ǰӦ�õ�������
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

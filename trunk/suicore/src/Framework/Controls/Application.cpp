
#include <Framework/Controls/Application.h>

#include <System/Windows/Assigner.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Types/Interface.h>
#include <System/Types/String.h>

#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

#include <System/Tools/TreeBuilder.h>
#include <System/Tools/DesignHelper.h>

namespace suic
{

static AppPtr g_s_app;

ImplementRTTIOfClass(Application, DpObject)

Application::Application()
    : _mainWindow(NULL)
{
    _resources = new ResourceDictionary();
    _assigner = Assigner::Current();

    ADDREFOBJ(_resources);
    ADDREFOBJ(_assigner);
}

Application::~Application()
{
    FREEREFOBJ(_mainWindow);
    FREEREFOBJ(_resources);
    FREEREFOBJ(_assigner);
}

void Application::Dispose()
{
    FREEREFOBJ(_mainWindow);
    FREEREFOBJ(_resources);
    FREEREFOBJ(_assigner);

    Object::Dispose();
}

void Application::SetCurrent(Application* pApp)
{
    if (g_s_app.get() == NULL)
    {
        g_s_app = pApp;
    }
    else if (pApp->InDesignMode())
    {
        DesignHelper::SetDesignApplication(pApp);
    }
}

void Application::Shutdown()
{
    Shutdown(0);
}

void Application::Shutdown(int exitCode)
{
    DoShutdown();

    if (GetMainWindow() != NULL)
    {
        GetMainWindow()->Close();
    }

    ::PostQuitMessage(exitCode);
}

void Application::DoShutdown()
{
}

void Application::PlaySound(String soundName)
{
}

Application* Application::Current()
{
    if (DesignHelper::IsInDesignMode())
    {
        return DesignHelper::GetDesignApplication();
    }
    return g_s_app.get();
}

void Application::Run()
{
    if (NULL == g_s_app)
    {
        g_s_app = this;
    }

    if (g_s_app != this)
    {
        throw InvalidOperationException(_U("Application::Run"), _U("Application::Current dont equal"), __FILELINE__);
    }

    if (NULL != _mainWindow && _mainWindow->Show() >= 0)
    {
        // ¼¤»îÖ÷´°¿Ú
        _mainWindow->Activate();
    }

    if (LoadCompleted)
    {
        EventArg ev;
        LoadCompleted(this, &ev);
    }

    Assigner::Current()->Run();

    g_s_app = NULL;
}

void Application::Run(Window* elem)
{
    WindowPtr runWnd(elem);
    SetMainWindow(runWnd.get());
    Run();
}

void Application::Run(Window* main, const String& path)
{
    DWORD dwTime = Environment::GetSystemTick();
    WindowPtr runWnd(main);

    SetMainWindow(main);
    XamlReader::LoadUri(NULL, this, path).get();

    if (g_s_app.get())
    {
        g_s_app->Run();
    }

    runWnd = NULL;
}

void Application::RunApp(Window* main, const String& path)
{
    WindowPtr runWnd(main);
    if (LoadApp(runWnd.get(), path))
    {
        Application::Current()->Run();
    }
}

ResourceUri Application::GetStartupUri() const
{
    return _startupUri;
}

void Application::SetStartupUri(const ResourceUri& uri)
{
    _startupUri = uri;
}

Window* Application::GetMainWindow()
{
    return _mainWindow;
}

void Application::SetMainWindow(Window* mainWnd)
{
    if (_mainWindow != mainWnd)
    {
        SETREFOBJ(_mainWindow, mainWnd);
    }
}

Window* Application::LookupWindow(String name)
{
    return RTTICast<Window>(HwndSubclass::LookRootElement(name));
}

bool Application::IsShuttingDown()
{
    return false;
}

void Application::SetResources(ResourceDictionary* resources)
{
    if (resources != _resources && resources)
    {
        SETREFOBJ(_resources, resources);
        InvalidateResourceReference();
    }
}

void Application::SetResources(const String& res)
{
    AutoObj obj;
    obj = Builder::BuildUri(NULL, res);
}

Object* Application::FindRes(const String& name)
{
    Object* resObj = NULL;
    if (_resources != NULL)
    {
        resObj = _resources->Lookup(name);
    }
    if (DpProperty::UnsetValue() == resObj)
    {
        resObj = CoreSystem::Ins()->FindRes(name);
    }
    return resObj;
}

bool Application::OnBuild(IXamlNode* pNode, ObjectPtr& obj)
{
    return false;
}

bool Application::OnEvent(Object* sender, MessageParam* mp)
{
    return false;
}

void Application::InvalidateResourceReference()
{
    HwndSubclass::InvalidateResourceReference();
}

bool Application::LoadApp(Window* comp, const String& path)
{
    ObjectPtr root;
    root = suic::XamlReader::LoadUri(NULL, comp, path);
    return (root.get() != NULL);
}

}

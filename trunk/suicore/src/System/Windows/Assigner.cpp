/////////////////////////////////////////////////////////////////////////
// assigner.cpp

#include <System/Windows/Assigner.h>
#include <System/Windows/CoreTool.h>

#include <System/Tools/Thread.h>
#include <System/Tools/Array.h>

#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Windows/BindingExpression.h>

#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// AssignerFrame

//Map<HWND, HACCEL> g_acctables;
//
//HACCEL InternalFindAccel(HWND hwnd)
//{
//    Map<HWND, HACCEL>::iterator iter = g_acctables.find(hwnd);
//
//    if (iter == g_acctables.end())
//    {
//        return NULL;
//    }
//    else
//    {
//        return iter->second;
//    }
//}
//
//void InternalRemoveAccel(HWND hwnd)
//{
//    g_acctables.erase(hwnd);
//}
//
//void InternalAddAccel(HWND hwnd, HACCEL hacc)
//{
//    InternalRemoveAccel(hwnd);
//    g_acctables[hwnd] = hacc;
//}

void BindingWorker::AddTask(Object* task)
{
    if (NULL != task)
    {
        task->ref();
        BindingExpression* expr = DynamicCast<BindingExpression>(task);
        if (expr)
        {
            _tasks.Add(WeakRef(task));
        }
        task->unref();
    }
}

void BindingWorker::Run()
{
    int iCount = GetTaskCount();

    for (int i = 0; i < iCount; ++i)
    {
        BindingExpression* expr = RTTICast<BindingExpression>(_tasks.GetItem(i).GetTarget());
        if (NULL != expr)
        {
            expr->AttachToContext(BindingExpression::AttachAttempt::Again);
        }
    }

    _tasks.Clear();
}

//Accelerator* __RetrieveAccelerator(HWND hwnd)
//{
//    VisualHostPtr pTarget(HwndSubclass::FindRenderTarget(hwnd));
//
//    if (!pTarget)
//    {
//        return NULL;
//    }
//    else
//    {
//        RenderInfoPtr pInfo(pTarget->GetRenderInfo());
//        return pInfo->GetHwndObject()->GetAccelerator();
//    }
//}

AssignerFrame::AssignerFrame()
{
    _exitWhenRequested = true;
    _continue = true;
    _assigner = Assigner::Current();
}

AssignerFrame::AssignerFrame(bool exitWhenRequested)
{
    _exitWhenRequested = exitWhenRequested;
    _continue = true;
    _assigner = Assigner::Current();
}

AssignerFrame::~AssignerFrame()
{
    Clear();
}

Assigner* AssignerFrame::GetAssigner()
{
    return _assigner;
}

bool AssignerFrame::IsContinue()
{
    bool shouldContinue = _continue; 

    if (shouldContinue)
    { 
        if (_exitWhenRequested) 
        {
            Assigner* assign = GetAssigner();

            if (assign->_exitAllFrames || assign->_hasStartedClose) 
            { 
                shouldContinue = false;
            } 
        }
    }

    return shouldContinue; 
}

void AssignerFrame::SetContinue(bool value)
{
    _continue = value;
}

void AssignerFrame::Clear()
{
    _hooks.Clear();
}

void AssignerFrame::RegisterHook(Object* obj, MessageHook hook)
{
    _hooks.RegisterHook(obj, hook);
}

void AssignerFrame::RemoveHookFromObject(Object* obj)
{
    _hooks.RemoveHookFromObject(obj);
}

void AssignerFrame::RemoveHook(Object* obj, MessageHook hook)
{
    _hooks.RemoveHook(obj, hook);
}

//void AssignerFrame::RegisterFrameHook(MessageHook hook)
//{
//    if (hook)
//    {
//        _assignerHooks.Add(hook);
//    }
//}
//
//void AssignerFrame::RemoveFrameHook(MessageHook hook)
//{
//    for (int i = 0; i < _assignerHooks.Length(); ++i)
//    {
//        if (_assignerHooks[i].Equals(&hook))
//        {
//            _assignerHooks.RemoveAt(i);
//            break;
//        }
//    }
//}

void AssignerFrame::ClearHooks()
{
    _hooks.Clear();
}

/////////////////////////////////////////////////////////////////////////
// Assigner

Popup* Assigner::GetTopTrackPopup()
{
    int iCount = _trackPopups.Length();

    if (iCount > 0)
    {
        return _trackPopups[iCount - 1];
    }
    else
    {
        return NULL;
    }
}

bool Assigner::PopTrackPopup()
{
    int iCount = _trackPopups.Length();

    if (iCount > 0)
    {
        _trackPopups.RemoveAt(iCount - 1);
    }

    return iCount > 1;
}

void Assigner::PushTrackPopup(Popup* popup)
{
    RemoveTrackPopup(popup);
    _trackPopups.Add(popup);
}

void Assigner::RemoveTrackPopup(Popup* popup)
{
    int iCount = _trackPopups.Length();

    for (int i = iCount - 1; i >= 0; --i)
    {
        if (popup == _trackPopups[i])
        {
            _trackPopups.RemoveAt(i);
            break;
        }
    }
}

void Assigner::InsertAssignerFrame(int index, AssignerFrame* frame)
{
    frame->ref();

    RemoveAssignerFrame(frame);

    if (index >= 0 && index < _frames.Length())
    {
        _frames.Insert(index, frame);
    }
    else
    {
        _frames.Add(frame);
    }
}

void Assigner::RemoveAssignerFrame(AssignerFrame* frame)
{
    int iCount = _frames.Length();

    for (int i = iCount - 1; i >= 0; --i)
    {
        if (frame == _frames[i])
        {
            _frames.RemoveAt(i);
            frame->unref();
            break;
        }
    }
}

void Assigner::RecycleObject(Object* obj)
{
    if (obj)
    {
        Object* pRef(DynamicCast<Object>(obj));
        if (pRef)
        {
            pRef->ref();
        }
        _recyEles.Add(obj);
        ::PostMessage(HANDLETOHWND(_hwnd), _msgRecycleObj, 0, 0);
    }
}

void Assigner::DoRecyleObjects()
{
    for (int i = 0; i < _recyEles.Length(); ++i)
    {
        Object* pRef(DynamicCast<Object>(_recyEles[i]));
        if (pRef)
        {
            pRef->unref();
        }
        else
        {
            delete _recyEles[i];
        }
    }

    _recyEles.Clear();
}

LRESULT __stdcall AssignerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool handled = false;
    LRESULT result = 0;

    result = Assigner::Current()->WndProcHook(HWNDTOHANDLE(hwnd)
            , (Int32)message, (Uint32)wParam, (Uint32)lParam, handled);
    
    if (!handled)
    {
        result = ::DefWindowProc(hwnd, message, wParam, lParam);
    }

    return result;
}

void Assigner::CreateMessageHwnd()
{
    if (NULL == _hwnd || !::IsWindow(HANDLETOHWND(_hwnd)))
    {
        Uint32 style = WS_POPUP;
        Uint32 styleEx = 0;
        const Char* className = _T("WRMSGDUIHWND");

        HINSTANCE hIns = (HINSTANCE)::GetModuleHandle(NULL);
        WNDCLASS wndcls;

        if (!::GetClassInfo(hIns, className, &wndcls))
        {
            wndcls.style = CS_DBLCLKS;
            wndcls.cbClsExtra = 0;
            wndcls.cbWndExtra = 0;
            wndcls.hInstance = hIns;
            wndcls.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION);
            wndcls.hCursor = 0;//LoadCursor(NULL, IDC_ARROW);
            wndcls.hbrBackground = NULL;
            wndcls.lpszMenuName = NULL;
            wndcls.lpszClassName = className;

            wndcls.lpfnWndProc = AssignerWndProc;
            RegisterClass(&wndcls);
        }

        _hwnd = HWNDTOHANDLE(CreateWindowEx(styleEx/* | WS_EX_APPWINDOW*/,
            className,
            NULL,
            style, 
            10,
            10,
            20,
            20,
            HWND_MESSAGE,
            NULL,
            hIns,
            (LPVOID)(LONG_PTR)this));

        SetTimer(HANDLETOHWND(_hwnd), 5, 10, NULL);
    }
}

Assigner* Assigner::_assigner = NULL;

Assigner::Assigner()
    : iscontinue(true)
    , _currentDoType(0)
    , _frameDepth(0)
    , _exitAllFrames(false)
    , _hasFinishedClose(false)
    , _hasStartedClose(false)
    , _startingClose(false)
    , _threadId(0)
    , _queue(NULL)
    , _msgProcessQueue(0)
    , _msgRecycleObj(0)
    , _isTimerSet(false)
    , _dueTimeFound(false)
    , _timersVersion(0)
    , _dueTimeInTicks(0)
    , _bindWorker(NULL)
    , _timeManager(NULL)
{
    
}

Assigner::~Assigner()
{
    if (_frames.GetCount() == 1)
    {
        _frames[0]->unref();
        _frames.RemoveAt(0);
    }

    DestroyWindow(HANDLETOHWND(_hwnd));
    _hwnd = NULL;
    delete _bindWorker;
}

void Assigner::DoInit()
{
    _msgProcessQueue = ::RegisterWindowMessage(L"MPFAssignerProcessQueue");
    _msgRecycleObj = ::RegisterWindowMessage(L"MPFAssignerRecycleObjects");
    _queue = new InvokerPriorityQueue();
    _threadId = Thread::CurrentThreadId();

    InsertAssignerFrame(-1, new AssignerFrame());

    _bindWorker = new BindingWorker();
    GetTimeManager()->Start();

    _queue->ref();

    CreateMessageHwnd();
}

void Assigner::RegisterHook(Object* obj, MessageHook hook)
{
    _hooks.RegisterHook(obj, hook);
}

void Assigner::RemoveHookFromObject(Object* obj)
{
    _hooks.RemoveHookFromObject(obj);
}

void Assigner::RemoveHook(Object* obj, MessageHook hook)
{
    _hooks.RemoveHook(obj, hook);
}

bool Assigner::DoMessageHook(Object* root, MessageParam* mp, bool& interrupt)
{
    return _hooks.DoMessageHook(root, mp, interrupt);
}

void Assigner::AddTimer(AssignerTimer* timer)
{
    {
        suic::Mutex::Locker sync(insLock);

        timer->ref();

        if (!_hasFinishedClose)
        {
            bool bExist = false;

            for (int i = 0; i < _timers.Length(); ++i)
            {
                if (_timers[i] == timer)
                {
                    bExist = true;
                    break;
                }
            }

            if (!bExist)
            {
                timer->ref();
                _timers.Add(timer);
                _timersVersion += 1;
            }
        }

        timer->unref();
    }

    UpdateDispachterTimer(false);
}

void Assigner::RemoveTimer(AssignerTimer* timer)
{
    {
        suic::Mutex::Locker sync(insLock);

        if (!_hasFinishedClose)
        {
            for (int i = 0; i < _timers.Length(); ++i)
            {
                if (_timers[i] == timer)
                {
                    _timers.RemoveAt(i);
                    timer->unref();
                    break;
                }
            }
            _timersVersion += 1L;
        }
    }

    UpdateDispachterTimer(false);
}

void Assigner::SetInternalTimer(int iTicks)
{
    HWND msgHwnd = HANDLETOHWND(_hwnd);

    if (msgHwnd != NULL && ::IsWindow(msgHwnd))
    {
        int uElapse = iTicks - Environment::GetSystemTick();

        if (uElapse < 1)
        {
            uElapse = 1;
        }

        ::SetTimer(msgHwnd, 2, uElapse, NULL);
        _isTimerSet = true;
    }
}

void Assigner::KillInternalTimer()
{
    HWND msgHwnd = HANDLETOHWND(_hwnd);

    if (msgHwnd != NULL && ::IsWindow(msgHwnd))
    {
        ::KillTimer(msgHwnd, 2);
        _isTimerSet = false;
    }
}

void Assigner::UpdateDispachterTimer(bool unUsed)
{
    if (!_hasFinishedClose)
    {
        bool flag = _dueTimeFound;
        int num2 = _dueTimeInTicks;
        _dueTimeFound = false;
        _dueTimeInTicks = 0;

        if (_timers.Length() > 0)
        {
            for (int i = 0; i < _timers.Length(); i++)
            {
                AssignerTimer* timer = _timers[i];

                if (!_dueTimeFound || (timer->_dueTime < _dueTimeInTicks))
                {
                    _dueTimeFound = true;
                    _dueTimeInTicks = timer->_dueTime;
                }
            }
        }

        if (_dueTimeFound)
        {
            if ((!_isTimerSet || !flag) || (num2 != _dueTimeInTicks))
            {
                SetInternalTimer(_dueTimeInTicks);
            }
        }
        else if (flag)
        {
            KillInternalTimer();
        }
    }
}

bool Assigner::CheckAccess()
{
    return (_threadId == Thread::CurrentThreadId());
}

void Assigner::VerifyAccess(char* f, int l)
{
    if (_threadId != Thread::CurrentThreadId())
    {
        InvalidOperationException e(_U("VerifyAccess failed(not in UI thread)!"), f, l);
#ifdef _DEBUG
        throw e;
#endif
    }
}

Assigner* Assigner::Current()
{
    if (!_assigner)
    {
        _assigner = new Assigner();
        _assigner->ref();
        _assigner->DoInit();
    }

    return _assigner;
}

MessageParam* Assigner::Lastmp()
{
    return &(Current()->lastmp);
}

AssignerFrame* Assigner::GetTopAssignerFrame()
{
    if (0 == _frames.Length())
    {
        return NULL;
    }
    else
    {
        return _frames[_frames.Length() - 1];
    }
}

AssignerFrame* Assigner::GetAssignerFrame(int index)
{
    return _frames[index];
}

int Assigner::GetAssignerFrameCount()
{
    return _frames.Length();
}

void Assigner::Run()
{
    PushMessageLoop(new AssignerFrame());
}

void Assigner::TranslateAndAssignMessage(MSG* msg)
{
    if (msg->message != 0x036A)
    {
        ::TranslateMessage(msg);
        ::DispatchMessage(msg);
    }
}

TimeManager* Assigner::GetTimeManager()
{
    if (_timeManager == NULL)
    {
        _timeManager = new TimeManager();
    }
    return _timeManager;
}

void Assigner::PushMessageLoop(AssignerFrame* frame)
{
    if (!frame)
    {
        throw _T("Assigner Object is null");
    }

    frame->ref();
    AssignerPtr currentAssigner = Current();

    if (currentAssigner->_hasFinishedClose)
    {
        throw _T("Assigner Object has been shut down");
    }

    if (frame->GetAssigner() != currentAssigner.get())
    {
        throw _T("Assigner Object doesn't match");
    }

    /*if (currentDispatcher->_disableProcessingCount > 0)
    {
    }*/

    currentAssigner->DoRealMessageLoop(frame);
    frame->unref();
}

bool Assigner::OnIdle(int iIdleCount)
{
    if (iIdleCount >= 0)
    {
        //GetTimeManager()->Tick();
    }
    return false;
}

bool Assigner::IsIdleMessage(MSG* pMsg)
{
    if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE)
    {
        if (lastPoint == pMsg->pt && pMsg->message == lastMessage)
        {
            return false;
        }

        lastPoint = pMsg->pt;  // remember for next time
        lastMessage = pMsg->message;
        return true;
    }

    // WM_PAINT and WM_SYSTIMER (caret blink)
    return pMsg->message != WM_PAINT && pMsg->message != 0x0118;
}

void Assigner::DoRealMessageLoop(AssignerFrame* frame)
{
    if (!_hwnd)
    {
        CreateMessageHwnd();
    }

    InsertAssignerFrame(-1, frame);
    ++_frameDepth;

    bool bDoIdle = TRUE;
    int iIdleCount = 0;
    String strErr;
#ifndef _DEBUG
    try
#endif
    {
        while (frame->IsContinue())
        {
            MSG msg = {0};

            while (bDoIdle && !::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
            {
                if (!OnIdle(iIdleCount++))
                {
                    bDoIdle = false;
                }
            }

            do
            {
                if (::GetMessage(&msg, NULL, NULL, NULL))
                {
                    TranslateAndAssignMessage(&msg);
                }
                else
                {
                    frame->SetContinue(false);
                    break;
                }

                if (!frame->IsContinue())
                {
                    break;
                }

                if (IsIdleMessage(&msg))
                {
                    bDoIdle = true;
                    iIdleCount = 0;
                }

            }while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));

            /*while (bDoIdle && !::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if (!OnIdle(iIdleCount++))
                {
                    bDoIdle = false;
                }
            }

            if (!GetMessage(&msg, 0, 0, 0))
            {
                break;
            }

            //if (!HandleHook(&msg))
            {
                //ProcessSenderMessageQueue();

                AssignerFrame* prevFrame = _currFrame;

                _currFrame = frame;
                TranslateAndAssignMessage(&msg);
                _currFrame = prevFrame;
            }

            if (!frame->IsContinue())
            {
                break;
            }

            if (msg.message == WM_MOUSEMOVE || msg.message == WM_NCMOUSEMOVE ||
                msg.message == WM_PAINT || msg.message == 0x0118)
            {
                ;
            }
            else
            {
                bDoIdle = true;
                iIdleCount = 0;
            }*/
        }

        if (_frameDepth == 1 && _hasStartedClose)
        {
            InternalShutdown();
        }
    }
#ifndef _DEBUG
    catch (String& err)
    {
        strErr = err;
    }
    catch (...)
    {
        ;
    }
#endif

    _frameDepth--;
    RemoveAssignerFrame(frame);

    if (_frameDepth == 0)
    {
        _exitAllFrames = false;
    }

    PostMessage(NULL, WM_NULL, 0, 0);
    Debug::Trace(_T("AssignerStage exited, Frame Depth is %d\n"), _frameDepth);
}

void Assigner::DoRealShutdown()
{
    if (!_startingClose)
    {
        _startingClose = true;

        //
        // 开始关闭整个系统
        //

        _hasStartedClose = true;

        if (_frameDepth <= 0)
        {
            InternalShutdown();
        }
    }
}

void Assigner::InternalShutdown()
{
}

LRESULT Assigner::
WndProcHook(Handle hwnd, Int32 message, Uint32 wParam, Uint32 lParam, bool& handled)
{
    if (hwnd != _hwnd)
    {
        return 0;
    }

    // 检测被禁止掉的操作项
    //ProcessMessageQueue();

    //
    // 处理销毁消息
    //
    if (message == WM_DESTROY)
    {
        // 退出
    }
    else if (_msgRecycleObj == message)
    {
        DoRecyleObjects();
        handled = true;
    }
    else if (_msgProcessQueue == message)
    {
        // 
        // 处理消息队列
        //
        ProcessMessageQueue();
        handled = true;
    }
    // 处理定时消息
    else if (message == WM_TIMER)
    {
        handled = true;

        if ((int)wParam == 1)
        {
            KillTimer(HANDLETOHWND(hwnd), 1);
            ProcessMessageQueue();
        }
        else if ((int)wParam == 2)
        {
            KillTimer(HANDLETOHWND(hwnd), 2);
            _isTimerSet = false;
            PromoteTimers(Environment::GetSystemTick());
        }
        else if ((int)wParam == 5)
        {
            ::KillTimer(HANDLETOHWND(hwnd), 5);
            GetTimeManager()->Tick();
            ::SetTimer(HANDLETOHWND(hwnd), 5, 10, NULL);
        }
    }

    return 0;
}

bool Assigner::IsInputPending()
{
    const Int32 MsgFlag = QS_INPUT | QS_POSTMESSAGE;

    return (0 == ::MsgWaitForMultipleObjectsEx(0, NULL, 0, 0x200f, MWMO_INPUTAVAILABLE));
}

bool Assigner::RequestProcessing(bool force)
{
    bool flag = true;

    if (IsWindowNull())
    {
        return false;
    }

    InvokerPriority maxPriority = (InvokerPriority)_queue->GetMaxPriority();

    switch (maxPriority)
    {
        case eInvalid:
        case eInactive:
            return flag;
    }

    if (force)
    {
        // 
        // 当前处理的是后台消息，关掉定时器
        //
        if (_currentDoType == 1)
        {
            KillTimer(HANDLETOHWND(_hwnd), 1);          
        }
        //
        // 前台消息，直接吃掉内部消息
        //
        else if (_currentDoType == 2)
        {
            MSG msg;
            PeekMessage(&msg, HANDLETOHWND(_hwnd), _msgProcessQueue, _msgProcessQueue, PM_REMOVE);
        }

        //
        // 设置处理类型为前台
        //
        _currentDoType = 0;
    }

    if (maxPriority < eInput)
    {
        return DoForegroundMessage();
    }
    else
    {
        return DoBackgroundMessage();
    }
}

void Assigner::BeginInvoke(AssignerInvoker* data)
{
    bool flag = false;
    {
        data->ref();

        if (!_hasFinishedClose)
        {
            _queue->Enqueue(data, data->GetPriority());

            flag = RequestProcessing(true);

            if (!flag)
            {
                _queue->RemoveItem(data);
                data->_status = eAborted;
            }
        }
        else
        {
            //data = new AssignerInvoker();
        }

        data->unref();
    }
}

void Assigner::Invoke(AssignerInvoker* data, Int32 timeout)
{
    // 
    // 调用异步事件进行处理
    //
    BeginInvoke(data);

    data->Wait(timeout);

    if (data->GetStatus() == eCompleted)
    {
        //obj = data->GetResult();
    }

    data->Abort();
}

void Assigner::ProcessSenderMessageQueue()
{
    AssignerInvoker* operation(_queue->GetSenderItem());

    if (operation)
    {
        operation->Invoke();
        operation->_status = eCompleted;

        //
        // 触发操作结束事件
        //
        operation->OnComplete();
        operation->unref();
    }
}

void Assigner::ProcessMessageQueue()
{
    InvokerPriority invalid = eInvalid;
    AssignerInvoker* operation = NULL;

    {
        _currentDoType = 0;
        bool flag2 = !IsInputPending();
        invalid = (InvokerPriority)_queue->GetMaxPriority();

        if (((invalid != eInvalid) && (invalid != eInactive)) && (invalid < 1 || flag2))
        {            
            operation = _queue->Dequeue();
        }

        invalid = (InvokerPriority)_queue->GetMaxPriority();

        RequestProcessing();
    }

    if (operation)
    {
        operation->Invoke();
        operation->_status = eCompleted;

        //
        // 触发操作结束事件
        //
        operation->OnComplete();
        operation->unref();
    }
}

void Assigner::PromoteTimers(Uint32 currTimeInTicks)
{
    {
        suic::Mutex::Locker sync(insLock);

        if (_hasFinishedClose || !_dueTimeFound || _dueTimeInTicks > currTimeInTicks)
        {
            return;
        }
    }

    for (;;)
    {
        AssignerTimer* pTimer = NULL;

        {
            suic::Mutex::Locker sync(insLock);

            for (int i = 0; i < _timers.Length(); ++i)
            {
                if (_timers[i]->_dueTime <= currTimeInTicks)
                {
                    pTimer = _timers[i];
                    _timers.RemoveAt(i);
                    break;
                }
            }
        }

        if (NULL == pTimer)
        {
            break;
        }

        pTimer->FireTick(false);
    }

    UpdateDispachterTimer(false);
}

bool Assigner::DoBackgroundMessage()
{
    bool flag = true;

    if (_currentDoType >= 1)
    {
        return flag;
    }

    // 有输入消息，启动定时器
    if (IsInputPending())
    {
        _currentDoType = 1;
        SetTimer(HANDLETOHWND(_hwnd), 1, 1, NULL);
        return true;
    }
    else
    {
        return DoForegroundMessage();
    }
}

bool Assigner::IsWindowNull()
{
    return NULL == HANDLETOHWND(_hwnd);
}

bool Assigner::DoForegroundMessage()
{
    // 
    // 后台消息，不做立即处理
    //
    if (_currentDoType >= 2)
    {
        return true;
    }

    //
    // 输入消息，关闭定时器马上处理
    //
    if (_currentDoType == 1)
    {
        KillTimer(HANDLETOHWND(_hwnd), 1);
    }

    _currentDoType = 2;

    //
    // 向隐藏窗口发送消息，启动队列处理
    //
    if (::IsWindow(HANDLETOHWND(_hwnd)))
    {
        ::PostMessage(HANDLETOHWND(_hwnd), _msgProcessQueue, 0, 0);
    }

    return true;
}

}

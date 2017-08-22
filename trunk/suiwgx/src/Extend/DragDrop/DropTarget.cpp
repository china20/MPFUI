
UDropTarget::UDropTarget()
{
    _refCount = 1;
}

UDropTarget::~UDropTarget()
{
}

STDMETHODIMP UDropTarget::QueryInterface(REFIID riid, void **ppv)  
{  
    static const QITAB qit[] =  
    {  
        QITABENT(SdkDropSource, IDropSource),  
        { 0 }  
    };  
  
    return QISearch(this, qit, riid, ppv);  
}  
  
STDMETHODIMP_(ULONG) UDropTarget::AddRef()  
{  
    return InterlockedIncrement(&_refCount);  
}  
  
STDMETHODIMP_(ULONG) UDropTarget::Release()  
{  
    ULONG lRef = InterlockedDecrement(&_refCount);  
    if (0 == lRef)  
    {  
        delete this;  
    }  
    return _refCount;  
}

STDMETHODIMP UDropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)  
{  
    if ( NULL == pDataObj )  
    {  
        return E_INVALIDARG;  
    }  
  
    POINT point = { pt.x, pt.y };  
    Point point2;

    _dataObject = GetDataObject(data);
    if ((_dataObject == NULL) || !IsDataAvailable(_dataObject))
    {
        effects = 0;
        return 1;
    }

    DpObject* currentTarget = GetCurrentTarget(point, point2);
    _lastTarget = currentTarget;

    if (currentTarget != NULL)
    {
        RaiseDragEvent(DragDrop::DragEnterEvent, dragDropKeyStates, effects, currentTarget, point2);
    }
    else
    {
        effects = 0;
    }

    return S_OK;  
}

STDMETHODIMP UDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)  
{  
    POINT ppt = { pt.x, pt.y };  
    Point point2;
    Debug::Assert(_dataObject != NULL);
    DpObject* currentTarget = GetCurrentTarget(point, point2);

    if (currentTarget != NULL)
    {
        if (currentTarget != _lastTarget)
        {
            if (_lastTarget != NULL)
            {
                RaiseDragEvent(DragDrop::DragLeaveEvent, dragDropKeyStates, effects, _lastTarget, point2);
            }
            RaiseDragEvent(DragDrop::DragEnterEvent, dragDropKeyStates, effects, currentTarget, point2);
            _lastTarget = currentTarget;
            return S_OK;
        }

        RaiseDragEvent(DragDrop.DragOverEvent, dragDropKeyStates, effects, currentTarget, point2);
    }
    else
    {
        if (_lastTarget != NULL)
        {
            RaiseDragEvent(DragDrop::DragLeaveEvent, dragDropKeyStates, effects, _lastTarget, point2);
        }
        _lastTarget = currentTarget;
        effects = 0;
    }

    return S_OK;  
}

STDMETHODIMP UDropTarget::DragLeave(void)  
{  
    if (_lastTarget != NULL)
    {
        int effects = 0;
        try
        {
            RaiseDragEvent(DragDrop::DragLeaveEvent, 0, effects, _lastTarget, Point(0.0, 0.0));
        }
        catch (...)
        {
        }

        _lastTarget = NULL;
        _dataObject = NULL;
    }
    return S_OK;  
}

STDMETHODIMP UDropTarget::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)  
{  
    Point point2;
    IDataObject* dataObject = GetDataObject(data);

    if ((dataObject == NULL) || !IsDataAvailable(dataObject))
    {
        effects = 0;
        return 1;
    }

    _lastTarget = NULL;
    DpObject* currentTarget = GetCurrentTarget(point, point2);

    if (currentTarget != NULL)
    {
        RaiseDragEvent(DragDrop::DropEvent, dragDropKeyStates, effects, currentTarget, point2);
    }
    else
    {
        effects = 0;
    }

    return S_OK;
}

DpObject* UDropTarget::GetCurrentTarget(Point dragPoint, Point& targetPoint)
{
    DpObject to = NULL;
    HwndSource source = HwndSource.FromHwnd(_windowHandle);
    targetPoint = GetClientPointFromScreenPoint(dragPoint, source);
    if (source != NULL)
    {
    }
}

IDataObject* UDropTarget::GetDataObject(Object* data)
{
    IDataObject obj2 = NULL;
    if (data == NULL)
    {
        return obj2;
    }
    if (DynamicCast<IDataObject>(data))
    {
        return (IDataObject*)data;
    }
    return NULL;
}

bool UDropTarget::IsDataAvailable(IDataObject* dataObject)
{
    if (dataObject != NULL)
    {
        ;
    }
    return false;
}

void UDropTarget::OnDefaultDragEnter(DragEventArg& e)
{
    if (e.Data == NULL)
    {
        e.SetEffects(DragDropEffects::None);
    }
    else
    {
        if ((e.GetAllowedEffects() & DragDropEffects::Move) != DragDropEffects::None)
        {
            e.SetEffects(DragDropEffects::Move);
        }
        if ((e.GetKeyStates() & DragDropKeyStates::ControlKey) != DragDropKeyStates::None)
        {
            e.SetEffects(DragDropEffects::Copy);
        }
    }
}

void UDropTarget::OnDefaultDragOver(DragEventArg& e)
{
    if (e.Data == NULL)
    {
        e.SetEffects(DragDropEffects::None);
    }
    else
    {
        if ((e.GetAllowedEffects() & DragDropEffects::Move) != DragDropEffects::None)
        {
            e.SetEffects(DragDropEffects::Move);
        }
        if ((e.GetKeyStates() & DragDropKeyStates::ControlKey) != DragDropKeyStates::None)
        {
            e.SetEffects(DragDropEffects::Copy);
        }
    }
}

void UDropTarget::RaiseDragEvent(RoutedEvent* dragEvent, int dragDropKeyStates, int& effects, DpObject* target, Point targetPoint)
{
    Debug::Assert(_dataObject != NULL);
    Debug::Assert(target != NULL);
    DragEventArg e(_dataObject, (DragDropKeyStates)dragDropKeyStates, (DragDropEffects)effects, target, targetPoint);

    if (dragEvent == DragDrop::DragEnterEvent)
    {
        e.SetRoutedEvent(DragDrop::PreviewDragEnterEvent);
    }
    else if (dragEvent == DragDrop::DragOverEvent)
    {
        e.SetRoutedEvent(DragDrop::PreviewDragOverEvent);
    }
    else if (dragEvent == DragDrop::DragLeaveEvent)
    {
        e.SetRoutedEvent(DragDrop::PreviewDragLeaveEvent);
    }
    else if (dragEvent == DragDrop::DropEvent)
    {
        e.SetRoutedEvent(DragDrop::PreviewDropEvent);
    }

    target->RaiseEvent(e);

    if (!e.IsHandled())
    {
        e.SetRoutedEvent(dragEvent);
        target->RaiseEvent(e);
    }

    if (!e.IsHandled())
    {
        if (dragEvent == DragDrop::DragEnterEvent)
        {
            OnDefaultDragEnter(e);
        }
        else if (dragEvent == DragDrop::DragOverEvent)
        {
            OnDefaultDragOver(e);
        }
    }

    effects = (int)e.GetEffects();
}

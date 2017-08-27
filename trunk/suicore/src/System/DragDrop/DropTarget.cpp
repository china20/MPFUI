
#include "DropTarget.h"
#include "DropSource.h"

#include <System/Tools/Debug.h>
#include <System/Input/Mouse.h>
#include <System/Windows/RenderTarget.h>
#include <System/DragDrop/DragDrop.h>

DropTarget::DropTarget(HWND hwnd)
{
    _refCount = 1;
    _windowHandle = hwnd;
    _dataObject = NULL;
}

DropTarget::~DropTarget()
{
    if (NULL != _dataObject)
    {
        _dataObject->Release();
    }
}

STDMETHODIMP DropTarget::QueryInterface(REFIID riid, void **ppv)  
{  
    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = (IUnknown *)this;
    }
    else if (IsEqualIID(riid, IID_IDropTarget))
    {
        *ppv = (IDropTarget *)this;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();

    return NOERROR;
}  
  
STDMETHODIMP_(ULONG) DropTarget::AddRef()  
{  
    return InterlockedIncrement(&_refCount);  
}  
  
STDMETHODIMP_(ULONG) DropTarget::Release()  
{  
    ULONG lRef = InterlockedDecrement(&_refCount);  
    if (0 == lRef)  
    {  
        delete this;  
    }  
    return _refCount;  
}

STDMETHODIMP DropTarget::DragEnter(IDataObject *pDataObj, DWORD dragDropKeyStates, POINTL pt, __RPC__inout DWORD* effects)  
{  
    if (NULL == pDataObj)  
    {  
        return E_INVALIDARG;  
    }  

    HRESULT hr = S_OK;
  
    suic::Point point(pt.x, pt.y);  
    suic::Point point2;

    suic::Element* currTarget = GetCurrentTarget(point, point2);

    if (NULL != _dataObject)
    {
        _dataObject->Release();
    }

    _dataObject = pDataObj;

    if (NULL == _dataObject)
    {
        effects = 0;
        return S_FALSE;
    }
    else if (dynamic_cast<DataObject*>(pDataObj) == NULL)
    {
        /*suic::IDataStore* dataStore = new DataObjectStore(pDataObj);
        _dataObject = new DataObject();
        _dataObject->SetDataStore(dataStore);*/
    }
    else if (!IsDataAvailable(_dataObject))
    {
        effects = 0;
        return S_FALSE;
    }

    _dataObject->AddRef();
    _lastTarget = currTarget;

    if (currTarget != NULL)
    {
        RaiseDragEvent(suic::DragDrop::DragEnterEvent, dragDropKeyStates, effects, currTarget, point2);
        return hr;
    }
    else
    {
        hr = S_FALSE;
    }

    effects = 0;
    return hr;
}

STDMETHODIMP DropTarget::DragOver(DWORD dragDropKeyStates, POINTL pt, DWORD* effects)  
{
    HRESULT hr = S_OK;
    suic::Point point(pt.x, pt.y);  
    suic::Point point2;
    suic::Debug::Assert(_dataObject != NULL);
    suic::Element* currTarget = GetCurrentTarget(point, point2);

    if (currTarget != NULL)
    {
        if (currTarget != _lastTarget)
        {
            if (_lastTarget != NULL)
            {
                RaiseDragEvent(suic::DragDrop::DragLeaveEvent, dragDropKeyStates, effects, _lastTarget, point2);
            }

            RaiseDragEvent(suic::DragDrop::DragEnterEvent, dragDropKeyStates, effects, currTarget, point2);

            _lastTarget = currTarget;
            return hr;
        }

        RaiseDragEvent(suic::DragDrop::DragOverEvent, dragDropKeyStates, effects, currTarget, point2);
    }
    else
    {
        if (_lastTarget != NULL)
        {
            RaiseDragEvent(suic::DragDrop::DragLeaveEvent, dragDropKeyStates, effects, _lastTarget, point2);
        }

        _lastTarget = currTarget;
        effects = 0;
    }

    return hr;  
}

STDMETHODIMP DropTarget::DragLeave(void)  
{
    HRESULT hr = S_OK;

    if (_lastTarget != NULL)
    {
        DWORD effects = 0;

         RaiseDragEvent(suic::DragDrop::DragLeaveEvent, 0, &effects, _lastTarget, suic::Point(0.0, 0.0));
        _lastTarget = NULL;
    }

    if (_dataObject != NULL)
    {
        _dataObject->Release();
        _dataObject = NULL;
    }

    return hr;  
}

STDMETHODIMP DropTarget::Drop(IDataObject *data, DWORD dragDropKeyStates, POINTL pt, DWORD *effects)  
{
    suic::Debug::Assert(_dataObject != NULL);

    HRESULT hr = S_OK;
    suic::Point point(pt.x, pt.y);
    suic::Point point2;
    suic::Element* currTarget = NULL;
    DataObject* dataObject = dynamic_cast<DataObject*>(_dataObject);

    if ((_dataObject == NULL) || (dataObject != NULL && !IsDataAvailable(dataObject)))
    {
        effects = 0;
        return 1;
    }

    _lastTarget = NULL;
    currTarget = GetCurrentTarget(point, point2);

    if (currTarget != NULL)
    {
        RaiseDragEvent(suic::DragDrop::DropEvent, dragDropKeyStates, effects, currTarget, point2);
    }
    else
    {
        effects = 0;
    }

    return hr;
}

suic::Element* DropTarget::GetCurrentTarget(suic::Point dragPoint, suic::Point& targetPoint)
{
    suic::Element* toElem = NULL;
    suic::ElementPtr rootElem = suic::VisualHost::FromHwnd(HWNDTOHANDLE(_windowHandle));

    if (rootElem.get() != NULL)
    {
        suic::Rect rcWnd;
        ::GetWindowRect(_windowHandle, &rcWnd);
        targetPoint.x = dragPoint.x - rcWnd.left;
        targetPoint.y = dragPoint.y - rcWnd.top;

        toElem = suic::Mouse::HitTestElement(rootElem.get(), targetPoint);
    }

    return toElem;
}

IDataObject* DropTarget::GetDataObject(suic::Object* data)
{
    IDataObject* obj2 = NULL;

    if (data == NULL)
    {
        return obj2;
    }
    else if (dynamic_cast<IDataObject*>(data))
    {
        return (IDataObject*)data;
    }
    else
    {
        return NULL;
    }
}

bool DropTarget::IsDataAvailable(IDataObject* dataObject)
{
    if (dataObject != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DropTarget::OnDefaultDragEnter(suic::DragEventArg* e)
{
    if (e->GetData() == NULL)
    {
        e->SetEffects(suic::DragDropEffects::eDragNone);
    }
    else
    {
        if ((e->GetAllowedEffects() & suic::DragDropEffects::eDragMove) != suic::DragDropEffects::eDragNone)
        {
            e->SetEffects(suic::DragDropEffects::eDragMove);
        }
        if ((e->GetKeyStates() & suic::DragDropKeyStates::eDragKeyControlKey) != suic::DragDropKeyStates::eDragKeyNone)
        {
            e->SetEffects(suic::DragDropEffects::eDragCopy);
        }
    }
}

void DropTarget::OnDefaultDragOver(suic::DragEventArg* e)
{
    if (e->GetData() == NULL)
    {
        e->SetEffects(suic::DragDropEffects::eDragNone);
    }
    else
    {
        if ((e->GetAllowedEffects() & suic::DragDropEffects::eDragMove) != suic::DragDropEffects::eDragNone)
        {
            e->SetEffects(suic::DragDropEffects::eDragMove);
        }
        if ((e->GetKeyStates() & suic::DragDropKeyStates::eDragKeyControlKey) != suic::DragDropKeyStates::eDragKeyNone)
        {
            e->SetEffects(suic::DragDropEffects::eDragCopy);
        }
    }
}

HRESULT DropTarget::RaiseDragEvent(suic::RoutedEvent* dragEvent, int dragDropKeyStates, DWORD* effects, suic::Element* target, suic::Point targetPoint)
{
    suic::Debug::Assert(_dataObject != NULL);
    suic::Debug::Assert(target != NULL);
    suic::IDataStore* dataStore = NULL;
    DataObject* dataObject = dynamic_cast<DataObject*>(_dataObject);

    if (NULL == dataObject)
    {
        dataStore = new DataObjectStore(_dataObject);
    }
    else
    {
        dataStore = dataObject->GetDataStore();
    }

    suic::DragEventArg e(dataStore, (suic::DragDropKeyStates)dragDropKeyStates, (suic::DragDropEffects)*effects, target, targetPoint);

    if (dragEvent == suic::DragDrop::DragEnterEvent)
    {
        e.SetRoutedEvent(suic::DragDrop::PreviewDragEnterEvent);
    }
    else if (dragEvent == suic::DragDrop::DragOverEvent)
    {
        e.SetRoutedEvent(suic::DragDrop::PreviewDragOverEvent);
    }
    else if (dragEvent == suic::DragDrop::DragLeaveEvent)
    {
        e.SetRoutedEvent(suic::DragDrop::PreviewDragLeaveEvent);
    }
    else if (dragEvent == suic::DragDrop::DropEvent)
    {
        e.SetRoutedEvent(suic::DragDrop::PreviewDropEvent);
    }

    target->RaiseEvent(&e);

    if (!e.IsHandled())
    {
        e.SetRoutedEvent(dragEvent);
        target->RaiseEvent(&e);
    }

    if (!e.IsHandled())
    {
        if (dragEvent == suic::DragDrop::DragEnterEvent)
        {
            OnDefaultDragEnter(&e);
        }
        else if (dragEvent == suic::DragDrop::DragOverEvent)
        {
            OnDefaultDragOver(&e);
        }
    }

    *effects = (DWORD)e.GetEffects();

    if (e.IsHandled())
    {
        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}

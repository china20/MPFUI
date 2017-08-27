
#include "DropSource.h"
#include <System/DragDrop/DragDrop.h>

DropSource::DropSource(suic::Element* elem)
{
    _refCount = 1;
    _dragSource = elem;
    _dragSource->ref();
}

DropSource::~DropSource()
{
    _dragSource->unref();
}

STDMETHODIMP DropSource::QueryInterface(REFIID riid, void **ppv)  
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = (IUnknown *)this;
    }
    else if (IsEqualIID(riid, IID_IDropSource))
    {
        *ppv = (IDropSource *)this;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return NOERROR;
}  
  
STDMETHODIMP_(ULONG) DropSource::AddRef()  
{  
    return InterlockedIncrement(&_refCount);  
}  
  
STDMETHODIMP_(ULONG) DropSource::Release()  
{  
    ULONG lRef = InterlockedDecrement(&_refCount);  
    if (0 == lRef)  
    {  
        delete this;  
    }  
    return _refCount;  
}  
  
STDMETHODIMP DropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)  
{
    bool escapePressed = false;
    if (fEscapePressed)
    {
        escapePressed = true;
    }

    suic::QueryContinueDragEventArg e(escapePressed, (suic::DragDropKeyStates)grfKeyState);
    RaiseQueryContinueDragEvent(e);

    if (e.GetAction() != suic::DragAction::Continue)
    {
        if (e.GetAction() == suic::DragAction::Drop)
        {
            return DRAGDROP_S_DROP;
        }
        if (e.GetAction() == suic::DragAction::Cancel)
        {
            return DRAGDROP_S_CANCEL;
        }
    }

    return S_OK;  
}  

STDMETHODIMP DropSource::GiveFeedback(DWORD dwEffect)  
{  
    suic::GiveFeedbackEventArg e((suic::DragDropEffects)dwEffect, false);
    RaiseGiveFeedbackEvent(e);

    if (e.GetUseDefaultCursors())
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
    else
    {
        return S_OK;  
    }
}

void DropSource::OnDefaultGiveFeedback(suic::GiveFeedbackEventArg& e)
{
    e.SetUseDefaultCursors(true);
}

void DropSource::OnDefaultQueryContinueDrag(suic::QueryContinueDragEventArg& e)
{
    bool flag = false;
    e.SetAction(suic::DragAction::Continue);

    flag = (e.GetKeyStates() & suic::DragDropKeyStates::eDragKeyLeftMouseButton) == suic::DragDropKeyStates::eDragKeyNone;
    if (e.IsEscapePressed())
    {
        e.SetAction(suic::DragAction::Cancel);
    }
    else if (flag)
    {
        e.SetAction(suic::DragAction::Drop);
    }
}

void DropSource::RaiseGiveFeedbackEvent(suic::GiveFeedbackEventArg& e)
{
    e.SetRoutedEvent(suic::DragDrop::PreviewGiveFeedbackEvent);

    _dragSource->RaiseEvent(&e);
    e.SetRoutedEvent(suic::DragDrop::GiveFeedbackEvent);

    if (!e.IsHandled())
    {
        _dragSource->RaiseEvent(&e);
    }

    if (!e.IsHandled())
    {
        OnDefaultGiveFeedback(e);
    }
}

void DropSource::RaiseQueryContinueDragEvent(suic::QueryContinueDragEventArg& e)
{
    e.SetRoutedEvent(suic::DragDrop::PreviewQueryContinueDragEvent);
    _dragSource->RaiseEvent(&e);

    e.SetRoutedEvent(suic::DragDrop::QueryContinueDragEvent);

    if (!e.IsHandled())
    {
        _dragSource->RaiseEvent(&e);
    }

    if (!e.IsHandled())
    {
        OnDefaultQueryContinueDrag(e);
    }
}


UDropSource::UDropSource(Element* elem)
{
    _refCount = 1;
    _dragSource = elem;
}

UDropSource::~UDropSource()
{
}

STDMETHODIMP UDropSource::QueryInterface(REFIID riid, void **ppv)  
{  
    static const QITAB qit[] =  
    {  
        QITABENT(SdkDropSource, IDropSource),  
        { 0 }  
    };  
  
    return QISearch(this, qit, riid, ppv);  
}  
  
STDMETHODIMP_(ULONG) UDropSource::AddRef()  
{  
    return InterlockedIncrement(&_refCount);  
}  
  
STDMETHODIMP_(ULONG) UDropSource::Release()  
{  
    ULONG lRef = InterlockedDecrement(&_refCount);  
    if (0 == lRef)  
    {  
        delete this;  
    }  
    return _refCount;  
}  
  
STDMETHODIMP UDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)  
{
    bool escapePressed = false;
    if (fEscapePressed)
    {
        escapePressed = true;
    }

    QueryContinueDragEventArgs e(escapePressed, (DragDropKeyStates)grfkeyState);
    RaiseQueryContinueDragEvent(e);

    if (e.GetAction() != DragAction::Continue)
    {
        if (e.GetAction() == DragAction::Drop)
        {
            return DRAGDROP_S_DROP;
        }
        if (e.GetAction() == DragAction::Cancel)
        {
            return DRAGDROP_S_CANCEL;
        }
    }

    return S_OK;  
}  

STDMETHODIMP UDropSource::GiveFeedback(DWORD dwEffect)  
{  
    GiveFeedbackEventArgs e((DragDropEffects)effect, false);
    RaiseGiveFeedbackEvent(e);

    if (e.UseDefaultCursors())
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
    else
    {
        return S_OK;  
    }
}

void UDropSource::OnDefaultGiveFeedback(GiveFeedbackEventArg& e)
{
    e.SetUseDefaultCursors(true);
}

void UDropSource::OnDefaultQueryContinueDrag(QueryContinueDragEventArg& e)
{
    bool flag = false;
    e.SetAction(DragAction::Continue);

    flag = (e.GetKeyStates() & DragDropKeyStates::LeftMouseButton) == DragDropKeyStates::None;
    if (e.IsEscapePressed())
    {
        e.SetAction(DragAction::Cancel);
    }
    else if (flag)
    {
        e.SetAction(DragAction::Drop);
    }
}

void UDropSource::RaiseGiveFeedbackEvent(GiveFeedbackEventArg& e)
{
    e.SetRoutedEvent(DragDrop::PreviewGiveFeedbackEvent);

    _dragSource->RaiseEvent(e);
    e.SetRoutedEvent(DragDrop::GiveFeedbackEvent);

    if (!e.IsHandled())
    {
        _dragSource->RaiseEvent(e);
    }

    if (!e.IsHandled())
    {
        OnDefaultGiveFeedback(e);
    }
}

void UDropSource::RaiseQueryContinueDragEvent(QueryContinueDragEventArg& e)
{
    e.SetRoutedEvent(DragDrop::PreviewQueryContinueDragEvent);
    _dragSource->RaiseEvent(e);

    e.SetRoutedEvent(DragDrop::QueryContinueDragEvent);

    if (!e.IsHandled())
    {
        _dragSource->RaiseEvent(e);
    }

    if (!e.IsHandled())
    {
        OnDefaultQueryContinueDrag(e);
    }
}


#ifndef _UIDROPSOURCE_H_
#define _UIDROPSOURCE_H_

#include <System/Windows/Element.h>
#include <System/Windows/DragEventArg.h>

class DropSource : public IDropSource  
{  
public:  

    DropSource(suic::Element* elem);  
    ~DropSource();  

    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);  
    IFACEMETHODIMP_(ULONG) AddRef(void);  
    IFACEMETHODIMP_(ULONG) Release(void);  

    IFACEMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);  
    IFACEMETHODIMP GiveFeedback(DWORD dwEffect); 

private:

    void OnDefaultGiveFeedback(suic::GiveFeedbackEventArg& e);
    void OnDefaultQueryContinueDrag(suic::QueryContinueDragEventArg& e);

    void RaiseGiveFeedbackEvent(suic::GiveFeedbackEventArg& e);
    void RaiseQueryContinueDragEvent(suic::QueryContinueDragEventArg& e);

private:  

    volatile LONG _refCount;
    suic::Element* _dragSource;
};

#endif


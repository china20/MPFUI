
#ifndef _UIDATATARGET_H_
#define _UIDATATARGET_H_

#include <System/Windows/Element.h>

#include "DropSource.h"
#include "DataObject.h"

typedef DWORD DROPEFFECT;  

class DropTarget : public IDropTarget  
{  
public:

    DropTarget(HWND hwnd);  
    ~DropTarget();  

    BOOL RegisterDropTarget(HWND hWnd);  
    void RevokeDropTarget();  
    void AllowDragDrop(BOOL bAllowDrop = TRUE);

    DROPEFFECT FilterDropEffect(DWORD grfKeyState, DROPEFFECT dwEffectSrc);  

    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);  
    IFACEMETHODIMP_(ULONG) AddRef(void);  
    IFACEMETHODIMP_(ULONG) Release(void);  

    IFACEMETHODIMP DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);  
    IFACEMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);  
    IFACEMETHODIMP DragLeave(void);  
    IFACEMETHODIMP Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

private:

    void OnDefaultDragEnter(suic::DragEventArg* e);
    void OnDefaultDragOver(suic::DragEventArg* e);

    suic::Element* GetCurrentTarget(suic::Point dragPoint, suic::Point& targetPoint);
    IDataObject* GetDataObject(suic::Object* data);

    bool IsDataAvailable(IDataObject* dataObject);
    HRESULT RaiseDragEvent(suic::RoutedEvent* dragEvent, int dragDropKeyStates, DWORD* effects, suic::Element* target, suic::Point targetPoint);

private:  

    volatile LONG _refCount;              

    HWND _windowHandle;
    suic::Element* _lastTarget;
    IDataObject* _dataObject;
};

#endif

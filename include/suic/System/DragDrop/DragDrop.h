
#ifndef _UIDRAGDROP_H_
#define _UIDRAGDROP_H_

#include <System/Windows/Element.h>

namespace suic
{

class DragDrop : public DpObject
{
public:

    static RoutedEvent* DragEnterEvent;
    static RoutedEvent* DragLeaveEvent;
    static RoutedEvent* DragOverEvent;
    static RoutedEvent* DropEvent;
    static RoutedEvent* GiveFeedbackEvent;
    static RoutedEvent* PreviewDragEnterEvent;
    static RoutedEvent* PreviewDragLeaveEvent;
    static RoutedEvent* PreviewDragOverEvent;
    static RoutedEvent* PreviewDropEvent;
    static RoutedEvent* PreviewGiveFeedbackEvent;
    static RoutedEvent* PreviewQueryContinueDragEvent;
    static RoutedEvent* QueryContinueDragEvent;

    static bool StaticInit();

    RTTIOfAbstractClass(DragDrop)

public:

    static void AddDragEnterHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(DragEnterEvent, handler);
    }

    static void AddDragLeaveHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(DragLeaveEvent, handler);
    }

    static void AddDragOverHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(DragOverEvent, handler);
    }

    static void AddDropHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(DropEvent, handler);
    }

    static void AddGiveFeedbackHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(GiveFeedbackEvent, handler);
    }

    static void AddPreviewDragEnterHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(PreviewDragEnterEvent, handler);
    }

    static void AddPreviewDragLeaveHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(PreviewDragLeaveEvent, handler);
    }

    static void AddPreviewDragOverHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(PreviewDragOverEvent, handler);
    }

    static void AddPreviewDropHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(PreviewDropEvent, handler);
    }

    static void AddPreviewGiveFeedbackHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(PreviewGiveFeedbackEvent, handler);
    }

    static void AddPreviewQueryContinueDragHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(PreviewQueryContinueDragEvent, handler);
    }

    static void AddQueryContinueDragHandler(Element* elem, RoutedEventHandler* handler)
    {
        elem->AddHandler(QueryContinueDragEvent, handler);
    }

public:

    static void RegisterDropTarget(HWND windowHandle);
    static void RevokeDropTarget(HWND windowHandle);

    static DragDropEffects DoDragDrop(Element* dragSource, Object* data, DragDropEffects allowedEffects);
    static DragDropEffects DoDragDrop(Element* dragSource, IDataStore* dataStore, DragDropEffects allowedEffects);

    static bool IsValidDragAction(DragAction dragAction);
    static bool IsValidDragDropEffects(DragDropEffects dragDropEffects);
    static bool IsValidDragDropKeyStates(DragDropKeyStates dragDropKeyStates);

public:    
    
    static void RemoveDragEnterHandler(Element* elem, RoutedEventHandler* handler);
    static void RemoveDragLeaveHandler(Element* elem, RoutedEventHandler* handler);
    static void RemoveDragOverHandler(Element* elem, RoutedEventHandler* handler);
    static void RemoveDropHandler(Element* elem, RoutedEventHandler* handler);
    static void RemoveGiveFeedbackHandler(Element* elem, RoutedEventHandler* handler);
    static void RemovePreviewDragEnterHandler(Element* elem, RoutedEventHandler* handler);
    static void RemovePreviewDragLeaveHandler(Element* elem, RoutedEventHandler* handler);
    static void RemovePreviewDragOverHandler(Element* elem, RoutedEventHandler* handler);
    static void RemovePreviewDropHandler(Element* elem, RoutedEventHandler* handler);

    static void RemovePreviewGiveFeedbackHandler(Element* elem, RoutedEventHandler* handler);
    static void RemovePreviewQueryContinueDragHandler(Element* elem, RoutedEventHandler* handler);
    static void RemoveQueryContinueDragHandler(Element* elem, RoutedEventHandler* handler);
};

}

#endif

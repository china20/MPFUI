
#include <Extend/DragDrop/DragDrop.h>
#include <System/Tools/EventHelper.h>

bool DragDrop::StaticInit()
{
    DragEnterEvent = EventHelper::RegisterRoutedEvent("DragEnter", RoutingStrategy::Bubble, typeof(DragEventHandler), typeof(DragDrop));
    DragLeaveEvent = EventHelper::RegisterRoutedEvent("DragLeave", RoutingStrategy.Bubble, typeof(DragEventHandler), typeof(DragDrop));
    DragOverEvent = EventHelper::RegisterRoutedEvent("DragOver", RoutingStrategy.Bubble, typeof(DragEventHandler), typeof(DragDrop));
    DropEvent = EventHelper::RegisterRoutedEvent("Drop", RoutingStrategy.Bubble, typeof(DragEventHandler), typeof(DragDrop));
    GiveFeedbackEvent = EventHelper::RegisterRoutedEvent("GiveFeedback", RoutingStrategy.Bubble, typeof(GiveFeedbackEventHandler), typeof(DragDrop));
    PreviewDragEnterEvent = EventHelper::RegisterRoutedEvent("PreviewDragEnter", RoutingStrategy.Tunnel, typeof(DragEventHandler), typeof(DragDrop));
    PreviewDragLeaveEvent = EventHelper::RegisterRoutedEvent("PreviewDragLeave", RoutingStrategy.Tunnel, typeof(DragEventHandler), typeof(DragDrop));
    PreviewDragOverEvent = EventHelper::RegisterRoutedEvent("PreviewDragOver", RoutingStrategy.Tunnel, typeof(DragEventHandler), typeof(DragDrop));
    PreviewDropEvent = EventHelper::RegisterRoutedEvent("PreviewDrop", RoutingStrategy.Tunnel, typeof(DragEventHandler), typeof(DragDrop));
    PreviewGiveFeedbackEvent = EventHelper::RegisterRoutedEvent("PreviewGiveFeedback", RoutingStrategy.Tunnel, typeof(GiveFeedbackEventHandler), typeof(DragDrop));
    PreviewQueryContinueDragEvent = EventHelper::RegisterRoutedEvent("PreviewQueryContinueDrag", RoutingStrategy.Tunnel, typeof(QueryContinueDragEventHandler), typeof(DragDrop));
    QueryContinueDragEvent = EventHelper::RegisterRoutedEvent("QueryContinueDrag", RoutingStrategy.Bubble, typeof(QueryContinueDragEventHandler), typeof(DragDrop));

    return true;
}

DragDropEffects DragDrop::DoDragDrop(DpObject*  dragSource, Object* data, DragDropEffects allowedEffects)
{
    if (dragSource == NULL)
    {
        throw ArgumentNullException("dragSource");
    }

    if (data == NULL)
    {
        throw ArgumentNullException("data");
    }

    IDataObject* dataObject = DynamicCast<IDataObject>(data);

    return ::DoDragDrop(dragSource, dataObject, allowedEffects);
}

bool DragDrop::IsValidDragAction(DragAction dragAction)
{
    if (((dragAction != DragAction::Continue) && (dragAction != DragAction::Drop)) && (dragAction != DragAction::Cancel))
    {
        return false;
    }
    return true;
}

bool DragDrop::IsValidDragDropEffects(DragDropEffects dragDropEffects)
{
    int num = -2147483641;
    if ((dragDropEffects & ~num) != DragDropEffects::None)
    {
        return false;
    }
    return true;
}

bool DragDrop::IsValidDragDropKeyStates(DragDropKeyStates dragDropKeyStates)
{
    int num = 0x3f;
    if ((dragDropKeyStates & ~num) != DragDropKeyStates::None)
    {
        return false;
    }
    return true;
}

DragDropEffects DragDrop::DoDragDrop(Element* dragSource, DataObject* dataObject, DragDropEffects allowedEffects)
{
    int finalEffect = 0;
    DropSource* dropSource = new DropSource(dragSource);
    ::DoDragDrop(dataObject, dropSource, (int) allowedEffects, &finalEffect);
    return (DragDropEffects)finalEffect;
}

void DragDrop::RegisterDropTarget(HWND windowHandle)
{
    if ((windowHandle != NULL))
    {
        DropTarget* dropTarget = new DropTarget(windowHandle);
        ::RegisterDragDrop(windowHandle, dropTarget);
    }
}

void DragDrop::RemoveDragEnterHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(DragEnterEvent, handler);
}

void DragDrop::RemoveDragLeaveHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(DragLeaveEvent, handler);
}

void DragDrop::RemoveDragOverHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(DragOverEvent, handler);
}

void DragDrop::RemoveDropHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(DropEvent, handler);
}

void DragDrop::RemoveGiveFeedbackHandler(Element* elem, GiveFeedbackEventHandler handler)
{
    elem->RemoveHandler(GiveFeedbackEvent, handler);
}

void DragDrop::RemovePreviewDragEnterHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(PreviewDragEnterEvent, handler);
}

void DragDrop::RemovePreviewDragLeaveHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(PreviewDragLeaveEvent, handler);
}

void DragDrop::RemovePreviewDragOverHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(PreviewDragOverEvent, handler);
}

void DragDrop::RemovePreviewDropHandler(Element* elem, DragEventHandler* handler)
{
    elem->RemoveHandler(PreviewDropEvent, handler);
}

void DragDrop::RemovePreviewGiveFeedbackHandler(Element* elem, GiveFeedbackEventHandler* handler)
{
    elem->RemoveHandler(PreviewGiveFeedbackEvent, handler);
}

void DragDrop::RemovePreviewQueryContinueDragHandler(Element* elem, QueryContinueDragEventHandler* handler)
{
    elem->RemoveHandler(PreviewQueryContinueDragEvent, handler);
}

void DragDrop::RemoveQueryContinueDragHandler(Element* elem, QueryContinueDragEventHandler* handler)
{
    elem->RemoveHandler(QueryContinueDragEvent, handler);
}

void DragDrop::RevokeDropTarget(HWND windowHandle)
{
    if (windowHandle != 0)
    {
        ::RevokeDragDrop(windowHandle);
    }
}

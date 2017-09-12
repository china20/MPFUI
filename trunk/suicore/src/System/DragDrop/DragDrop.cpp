
#include "DropTarget.h"
#include "DropSource.h"
#include "DataObject.h"

#include <System/DragDrop/DragDrop.h>
#include <System/Tools/EventHelper.h>

namespace suic
{

RoutedEvent* DragDrop::DragEnterEvent;
RoutedEvent* DragDrop::DragLeaveEvent;
RoutedEvent* DragDrop::DragOverEvent;
RoutedEvent* DragDrop::DropEvent;
RoutedEvent* DragDrop::GiveFeedbackEvent;
RoutedEvent* DragDrop::PreviewDragEnterEvent;
RoutedEvent* DragDrop::PreviewDragLeaveEvent;
RoutedEvent* DragDrop::PreviewDragOverEvent;
RoutedEvent* DragDrop::PreviewDropEvent;
RoutedEvent* DragDrop::PreviewGiveFeedbackEvent;
RoutedEvent* DragDrop::PreviewQueryContinueDragEvent;
RoutedEvent* DragDrop::QueryContinueDragEvent;

ImplementRTTIOfClass(DragDrop, DpObject)

void DragDrop::StaticInit()
{
    if (NULL == DragEnterEvent)
    {
        DragEnterEvent = EventHelper::RegisterRoutedEvent("DragEnter", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        DragLeaveEvent = EventHelper::RegisterRoutedEvent("DragLeave", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        DragOverEvent = EventHelper::RegisterRoutedEvent("DragOver", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        DropEvent = EventHelper::RegisterRoutedEvent("Drop", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        GiveFeedbackEvent = EventHelper::RegisterRoutedEvent("GiveFeedback", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
        PreviewDragEnterEvent = EventHelper::RegisterRoutedEvent("PreviewDragEnter", RoutingStrategy::Tunnel, RoutedEventHandler::RTTIType(), RTTIType());
        PreviewDragLeaveEvent = EventHelper::RegisterRoutedEvent("PreviewDragLeave", RoutingStrategy::Tunnel, RoutedEventHandler::RTTIType(), RTTIType());
        PreviewDragOverEvent = EventHelper::RegisterRoutedEvent("PreviewDragOver", RoutingStrategy::Tunnel, RoutedEventHandler::RTTIType(), RTTIType());
        PreviewDropEvent = EventHelper::RegisterRoutedEvent("PreviewDrop", RoutingStrategy::Tunnel, RoutedEventHandler::RTTIType(), RTTIType());
        PreviewGiveFeedbackEvent = EventHelper::RegisterRoutedEvent("PreviewGiveFeedback", RoutingStrategy::Tunnel, RoutedEventHandler::RTTIType(), RTTIType());
        PreviewQueryContinueDragEvent = EventHelper::RegisterRoutedEvent("PreviewQueryContinueDrag", RoutingStrategy::Tunnel, RoutedEventHandler::RTTIType(), RTTIType());
        QueryContinueDragEvent = EventHelper::RegisterRoutedEvent("QueryContinueDrag", RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());
    }
}

DragDropEffects DragDrop::DoDragDrop(Element* dragSource, Object* data, DragDropEffects allowedEffects)
{
    if (dragSource == NULL)
    {
        throw ArgumentNullException("dragSource", __FILELINE__);
    }

    if (data == NULL)
    {
        throw ArgumentNullException("data", __FILELINE__);
    }

    DWORD dwEffects = 0;
    DataObject* dataObject = dynamic_cast<DataObject*>(data);
    if (NULL == dataObject)
    {
        IDataStore* dataStore = dynamic_cast<IDataStore*>(data);
        if (dataStore == NULL)
        {
            dataStore = new DataStore(data);
        }
        dataObject = new DataObject();
        dataObject->SetDataStore(dataStore);
    }

    DropSource* dropSource = new DropSource(dragSource);

    ::DoDragDrop(dataObject, dropSource, allowedEffects, &dwEffects);

    return (DragDropEffects)dwEffects;
}

bool DragDrop::IsValidDragAction(DragAction dragAction)
{
    if (((dragAction != DragAction::Continue) && 
        (dragAction != DragAction::Drop)) && 
        (dragAction != DragAction::Cancel))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool DragDrop::IsValidDragDropEffects(DragDropEffects dragDropEffects)
{
    int num = -2147483641;
    if ((dragDropEffects & ~num) != DragDropEffects::eDragNone)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool DragDrop::IsValidDragDropKeyStates(DragDropKeyStates dragDropKeyStates)
{
    int num = 0x3f;
    if ((dragDropKeyStates & ~num) != DragDropKeyStates::eDragKeyNone)
    {
        return false;
    }
    else
    {
        return true;
    }
}

DragDropEffects DragDrop::DoDragDrop(Element* dragSource, IDataStore* dataStore, DragDropEffects allowedEffects)
{
    DWORD finalEffect = 0;
    DropSource* dropSource = new DropSource(dragSource);
    DataObject* dataObj = new DataObject();

    dataObj->SetDataStore(dataStore);

    //
    // 调用此函数开始拖动
    //
    ::DoDragDrop(dataObj, dropSource, allowedEffects, &finalEffect);
    return (DragDropEffects)finalEffect;
}

void DragDrop::RegisterDropTarget(HWND windowHandle)
{
    //
    // 注册此窗口允许接收OLE拖动
    //
    if ((windowHandle != NULL))
    {
        DropTarget* dropTarget = new DropTarget(windowHandle);
        ::RegisterDragDrop(windowHandle, dropTarget);
    }
}

void DragDrop::RemoveDragEnterHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(DragEnterEvent, handler);
}

void DragDrop::RemoveDragLeaveHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(DragLeaveEvent, handler);
}

void DragDrop::RemoveDragOverHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(DragOverEvent, handler);
}

void DragDrop::RemoveDropHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(DropEvent, handler);
}

void DragDrop::RemoveGiveFeedbackHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(GiveFeedbackEvent, handler);
}

void DragDrop::RemovePreviewDragEnterHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(PreviewDragEnterEvent, handler);
}

void DragDrop::RemovePreviewDragLeaveHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(PreviewDragLeaveEvent, handler);
}

void DragDrop::RemovePreviewDragOverHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(PreviewDragOverEvent, handler);
}

void DragDrop::RemovePreviewDropHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(PreviewDropEvent, handler);
}

void DragDrop::RemovePreviewGiveFeedbackHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(PreviewGiveFeedbackEvent, handler);
}

void DragDrop::RemovePreviewQueryContinueDragHandler(Element* elem, RoutedEventHandler* handler)
{
    elem->RemoveHandler(PreviewQueryContinueDragEvent, handler);
}

void DragDrop::RemoveQueryContinueDragHandler(Element* elem, RoutedEventHandler* handler)
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

}


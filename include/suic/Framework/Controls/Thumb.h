// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Thumb.h
// ��  �ܣ��϶��߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UITHUMB_H_
#define _UITHUMB_H_

#include <Framework/Controls/DragEvent.h>
#include <Framework/Controls/Control.h>

namespace suic
{

/// <summary>
/// ʵ����ҷ�ؼ�
/// </summary>
class SUICORE_API Thumb : public Control
{
public:

    static RoutedEvent* DragStartedEvent;
    static RoutedEvent* DragDeltaEvent;
    static RoutedEvent* DragCompletedEvent;

    static DpProperty* IsDraggingProperty;

    Thumb();
    virtual ~Thumb();

    static bool StaticInit();

    RTTIOfClass(Thumb)

    void AddDragStarted(RoutedEventHandler* handler);
    void AddDragDelta(RoutedEventHandler* handler);
    void AddDragCompleted(RoutedEventHandler* handler);

    void RemoveDragStarted(RoutedEventHandler handler);
    void RemoveDragDelta(RoutedEventHandler handler);
    void RemoveDragCompleted(RoutedEventHandler handler);

    bool IsDragging();

protected:

    Size OnMeasure(const Size& constraintSize);

    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);
    void OnLostMouseCapture(MouseButtonEventArg* e);

private:

    void SetIsDraging(bool val);
    void HandleMouseLeftButtonUp(Point pt);

protected:

    static Point _prevPoint;
    static Point _originPoint;
};

inline bool Thumb::IsDragging()
{
    return GetValue(IsDraggingProperty)->ToBool();
}

}

#endif

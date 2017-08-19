// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Control.h
// ��  �ܣ�������Ŀ�һ���û��ؼ��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UICONTROL_H_
#define _UICONTROL_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Windows/RoutedCommand.h>
#include <Framework/Controls/ControlTemplate.h>

namespace suic
{

enum OverflowMode
{
    AsNeeded,
    Always,
    Never,
};

/// <summary>
/// �ؼ�����࣬����̳�֧��tab���л�
/// </summary>
class SUICORE_API Control : public FrameworkElement
{
public:

    static DpProperty* BackgroundProperty;
    static DpProperty* BorderBrushProperty;
    static DpProperty* BorderThicknessProperty;
    static DpProperty* ForegroundProperty;
    static DpProperty* PaddingProperty;

    static DpProperty* CornerRadiusProperty;

    static DpProperty* FontFamilyProperty;
    static DpProperty* FontSizeProperty;
    static DpProperty* FontWeightProperty;
    static DpProperty* FontStyleProperty;
    static DpProperty* FontStretchProperty;

    static DpProperty* TemplateProperty;

    static DpProperty* HorizontalContentAlignmentProperty;
    static DpProperty* VerticalContentAlignmentProperty;
    static DpProperty* TabIndexProperty;
    static DpProperty* IsTabStopProperty;

    static RoutedEvent* MouseDoubleClickEvent;
    static RoutedEvent* PreviewMouseDoubleClickEvent;
    
    Control();
    virtual ~Control();

    RTTIOfClass(Control)

    static bool StaticInit();

// ���Է��ʶ���
public:

    enum ItemBoolFlags : Uint16
    {
        ContentIsNotLogical = 0x1,
        HeaderIsNotLogical = 0x2,
        HasItemsFlag = 0x4,
        HasHeaderFlag = 0x8,
        IsExpandedFlag = 0x10,
        IsSelectedFlag = 0x20,
        HasContentFlag = 0x40,
        CantCachedFocusedItem = 0x80,
        ContentEqualItem = 0x100,
        HeaderEqualItem = 0x200,
        ContainsSelection = 0x800, 
        ScrollHostValid = 0x1000,
    };

    ControlTemplate* GetTemplate();
    void SetTemplate(ControlTemplate* val);

    FrameworkTemplate* GetTemplateInternal();

    FrameworkTemplate* GetTemplateCache();
    void SetTemplateCache(FrameworkTemplate* val);

    FrameworkElement* GetCommandTarget();

    fRect GetCornerRadius();
    void SetCornerRadius(fRect val);

    bool IsTabStop();
    void SetTabStop(bool val);

    int GetTabIndex();
    void SetTabIndex(int val);

    String GetFontFamily();
    void SetFontFamily(String val);

    int GetFontSize();
    void SetFontSize(int val);

    int GetFontWeight();
    void SetFontWeight(int val);

    int GetFontStyle();
    void SetFontStyle(int val);

    int GetFontStretch();
    void SetFontStretch(int val);

    Rect GetBorderThickness();
    void SetBorderThickness(Rect border);

    /// <summary>
    /// ��ȡԪ���ڱ߾�
    /// </summary>
    /// <remarks>
    /// Ԫ����߾���Ϊ�������Сһ���֣����������Ⱦ�����ڡ�
    /// </remarks>
    /// <returns>Ԫ���ڱ߾�</returns> 
    Rect GetPadding();
    void SetPadding(Rect val);

    Brush* GetBackground();
    void SetBackground(Brush* val);

    Brush* GetForeground();
    void SetForeground(Brush* val);

    Brush* GetBorderBrush();
    void SetBorderBrush(Brush* val);

    /// <summary>
    ///  ȡ��Ԫ�����ݵ�ˮƽ���뷽ʽ
    /// </summary>
    /// <remarks>
    ///  ���뷽ʽ��Left��Center��Right��
    /// </remarks>
    /// <returns>����ˮƽ���뷽ʽ</returns> 
    int GetHorizontalContentAlignment();
    void SetHorizontalContentAlignment(int val);

    /// <summary>
    ///  ȡ��Ԫ�����ݵĴ�ֱ���뷽ʽ
    /// </summary>
    /// <remarks>
    ///  ���뷽ʽ��Top��Center��Bottom��
    /// </remarks>
    /// <returns>��ֱ���ݶ��뷽ʽ</returns> 
    int GetVerticalContentAlignment();
    void SetVerticalContentAlignment(int val);

    void AddPreviewMouseDoubleClick(MouseButtonEventHandler* h);
    void RemovePreviewMouseDoubleClick(MouseButtonEventHandler* h);

    void AddMouseDoubleClick(MouseButtonEventHandler* h);
    void RemoveMouseDoubleClick(MouseButtonEventHandler* h);

    void WriteItemFlags(Uint16 flag, bool val);
    bool ReadItemFlags(Uint16 flag) const;

public:

    static void HandleDoubleClick(Element* sender, MouseButtonEventArg* e);
    static void OnTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e);

    virtual void OnMouseDoubleClick(MouseButtonEventArg* e);
    virtual void OnPreviewMouseDoubleClick(MouseButtonEventArg* e);

    virtual void OnTemplateChanged(ControlTemplate* oldTemp, ControlTemplate* newTemp);

protected:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

    void OnMouseDown(MouseButtonEventArg* e);
    void OnPreviewMouseDown(MouseButtonEventArg* e);

    void OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate);

protected:

    Uint16 _itemFlags;
    ControlTemplate* _templateCache; 
};

inline void Control::WriteItemFlags(Uint16 flag, bool val)
{
    if (val)
    {
        _itemFlags |= flag;
    }
    else
    {
        _itemFlags &= ~flag;
    }
}

inline bool Control::ReadItemFlags(Uint16 flag) const
{
    return (flag == (flag & _itemFlags));
}

inline ControlTemplate* Control::GetTemplate()
{
    return _templateCache;
}

inline void Control::SetTemplate(ControlTemplate* val)
{ 
    SetValue(TemplateProperty, val);
}

inline FrameworkTemplate* Control::GetTemplateInternal()
{ 
    return GetTemplate();
}

inline FrameworkTemplate* Control::GetTemplateCache()
{
    return _templateCache;
}

inline void Control::SetTemplateCache(FrameworkTemplate* val)
{
    _templateCache = RTTICast<ControlTemplate>(val);
}

}

#endif

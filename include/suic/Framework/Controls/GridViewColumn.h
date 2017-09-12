// 华勤科技版权所有 2008-2022
// 
// 文件名：GridViewColumn.h
// 功  能：列表视图列。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEWCOLUMN_H_
#define _UIGRIDVIEWCOLUMN_H_

#include <System/Windows/DpObject.h>
#include <Framework/Controls/DataTemplate.h>

namespace suic
{

class SUICORE_API GridViewColumn : public DpObject
{
public:

    RTTIOfClass(GridViewColumn)

    static DpProperty* WidthProperty;
    static DpProperty* MinWidthProperty;
    static DpProperty* MaxWidthProperty;
    static DpProperty* HeaderProperty;
    static DpProperty* HeaderTemplateProperty;
    static DpProperty* CellTemplateProperty;

    static void OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeaderPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnCellTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnHeaderTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e);

    static void StaticInit();

    GridViewColumn();
    ~GridViewColumn();

    Binding* GetDisplayMember();
    void SetDisplayMember(Binding* val);

    void SetAttribute(const String& key, Object* val);

public:

    Object* GetHeader();
    void SetHeader(Object* header);

    int GetColumnIndex();
    void SetColumnIndex(int val);

    int GetActualWidth();
    void SetActualWidth(int val);

    int GetWidth();
    void SetWidth(int val);

    int GetMinWidth();
    void SetMinWidth(int val);

    int GetMaxWidth();
    void SetMaxWidth(int val);

    DataTemplate* GetHeaderTemplate();
    void SetHeaderTemplate(DataTemplate* temp);

    DataTemplate* GetCellTemplate();
    void SetCellTemplate(DataTemplate* temp);

private:

    int _columnIndex;
    int _actualWidth;
    
    int _iWidth;
    int _iMinWidth;
    int _iMaxWidth;

    Object* _header;
    Binding* _displayMember;

    DataTemplate* _headerTemplate;
    DataTemplate* _cellTemplate;
};

inline Binding* GridViewColumn::GetDisplayMember()
{
    return _displayMember;
}

inline void GridViewColumn::SetDisplayMember(Binding* val)
{
    SETREFOBJ(_displayMember, val);
}

inline Object* GridViewColumn::GetHeader()
{
    return _header;
}

inline void GridViewColumn::SetHeader(Object* header)
{
    SetValue(HeaderProperty, header);
}

inline int GridViewColumn::GetColumnIndex()
{
    return _columnIndex;
}

inline void GridViewColumn::SetColumnIndex(int val)
{
    _columnIndex = val;
}

inline int GridViewColumn::GetActualWidth()
{
    return _actualWidth;
}

inline void GridViewColumn::SetActualWidth(int val)
{
    _actualWidth = val;
}

inline int GridViewColumn::GetWidth()
{
    return _iWidth;
}

inline void GridViewColumn::SetWidth(int val)
{
    SetValue(WidthProperty, new Integer(val));
}

inline int GridViewColumn::GetMinWidth()
{
    return _iMinWidth;
}

inline void GridViewColumn::SetMinWidth(int val)
{
    SetValue(MinWidthProperty, new Integer(val));
}

inline int GridViewColumn::GetMaxWidth()
{
    return _iMaxWidth;
}

inline void GridViewColumn::SetMaxWidth(int val)
{
    SetValue(MaxWidthProperty, new Integer(val));
}

inline DataTemplate* GridViewColumn::GetHeaderTemplate()
{
    return _headerTemplate;
}

inline void GridViewColumn::SetHeaderTemplate(DataTemplate* temp)
{
    SetValue(HeaderTemplateProperty, temp);
}

inline DataTemplate* GridViewColumn::GetCellTemplate()
{
    return _cellTemplate;
}

inline void GridViewColumn::SetCellTemplate(DataTemplate* temp)
{
    SetValue(CellTemplateProperty, temp);
}

}

#endif

// 华勤科技版权所有 2008-2022
// 
// 文件名：ComboBox.h
// 功  能：实现组合框逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UICOMBOBOX_H_
#define _UICOMBOBOX_H_

#include <Extend/suiwgxconfig.h>

#include <Framework/Controls/Selector.h>
#include <Framework/Controls/Popup.h>
#include <Framework/Controls/TextEditor.h>

namespace suic
{

/// <summary>
/// 组合框下拉框打开时事件
/// </summary>
class SUIWGX_API DropDownEventArg : public EventArg
{
public:

    DropDownEventArg(PopupPtr popup);

    PopupPtr GetPopup();
    
protected:

    PopupPtr _popup;
};

/// <summary>
/// 组合框，由两个子控件组成（TextElement和ListBox）
/// </summary>
class SUIWGX_API ComboBox : public Selector
{
public:

    EventHandler DropDownClosed;
    EventHandler DropDownOpened;

    static DpProperty* TextProperty;
    static DpProperty* IsEditableProperty;
	static DpProperty* IsReadOnlyProperty;
    static DpProperty* IsDropDownOpenProperty;
    static DpProperty* MaxDropDownHeightProperty;
    static DpProperty* SelectionBoxItemProperty;
    static DpProperty* SelectionBoxItemTemplateProperty;

    static void OnTextPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsEditableChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsDropDownOpenChanged(DpObject* d, DpPropChangedEventArg* e);

    static bool StaticInit();

    RTTIOfClass(ComboBox)

    ComboBox();
    virtual ~ComboBox();

    void Dispose();

    int FindIndex(const String& text);
    int FindIndex(Object* item);

    void SetReadOnly(bool bOnly);
    bool IsReadOnly();

    void SetEditable(bool bEdit);
    bool IsEditable();

    bool IsDropDownOpen();
    void SetIsDropDownOpen(bool val);

    /// <summary>
    ///  设置下拉框的高度
    /// </summary>
    /// <param name="height">下拉框高度</param>
    /// <returns>无</returns>
    void SetDropDownHeight(int height);
    int GetDropDownHeight();

    TextEditor* GetTextBoxSite() const;
    Object* GetSelectionBoxItem();

    void SetSelectionBoxItemTemplate(DataTemplate* temp);
    DataTemplate* GetSelectionBoxItemTemplate();

    /// <summary>
    ///  设置元素文本，默认会调_textBox进行设置
    /// </summary>
    /// <param name="text">文本信息</param>
    /// <returns>无</returns>
    void SetText(const String & text);
    String GetText();

    /// <summary>
    ///  检测下拉框是否已经打开
    /// </summary>
    /// <returns>true：打开；false：关闭</returns>
    bool StaysOpenOnEdit();

    bool OnMessageFilter(Object* sender, MessageParam* mp, bool& interrupt);

public:

    bool IsItemItsOwnContainer(Object* item);
    Element* GetContainerForItemOverride();

    void OnTextBoxCursor(Element*, CursorEventArg* e);

    virtual void OnDropDownClosed(DropDownEventArg* e);
    virtual void OnDropDownOpened(DropDownEventArg* e);

    void OnSelectionChanged(SelectionChangedEventArg* e);
    void OnTextSiteTextChanged(DpObject* sender, RoutedEventArg* e);

protected:

    void OnKeyDown(KeyboardEventArg* e);
    void OnTextInput(KeyboardEventArg* e);
    void OnGotFocus(RoutedEventArg* e);

    void OnPreviewSetCursor(CursorEventArg* e);
    void OnPreviewMouseLeftButtonDown(MouseButtonEventArg* e);

    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);
    void OnPreviewMouseMove(MouseButtonEventArg* e);

    friend class ComboBoxItem;

protected:

    void RaisedSelectionChanged(Object* sender);

    void OnApplyTemplate();

    void Update(bool bFocus);
    bool BlockRouteEvent();
    void SetSelectionBoxItem(Object* temp);

protected:

    bool _updatingText;
    TextEditor* _textInput;
    // 下拉弹出框,用于list的容器
    Popup* _dropDownPopup;
};

typedef shared<ComboBox> ComboBoxPtr;

inline int ComboBox::GetDropDownHeight()
{
    return GetValue(MaxDropDownHeightProperty)->ToInt();
}

inline void ComboBox::SetSelectionBoxItem(Object* item)
{
    SetValue(SelectionBoxItemProperty, item);
}

inline Object* ComboBox::GetSelectionBoxItem()
{
    return GetValue(SelectionBoxItemProperty);
}

inline void ComboBox::SetSelectionBoxItemTemplate(DataTemplate* temp)
{
    SetValue(SelectionBoxItemTemplateProperty, temp);
}

inline DataTemplate* ComboBox::GetSelectionBoxItemTemplate()
{
    return RTTICast<DataTemplate>(GetValue(SelectionBoxItemTemplateProperty));
}

}

#endif

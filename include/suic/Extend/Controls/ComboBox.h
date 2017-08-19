// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ComboBox.h
// ��  �ܣ�ʵ����Ͽ��߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
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
/// ��Ͽ��������ʱ�¼�
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
/// ��Ͽ��������ӿؼ���ɣ�TextElement��ListBox��
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
    ///  ����������ĸ߶�
    /// </summary>
    /// <param name="height">������߶�</param>
    /// <returns>��</returns>
    void SetDropDownHeight(int height);
    int GetDropDownHeight();

    TextEditor* GetTextBoxSite() const;
    Object* GetSelectionBoxItem();

    void SetSelectionBoxItemTemplate(DataTemplate* temp);
    DataTemplate* GetSelectionBoxItemTemplate();

    /// <summary>
    ///  ����Ԫ���ı���Ĭ�ϻ��_textBox��������
    /// </summary>
    /// <param name="text">�ı���Ϣ</param>
    /// <returns>��</returns>
    void SetText(const String & text);
    String GetText();

    /// <summary>
    ///  ����������Ƿ��Ѿ���
    /// </summary>
    /// <returns>true���򿪣�false���ر�</returns>
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
    // ����������,����list������
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

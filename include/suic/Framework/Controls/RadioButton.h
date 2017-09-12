// 华勤科技版权所有 2008-2022
// 
// 文件名：RadioButton.h
// 功  能：按钮基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UIRADIOBUTTON_H_
#define _UIRADIOBUTTON_H_

#include <System/Tools/Collection.h>
#include <Framework/Controls/ToggleButton.h>

namespace suic
{

typedef suic::Array<WeakRef> RadioGroups;

class RBGroupDic : public StringDic<RadioGroups*>
                 , public Object
{
public:

    RBGroupDic();
    ~RBGroupDic();
};

/// <summary>
/// RadioButton。
/// </summary>
class SUICORE_API RadioButton : public ToggleButton
{
public:

    static DpProperty* GroupNameProperty;

    static UncommonField<RBGroupDic> GroupNamesField;

    static RBGroupDic* GetRadioGroups(RadioButton* radioBtn);
    static void OnGroupNameChanged(DpObject* d, DpPropChangedEventArg* e);
    static void Register(String groupName, RadioButton* radioBtn);
    static void Unregister(String groupName, RadioButton* radioBtn);

    static void StaticInit();

    RadioButton();
    virtual ~RadioButton();

    RTTIOfClass(RadioButton)

    String GetGroupName();
    void SetGroupName(String val);

protected:

    void OnLoaded(LoadedEventArg* e);

    void OnToggle();
    void OnChecked(RoutedEventArg* e);

    void PurgeChecked();

    static void PurgeDead(Array<WeakRef>& elems, Object* elem);
};

typedef shared<RadioButton> RadioButtonPtr;

inline String RadioButton::GetGroupName()
{
    return GetValue(GroupNameProperty)->ToString();
}

inline void RadioButton::SetGroupName(String val)
{
    SetValue(GroupNameProperty, new OString(val));
}

}

#endif

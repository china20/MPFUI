
#include <Framework/Controls/RadioButton.h>
#include <System/Tools/VisualTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(RadioButton, ToggleButton)

DpProperty* RadioButton::GroupNameProperty;
UncommonField<RBGroupDic> RadioButton::GroupNamesField;

RBGroupDic::RBGroupDic()
{
}

RBGroupDic::~RBGroupDic()
{

}

RadioButton::RadioButton()
{
    
}

RadioButton::~RadioButton()
{
    RadioButton::Unregister(GetGroupName(), this);
}

void RadioButton::StaticInit()
{
    if (GroupNameProperty == NULL)
    {
        ToggleButton::StaticInit();
        GroupNameProperty = DpProperty::Register(_U("GroupName"), RTTIType(), OString::RTTIType()
            , DpPropMemory::GetPropMeta(OString::EmptyString, PropMetadataOptions::AffectsNone, &RadioButton::OnGroupNameChanged));
    }
}

void RadioButton::OnLoaded(LoadedEventArg* e)
{
    ToggleButton::OnLoaded(e);
    Register(GetGroupName(), this);
    if (GetChecked() == Boolean::True)
    {
        PurgeChecked();
    }
}

void RadioButton::OnGroupNameChanged(DpObject* d, DpPropChangedEventArg* e)
{
    RadioButton* rBtn = DynamicCast<RadioButton>(d);
    String newValue = e->GetNewValue()->ToString();
    String oldValue = e->GetOldValue()->ToString();

    if (!newValue.Equals(oldValue))
    {
        Unregister(oldValue, rBtn);
        Register(newValue, rBtn);
    }
}

void RadioButton::PurgeDead(Array<WeakRef, true>& elems, Object* elem)
{
    int index = 0;

    while (index < elems.Length())
    {
        WeakRef& refw = elems[index];
        Object* target = refw.GetTarget();

        if (target == NULL || target == elem)
        {
            elems.RemoveAt(index);
        }
        else
        {
            index++;
        }
    }
}

RBGroupDic* RadioButton::GetRadioGroups(RadioButton* radioBtn)
{
    RBGroupDic* rbGrp = NULL;

    if (NULL != radioBtn)
    {
        Element* root = NULL;
        Element* parent = radioBtn->GetUIParent();
        while (parent != NULL)
        {
            if (NULL == parent->GetUIParent())
            {
                root = parent;
                break;
            }
            parent = parent->GetUIParent();
        }

        if (NULL != root)
        {
            rbGrp = GroupNamesField.GetValue(root);

            if (NULL == rbGrp)
            {
                rbGrp = new RBGroupDic();
                GroupNamesField.SetValue(root, rbGrp);
            }
        }
    }

    return rbGrp;
}

void RadioButton::Register(String groupName, RadioButton* radioBtn)
{
    RadioGroups* rGrp = NULL;
    RBGroupDic* rbGrp = GetRadioGroups(radioBtn);

    if (NULL != rbGrp)
    {
        rbGrp->TryGetValue(groupName, rGrp);

        if (rGrp != NULL)
        {
            PurgeDead(*rGrp, radioBtn);
        }
        else
        {
            rGrp = new RadioGroups();
            rbGrp->Add(groupName, rGrp);
        }

        rGrp->Add(WeakRef(radioBtn));
    }
}

void RadioButton::Unregister(String groupName, RadioButton* radioBtn)
{
    RadioGroups* elems = NULL;
    RBGroupDic* rbGrp = GetRadioGroups(radioBtn);
    
    if (NULL != rbGrp)
    {
        rbGrp->TryGetValue(groupName, elems);
    }

    if (elems != NULL)
    {
        PurgeDead(*elems, radioBtn);
        if (elems->Length() == 0)
        {
            delete elems;
            rbGrp->Remove(groupName);
        }
    }
}

void RadioButton::OnToggle()
{
    if (GetChecked() == Boolean::False)
    {
        ToggleButton::OnToggle();
    }
}

void RadioButton::OnChecked(RoutedEventArg* e)
{
    PurgeChecked();
    ToggleButton::OnChecked(e);
}

void RadioButton::PurgeChecked()
{
    RadioGroups* rGrp = NULL;
    RBGroupDic* rbGrp = GetRadioGroups(this);

    if (NULL != rbGrp)
    {
        rbGrp->TryGetValue(GetGroupName(), rGrp);

        //
        // 可能窗口宿主已经改变
        //
        if (NULL == rGrp)
        {
            Register(GetGroupName(), this);
            rbGrp->TryGetValue(GetGroupName(), rGrp);
        }
    }

    if (rGrp != NULL)
    {
        Element* root = VisualTreeOp::GetVisualRoot(this);

        for (int i = 0; i < rGrp->Length(); ++i)
        {
            RadioButton* rBtn(rGrp->GetItem(i).GetElement<RadioButton>());

            if (rBtn != NULL)
            {
                if (rBtn != this)
                {
                    if (rBtn->GetChecked()  == Boolean::True)
                    {
                        rBtn->SetChecked(Boolean::False);
                    }
                }
            }
        }
    }
}

}

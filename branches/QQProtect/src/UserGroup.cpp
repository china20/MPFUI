
#include "stdafx.h"
#include "UserGroup.h"

ImplementRTTIOfClass(UserGroup, NotifyPropChanged)

UserTemplateSelector::UserTemplateSelector(suic::DataTemplate* t1, suic::DataTemplate* t2)
{
    userGrp = t1;
    userId = t2;
}

DataTemplate* UserTemplateSelector::SelectTemplate(Object* item, FrameworkElement* container)
{
    if (item->GetRTTIType() == UserGroup::RTTIType())
    {
        return userGrp;
    }
    else if (item->GetRTTIType() == UserInfo::RTTIType())
    {
        return userId;
    }

    return NULL;
}

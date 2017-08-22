//////////////////////////////////////////////////////////////////////////////
// Solution.cpp

#include "stdafx.h"
#include "Solution.h"

#include "SlnTreeManager.h"

Solution::Solution(SlnTreeManager* docMana)
    : _docMana(docMana)
{
}

Solution::Solution(SlnTreeManager* docMana, String name)
    : _slnName(name)
    , _docMana(docMana)
{

}

Solution::~Solution()
{

}

suic::ItemCollection* Solution::GetProjects()
{
    return GetChildren();
}

Brush* Solution::GetIcon() const
{
    return NULL;
}

String Solution::GetName() const
{
    return _slnName;
}

void Solution::SetName(String val)
{
    _slnName = val;
}

String Solution::ToString()
{
    String name;

    name.Format(_U("解决方案 '%s' (%d个项目)")
        , _slnName.c_str(), GetProjectCount());

    return name;
}

int Solution::GetProjectCount()
{
    int iCount = 0;

    for (int i = 0; i < GetCount(); ++i)
    {
        Project* prj(RTTICast<Project>(GetItem(i)));
        if (prj)
        {
            iCount++;
        }
    }

    return iCount;
}

void Solution::Close()
{

}

void Solution::SaveAll()
{

}

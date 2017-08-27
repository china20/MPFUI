//////////////////////////////////////////////////////////////////////////////
// FilterNode.cpp

#include "stdafx.h"
#include "Project.h"
#include "FilterNode.h"
#include "XamlRootItem.h"

#include <System/Tools/CoreHelper.h>

ImplementRTTIOfClass(ImageRootItem, RootItem)
ImplementRTTIOfClass(FilterNode, ResNode)

//=====================================================
// LayoutNode

ImageRootItem::ImageRootItem()
{

}

String ImageRootItem::GetRootType() const
{
    return String();
}

bool ImageRootItem::Save()
{
    return false;
}

bool ImageRootItem::SaveAs(const String& strPath)
{
    return false;
}

bool ImageRootItem::OnLoad(bool onlyReadAppRes)
{
    return false;
}

void ImageRootItem::OnClose()
{
}

void ImageRootItem::OnClear()
{
}

FilterNode::FilterNode()
{

}

FilterNode::FilterNode(const String& name)
    : _name(name)
{

}

FilterNode::~FilterNode()
{

}

bool FilterNode::IsSingleValue()
{
    return false;
}

suic::String FilterNode::GetResXml(const String& offset)
{
    return suic::String();
}

Project* FilterNode::GetProject() const
{
    const XamlNodeItem* parent = this;

    while (parent != NULL)
    {
        if (parent->GetRTTIType()->InheritFrom(Project::RTTIType()))
        {
            return (Project*)(parent);
        }

        parent = parent->GetParent();
    }

    return NULL;
}

FilterNode* FilterNode::FindItemOnChild(const String& name)
{
    FilterNode* filterNode = NULL;

    for (int i = 0; i < GetCount(); ++i)
    {
        FilterNode* pNode = RTTICast<FilterNode>(GetItem(i));
        if (0 == pNode->GetName().CompareI(name))
        {
            filterNode = pNode;
            break;
        }
    }
    return filterNode;
}

FilterNode* FilterNode::FindItemFromPath(const String& name)
{
    ResourceUri resPath;
    String strPath = name;

    resPath.Parse(name);
    strPath = resPath.GetResourcePath();

    strPath.Replace(_U("\\"), _U("/"));
    StringArray vec;
    int iStart = 0;
    int i = 0;
    FilterNode* currNode = this;

    vec.SplitString(strPath, _U("/"));

    for (i = iStart; i < vec.GetCount(); ++i)
    {
        String strName = vec[i];
        FilterNode* pChild = currNode->FindItemOnChild(strName);
        if (NULL != pChild)
        {
            currNode = pChild;
        }
        else
        {
            break;
        }
    }

    if (i == vec.GetCount())
    {
        return currNode;
    }

    return NULL;
}

void FilterNode::RemoveItemOnPath(const String& name)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        FilterNode* rootElem = RTTICast<FilterNode>(GetItem(i));
        if (0 == rootElem->GetRelativePath().CompareI(name))
        {
            RemoveItemAt(i);
            break;
        }
        else
        {
            rootElem->RemoveItemOnPath(name);
        }
    }
}

void FilterNode::RemoveFilterItem(FilterNode* rootElem)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        Object* pObj = GetItem(i);
        FilterNode* pNode = RTTICast<FilterNode>(pObj);

        if (pObj == rootElem)
        {
            RemoveItemAt(i);
            break;
        }
        else if (pNode != NULL)
        {
            pNode->RemoveFilterItem(rootElem);
        }
    }
}

String FilterNode::GetRelativePath() const
{
    String strPath;
    const FilterNode* parent = this;

    while (parent != NULL)
    {
        if (parent->GetRTTIType() == Project::RTTIType())
        {
            break;
        }

        if (strPath.Empty())
        {
            strPath = parent->GetName();

            // 中间目录，加上/
            if (!GetRTTIType()->InheritFrom(RootItem::RTTIType()))
            {
                strPath += _U("/");
            }
        }
        else
        {
            strPath = _U("/") + strPath;
            strPath = parent->GetName() + strPath;
        }

        parent = RTTICast<FilterNode>(parent->GetParent());
    }

    return strPath;
}

String FilterNode::GetFullPath() const
{
    String strPath;
    Project* pPrj = NULL;
    const FilterNode* parent = this;

    while (parent != NULL)
    {
        if (parent->GetRTTIType() == Project::RTTIType())
        {
            pPrj = (Project*)parent;
            break;
        }

        if (strPath.Empty())
        {
            strPath = parent->GetName();
        }
        else
        {
            strPath = _U("\\") + strPath;
            strPath = parent->GetName() + strPath;
        }

        const FilterNode* prevElem = parent;
        parent = RTTICast<FilterNode>(parent->GetParent());
    }

    if (strPath.Empty())
    {
        strPath = pPrj->GetProjectDir();
    }
    else
    {
        strPath = pPrj->GetProjectDir() + strPath;

        // 中间目录，加上/
        if (!GetRTTIType()->InheritFrom(RootItem::RTTIType()))
        {
            strPath += _U("\\");
        }
    }

    return strPath;
}

void FilterNode::AddSubFilter(const String& strPath)
{
    FilterNode* pNode = new FilterNode(strPath);
    if (NULL != pNode)
    {
        AddItem(pNode);

        // 创建子目录
        String strFullPath = pNode->GetFullPath();
        FileDir::DupCreateDir(strFullPath);
    }
}

bool FilterNode::IsRootNode() const
{
    if (RTTICast<Project>(GetParent()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void FilterNode::SetName(const String& name)
{
    _name = name;
}

String FilterNode::GetName() const
{
    return _name;
}

String FilterNode::ToString()
{
    return _name;
}

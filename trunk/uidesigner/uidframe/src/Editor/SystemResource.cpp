/////////////////////////////////////////////////////
// SystemResource.cpp

#include "stdafx.h"

#include <Editor/SystemResource.h>

SystemResource* SystemResource::Ins()
{
    static SystemResource _ins;
    return &_ins;
}

ResourceDicRootItem* SystemResource::GetDefaultResDic()
{
    return _resRootItem;
}

ResNode* SystemResource::FindResNode(const suic::String& strName)
{
    if (NULL != _resRootItem)
    {
        ResourceDictionaryNode* resDicNode = _resRootItem->GetResourceDicNode();
        return resDicNode->SearchResItem(strName);
    }
    else
    {
        return NULL;
    }
}

TemplateRootItem* SystemResource::FindControlTemplate(const suic::String& strType)
{
    TemplateRootItem* tempRootItem = NULL;
    suic::String strName = _U("sys") + strType + _U("Template");
    tempRootItem = suic::RTTICast<TemplateRootItem>(FindResNode(strName));

    if (NULL == tempRootItem)
    {
        SetterNode* setterNode = NULL;
        StyleNode* styleNode = FindStyle(strType);
        if (NULL != styleNode)
        {
            setterNode = styleNode->GetSetterCollection()->FindSetter(_U("Template"));
            if (NULL != setterNode)
            {
                ResNode* resNode = setterNode->GetResNode();
                ExtensionResNode* extResNode = suic::RTTICast<ExtensionResNode>(resNode);

                if (NULL == extResNode || extResNode->GetResourceKey().Empty())
                {
                    tempRootItem = suic::RTTICast<TemplateRootItem>(resNode);
                }
                else
                {
                    tempRootItem = suic::RTTICast<TemplateRootItem>(FindResNode(extResNode->GetResourceKey()));
                }
            }
        }
    }

    return tempRootItem;
}

StyleNode* SystemResource::FindStyle(const suic::String& strType)
{
    return suic::RTTICast<StyleNode>(FindResNode(strType));
}

bool SystemResource::FindAndCloneStyle(const suic::String& strType, ResNodePtr& resNode)
{
    StyleNode* styleNode = FindStyle(strType);
    if (NULL != styleNode)
    {
        styleNode->CloneNode(resNode);
        return true;
    }
    else
    {
        return false;
    }
}

bool SystemResource::FindAndCloneControlTemplate(const suic::String& strType, ResNodePtr& resNode)
{
    TemplateRootItem* tempNode = FindControlTemplate(strType);
    if (NULL != tempNode)
    {
        tempNode->CloneNode(resNode);
        return true;
    }
    else
    {
        return false;
    }
}

bool  SystemResource::InitDefaultResDic()
{
    if (NULL == _resRootItem)
    {
        //static suic::String strDefRes(_U("resource/uidesign/Template/theme.xaml"));
        //suic::String strPath = suic::FileDir::CalculatePath(strDefRes);
        suic::ByteStream data;
        DesignHelper::ReadDefaultThemeData(&data);
        XamlLoader xamlLoader;
        _resRootItem = new ResourceDicRootItem();
        _resRootItem->ref();
        return xamlLoader.LoadResourceDicRootXamlFromMemory(_resRootItem, (const char*)data.GetBuffer(), data.GetSize());
        //return xamlLoader.LoadResourceDicRootXaml(_resRootItem, strPath);
    }
    return (NULL != _resRootItem);
}

SystemResource::SystemResource()
{
    _resRootItem = NULL;
}

SystemResource::~SystemResource()
{
    if (NULL != _resRootItem)
    {
        _resRootItem->unref();
        _resRootItem = NULL;
    }
}
/////////////////////////////////////////////////////
// DpManager.cpp

#include "stdafx.h"

#include <Core/DpManager.h>
#include <map>

DpManager* DpManager::Ins()
{
    static DpManager _ins;
    if (_ins._dps.GetCount() == 0)
    {
        _ins.InitFilterDps();
        _ins.InitDefaultDpItems();
        _ins.MergeDpItems();
    }

    return &_ins;
}

void DpManager::AddDpItem(DpItem* pItem)
{
    if (!_dpItems.Contains(pItem->name))
    {
        _dpItems.Add(pItem->name, pItem);
    }
    else
    {
        suic::InvalidValueException(suic::String().Format(_U("Property(%s) has exist"), pItem->name.c_str()), __FILELINE__);
    }
}

void DpManager::InitFilterDps()
{
    _filterTagDps.Add(suic::FrameworkElement::TagProperty, 0);
    _filterTagDps.Add(suic::Element::Tag0Property, 0);
    _filterTagDps.Add(suic::Element::Tag1Property, 0);
    _filterTagDps.Add(suic::Element::Tag2Property, 0);
    _filterTagDps.Add(suic::Element::Tag3Property, 0);

    _filterDps.Add(_U("Clip"), 0);
    _filterDps.Add(_U("IsActive"), 0);
    _filterDps.Add(_U("DirectDp"), 0);
    _filterDps.Add(_U("FocusedElement"), 0);

    _filterDps.Add(_U("DataContext"), 0);
    _filterDps.Add(_U("VisualEffect"), 0);
    _filterDps.Add(_U("RenderTransform"), 0);
    _filterDps.Add(_U("VisualTransform"), 0);
    _filterDps.Add(_U("RenderTransformOrigin"), 0);

    _filterDps.Add(_U("Language"), 0);
    _filterDps.Add(_U("LayoutTransform"), 0);
}

void DpManager::InitPropertyOptions()
{
    suic::ResourceDictionary resDic;
    resDic.setAutoDelete(false);
    suic::XamlReader xamlRead;
    String strPath("/mpfuid;/resource/uidesign/data/PropertyOptions.xaml");
    xamlRead.LoadUri(NULL, &resDic, strPath);
    suic::XDictionary* pDic = suic::RTTICast<suic::XDictionary>(resDic.Lookup(_U("propOptions")));
    if (NULL != pDic)
    {
        suic::XDictionary::Enumerator enumer(pDic, 0);
        while (enumer.HasNext())
        {
            suic::String strName = enumer.Current()->key;
            suic::XArray* vals = suic::RTTICast<suic::XArray>(enumer.Current()->val);
            if (NULL != vals)
            {
                OptionDpItem* optionItem = new OptionDpItem(strName);
                AddDpItem(optionItem);
                for (int i = 0; i < vals->GetCount(); ++i)
                {
                    suic::String strVal(vals->GetItem(i)->ToString());
                    
                    strVal.Trim();

                    if (strVal[0] == '#')
                    {
                        strVal = strVal.Substring(1);
                        suic::StringArray strArr;
                        strArr.SplitString(strVal, _U(":"));
                        if (strArr.GetCount() == 2)
                        {
                            strVal = strArr[0];
                            optionItem->AddValueMap(strArr[1].ToInt(), i);
                        }
                    }
                    optionItem->AddOption(strVal);
                }

                optionItem->CheckValueMap();
            }
        }
    }
}

void DpManager::InitDefaultDpItems()
{
    DpProperty::GetProperties(_dps);

    AddDpItem(new VertAlignDpItem(_U("VerticalAlignment")));
    AddDpItem(new VertAlignDpItem(_U("VerticalContentAlignment")));

    AddDpItem(new HoriAlignDpItem(_U("HorizontalAlignment")));
    AddDpItem(new HoriAlignDpItem(_U("HorizontalContentAlignment")));

    InitPropertyOptions();
/*
    AddDpItem(new OrientationDpItem(_U("Orientation"), Integer::RTTIType()));
    AddDpItem(new DockDpItem(_U("Dock"), Integer::RTTIType()));
    AddDpItem(new DockDpItem(_U("TabStripPlacement"), Integer::RTTIType()));

    AddDpItem(new VisibilityDpItem(_U("Visibility"), Integer::RTTIType()));
    AddDpItem(new WindowStateDpItem(_U("WindowState"), Integer::RTTIType()));
    AddDpItem(new WindowStyleDpItem(_U("WindowStyle"), Integer::RTTIType()));
    AddDpItem(new ResizeModeDpItem(_U("ResizeMode"), Integer::RTTIType()));
    AddDpItem(new PlacementDpItem(_U("Placement"), Integer::RTTIType()));
    AddDpItem(new TextAlignmentDpItem(_U("TextAlignment"), Integer::RTTIType()));
    AddDpItem(new TextWrappingDpItem(_U("TextWrapping"), Integer::RTTIType()));
    AddDpItem(new RoleDpItem(_U("Role"), Integer::RTTIType()));
    AddDpItem(new ExpandDirectionDpItem(_U("ExpandDirection"), Integer::RTTIType()));

    AddDpItem(new StretchDpItem(_U("Stretch"), Integer::RTTIType()));
    AddDpItem(new StretchDirectionDpItem(_U("StretchDirection"), Integer::RTTIType()));

    AddDpItem(new FontWeightDpItem(_U("FontWeight"), Integer::RTTIType()));
    AddDpItem(new FontStyleDpItem(_U("FontStyle"), Integer::RTTIType()));

    AddDpItem(new HitTestFilterActionDpItem(_U("HitTestFilterAction"), Integer::RTTIType()));

    AddDpItem(new ScrollBarVisibilityDpItem(_U("HorizontalScrollBarVisibility"), Integer::RTTIType()));
    AddDpItem(new ScrollBarVisibilityDpItem(_U("VerticalScrollBarVisibility"), Integer::RTTIType()));
*/
    //-----------------------------------------------------------------------------

    AddDpItem(new WidthDpItem(_U("Width"), Integer::RTTIType()));
    AddDpItem(new WidthDpItem(_U("Height"), Integer::RTTIType()));
    AddDpItem(new WidthDpItem(_U("MinWidth"), Integer::RTTIType()));
    AddDpItem(new WidthDpItem(_U("MinHeight"), Integer::RTTIType()));
    AddDpItem(new WidthDpItem(_U("MaxWidth"), Integer::RTTIType()));
    AddDpItem(new WidthDpItem(_U("MaxHeight"), Integer::RTTIType()));

    AddDpItem(new OpacityDpItem(_U("Opacity"), OFloat::RTTIType()));
    AddDpItem(new ToolTipDpItem(_U("ToolTip"), OString::RTTIType()));
}

suic::String DpManager::NameFromDb(DpProperty* dp)
{
    /*suic::String name;
    if (dp->IsAttached())
    {
        name.Format(_U("%s.%s"), dp->GetOwnerType()->typeName->, dp->GetName().c_str());
    }
    else
    {
        name = dp->GetName();
    }*/
    return dp->GetName();
}

void DpManager::MergeDpItems()
{
    for (int i = 0; i < _dps.GetCount(); ++i)
    {
        DpProperty* dp = _dps.GetItem(i);
        suic::String name = DpManager::NameFromDb(dp);

        if (dp->GetOwnerType()->InheritFrom(suic::Transform::RTTIType()))
        {
            continue;
        }

        if (!_filterDps.Contains(name))
        {
            DpItem* dpItem = FindDpItem(name);
            if (dpItem == NULL)
            {
                dpItem = new DpItem(name, dp->GetPropType());
                AddDpItem(dpItem);
            }
            else
            {
                bool bFind = false;
                for (int i = 0; i < dpItem->dps.GetCount(); ++i)
                {
                    if (dpItem->dps[i] == dp)
                    {
                        bFind = true;
                        break;
                    }
                }
                if (!bFind)
                {
                    dpItem->dps.Add(dp);
                }
            }
            dpItem->AddDp(dp);
        }
    }

    _dpItems.GetValues(_dpItemColl);

    _trgOnlyDps.AddItem(FindDpItem(_U("IsEnabled")));
    _trgOnlyDps.AddItem(FindDpItem(_U("IsFocused")));
    _trgOnlyDps.AddItem(FindDpItem(_U("IsMouseOver")));

    _hideDps.Add(_U("Style"), 0);
}

DpProperty* DpManager::FindDp(const String& name)
{
    for (int i = 0; i < _dps.GetCount(); ++i)
    {
        DpProperty* dp = _dps.GetItem(i);
        if (dp->GetName().Equals(name))
        {
            return dp;
        }
    }
    return NULL;
}

DpProperty* DpManager::FindDp(const String& name, RTTIOfInfo* owner, bool isAttach)
{
    for (int i = 0; i < _dps.GetCount(); ++i)
    {
        DpProperty* dp = _dps.GetItem(i);
        if (dp->GetName().Equals(name))
        {
            if ((dp->IsAttached() && isAttach) || dp->FindMetadata(owner) != NULL)
            {
                return dp;
            }
        }
    }
    return NULL;
}

DpItem* DpManager::FindDpItem(const String& name)
{
    DpItem* pItem = NULL;
    _dpItems.TryGetValue(name, pItem);
    return pItem;
}

DpItem* DpManager::FindDpItem(DpProperty* dp)
{
    DpItem* pItem = NULL;
    _dpItems.TryGetValue(dp->GetName(), pItem);
    return pItem;
}

String DpManager::GetEditorName(DpItem* pItem)
{
    String strEditor = pItem->editor;
    if (strEditor.Empty())
    {
        if (pItem->type == NULL)
        {
            strEditor = pItem->name;
        }
        else
        {
            strEditor = pItem->GetEditorName();
        }
    }

    return strEditor + _U("_Prop");
}

void DpManager::FillPropByDp(ItemCollection* itemColl, RTTIOfInfo* owner, DpProperty* dp)
{
    Array<DpItem*> arrTag;
    arrTag.Resize(5);

    for (int i = 0; i < _dpItemColl.GetCount(); ++i)
    {
        DpItem* dpItem = _dpItemColl.GetItem(i);
        DpProperty* tDp = dpItem->GetDp(owner, true);
        if (tDp != NULL)
        {
            if (_filterTagDps.Contains(tDp))
            {
                arrTag.Add(dpItem);
            }
            else if (tDp->GetPropType()->InheritFrom(dp->GetPropType()))
            {
                itemColl->AddItem(dpItem);
            }
        }
    }

    for (int j = 0; j < arrTag.GetCount(); ++j)
    {
        itemColl->AddItem(arrTag[j]);
    }
}

bool DpManager::ExistOnItemCollection(ItemCollection* filterColl, DpItem* dpItem)
{
    if (NULL == filterColl)
    {
        return false;
    }

    for (int i = 0; i < filterColl->GetCount(); ++i)
    {
        if (filterColl->GetItem(i) == dpItem)
        {
            return true;
        }
        else
        {
            SetterNode* pSetter = RTTICast<SetterNode>(filterColl->GetItem(i));
            if (NULL != pSetter && pSetter->GetName().Equals(dpItem->name))
            {
                return true;
            }
        }
    }
    return false;
}

void DpManager::QueryDpItems(QueryDpCond* cond, ItemCollection* result)
{
    std::map<String, DpItem*> findDps;
    std::map<String, DpItem*> attachDps;

    suic::RTTIOfInfo* owner = cond->target;

    // 一般属性查询
    if (!cond->fromTrigger)
    {
        for (int i = 0; i < _dpItemColl.GetCount(); ++i)
        {
            DpItem* dpItem = _dpItemColl.GetItem(i);
            suic::DpProperty* dp = dpItem->GetDp(owner, true);

            if (!ExistOnItemCollection(cond->propFilter, dpItem) && NULL != dp && !dp->IsReadOnly())
            {
                if (cond->filterTemp && 
                    dp->GetPropType()->InheritFrom(suic::FrameworkTemplate::RTTIType()))
                {
                    continue;
                }

                if (_hideDps.Contains(dp->GetName()))
                {
                    continue;
                }

                if (NULL == cond->trgFilter || !ExistOnItemCollection(cond->trgFilter, dpItem))
                {
                    if (dp->IsAttached() && !owner->InheritFrom(dp->GetOwnerType()))
                    {
                        attachDps.insert(std::make_pair(dp->GetName(), dpItem));
                    }
                    else
                    {
                        findDps.insert(std::make_pair(dp->GetName(), dpItem));
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < _dpItemColl.GetCount(); ++i)
        {
            DpItem* dpItem = _dpItemColl.GetItem(i);

            if (!ExistOnItemCollection(cond->propFilter, dpItem) && !_trgOnlyDps.Contains(dpItem))
            {
                suic::DpProperty* dp = dpItem->GetDp(owner, false);
                if (dp != NULL && 
                    (dp->GetPropType() == suic::Boolean::RTTIType() || 
                    dp->GetPropType() == suic::Integer::RTTIType()))
                {
                    if (_hideDps.Contains(dp->GetName()))
                    {
                        continue;
                    }

                    if (dp->IsAttached() && !owner->InheritFrom(dp->GetOwnerType()))
                    {
                        attachDps.insert(std::make_pair(dp->GetName(), dpItem));
                    }
                    else
                    {
                        findDps.insert(std::make_pair(dp->GetName(), dpItem));
                    }
                }
            }
        }

        for (int j = 0; j < _trgOnlyDps.GetCount(); ++j)
        {
            result->AddItem(_trgOnlyDps.GetItem(j));
        }
    }

    std::map<String, DpItem*>::iterator iter = findDps.begin();
    while (iter != findDps.end())
    {
        result->AddItem(iter->second);
        ++iter;
    }

    // attach属性加到最后
    std::map<String, DpItem*>::iterator it = attachDps.begin();
    while (it != attachDps.end())
    {
        result->AddItem(it->second);
        ++it;
    }
}

String DpItem::GetEditorName()
{
    if (type->InheritFrom(Brush::RTTIType()))
    {
        return _U("Brush");
    }
    else if (type->InheritFrom(ControlTemplate::RTTIType()))
    {
        return _U("Template");
    }
    else if (type->InheritFrom(DataTemplate::RTTIType()))
    {
        return _U("DataTemplate");
    }
    else
    {
        return type->typeName;
    }
}

int DpItem::GetSelectIndex()
{
    return 0;
}
// 华勤科技版权所有 2010-2011
// 
// 文件名：DpManager.h
// 功  能：管理可编辑的属性。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _DPMANAGER_H_
#define _DPMANAGER_H_

#include <Core/ResNode.h>
#include <Main/XamlRootItem.h>
#include <System/Types/Const.h>

struct DpItem : public Object
{
    suic::String name;
    // 属性编辑器名字
    suic::String editor;
    // 属性类型
    suic::RTTIOfInfo* type;
    // 对应属性个数
    suic::Array<DpProperty*> dps;

    DpItem(const String& propName, suic::RTTIOfInfo* t)
    {
        name = propName;
        type = t;
    }
   
    String ToString()
    {
        return name;
    }

    // 获取属性注册时的元数据
    PropMetadata* GetMetatata(suic::RTTIOfInfo* owner)
    {
        PropMetadata* meta = NULL;
        for (int i = 0; i < dps.GetCount(); ++i)
        {
            meta = dps[i]->FindMetadata(owner);
            if (meta != NULL)
            {
                break;
            }
        }
        return meta;
    }

    // 根据创建属性时的Owner类型查询对应的属性
    // 如果bAttach为true则返回附加属性
    DpProperty* GetDp(suic::RTTIOfInfo* owner, bool bAttach)
    {
        PropMetadata* meta = NULL;
        for (int i = 0; i < dps.GetCount(); ++i)
        {
            meta = dps[i]->FindMetadata(owner);
            if (meta != NULL || (dps[i]->IsAttached() && bAttach))
            {
                return dps[i];
            }
        }
        return NULL;
    }

    DpProperty* GetDpEx(suic::RTTIOfInfo* owner, bool bAttach, bool& bOwner)
    {
        PropMetadata* meta = NULL;
        bOwner = false;

        for (int i = 0; i < dps.GetCount(); ++i)
        {
            meta = dps[i]->FindMetadata(owner);
            if (meta != NULL)
            {
                bOwner = true;
                return dps[i];
            }
            else if (dps[i]->IsAttached() && bAttach)
            {
                return dps[i];
            }
        }
        return NULL;
    }

    void AddDp(DpProperty* dp)
    {
        dps.Add(dp);
    }

    virtual String GetEditorName();
    virtual int GetSelectIndex();

    virtual suic::String ValueToString(ResNode* val)
    {
        return val->GetSingleXml();
    }

    virtual void FillAddChild(IAddChild* sender) {}
};

// 定义查询依赖属性的条件结构
struct QueryDpCond
{
    RTTIOfInfo* target;
    
    // 依赖属性的返回值集合
    ItemCollection* propFilter;
    ItemCollection* trgFilter;

    // 是否是Trigger里的属性
    bool fromTrigger;
    // 是否是模版里的属性
    bool inTemplate;
    // 是否跳过模版
    bool filterTemp;

    QueryDpCond()
    {
        target = NULL;
        propFilter = NULL;
        trgFilter = NULL;

        fromTrigger = false;
        inTemplate = false;
        filterTemp = false;
    }
};

/// <Summary>
///  统一管理系统中所有的依赖属性，因此在设计器里必须先
///  注册控件和其定义的依赖属性，这样，才可以自动把需要编辑的依赖
///  纳入统一的管理。
/// </Summary>
class DpManager
{
public:

    static DpManager* Ins();

    static suic::String NameFromDb(DpProperty* dp);

    /// <summary>
    ///   根据依赖属性名称查询对应的所有依赖属性
    /// </summary>
    /// <remarks>
    ///   名称相同的依赖属性可以属于不同的控件对象
    /// </remarks>
    /// <param name="name">依赖属性名称</param>
    /// <returns>依赖属性列表集合</returns>
    DpItem* FindDpItem(const String& name);
    DpItem* FindDpItem(DpProperty* dp);
    DpProperty* FindDp(const String& name);
    DpProperty* FindDp(const String& name, RTTIOfInfo* owner, bool isAttach);
    String GetEditorName(DpItem* pItem);

    void FillPropByDp(ItemCollection* itemColl, RTTIOfInfo* owner, DpProperty* dp);

    bool ExistOnItemCollection(ItemCollection* filterColl, DpItem* dpItem);
    void QueryDpItems(QueryDpCond* cond, ItemCollection* result);

    void GetDpItems(Collection<DpItem*>& dpItems)
    {
        _dpItems.GetValues(dpItems);
    }

private:

    void AddDpItem(DpItem* pItem);

    void InitFilterDps();
    void MergeDpItems();
    void InitDefaultDpItems();
    void InitPropertyOptions();

private:

    Collection<DpProperty*> _dps;
    Collection<DpItem*> _dpItemColl;
    StringAutoDic<DpItem> _dpItems;

    StringDic<int> _filterDps;
    PointerDic<DpProperty*, int> _filterTagDps;
    StringDic<int> _hideDps;

    Collection<DpItem*> _trgOnlyDps;
};

struct OpacityDpItem : public DpItem
{
public:

    OpacityDpItem(const suic::String& propName, suic::RTTIOfInfo* t)
        : DpItem(propName, t)
    {
        editor = _U("Ratio");
        name = propName;
    }
};

struct ToolTipDpItem : public DpItem
{
public:

    ToolTipDpItem(const suic::String& propName, suic::RTTIOfInfo* t)
        : DpItem(propName, t)
    {
        editor = _U("String");
        name = propName;
    }
};

struct WidthDpItem : public DpItem
{
public:

    WidthDpItem(const suic::String& propName, suic::RTTIOfInfo* t)
        : DpItem(propName, t)
    {
        editor = _U("Width");
        name = propName;
    }

    virtual suic::String ValueToString(ResNode* val)
    {
        suic::Integer* pWid = RTTICast<suic::Integer>(val->GetValue());
        
        if (NULL != pWid && pWid->ToInt() < 0)
        {
            return _U("Auto");
        }
        else
        {
            return DpItem::ValueToString(val);
        }
    }
};

struct OptionDpItem : public DpItem
{
public:

    OptionDpItem(const String& name)
        : DpItem(name, suic::Integer::RTTIType())
    {
        editor = _U("ComboBox");
    }

    virtual suic::String ValueToString(ResNode* val)
    {
        suic::String strXml;
        ExtensionResNode* exten = RTTICast<ExtensionResNode>(val);
        if (NULL != exten)
        {
            strXml = exten->GetSingleXml();
        }
        else
        {
            strXml = _options[GetChoiceIndex(val, suic::Integer::GetZeroInt())];
        }
        return strXml;
    }

    String GetEditorName()
    {
        return _U("ComboBox");
    }

    suic::String ValueToXml(ResNode* val)
    {
        return ValueToString(val);
    }

    virtual void FillAddChild(IAddChild* sender)
    {
        for (int i = 0; i < _options.GetCount(); ++i)
        {
            sender->AddText(_options[i]);
        }
    }

    int ValueFromIndex(int index)
    {
        suic::ValueDic<int, int>::Enumerator enumer(&_valMaps, 0);
        while (enumer.HasNext())
        {
            if (enumer.Current()->val == index)
            {
                return enumer.Current()->key;
            }
        }
        return index;
    }

    int ValueToIndex(int iVal)
    {
        int index = 0;
        if (!_valMaps.TryGetValue(iVal, index))
        {
            index = iVal;
        }
        return index;
    }

    void AddOption(const suic::String& val)
    {
        _options.Add(val);
    }

    void AddValueMap(int val, int index)
    {
        _valMaps.Add(val, index);
    }

    void CheckValueMap()
    {
        if (_valMaps.GetCount() != _options.GetCount())
        {
            _valMaps.Clear();
        }
    }

private:

    int GetChoiceIndex(ResNode* val, Integer* pDef)
    {
        int iVal = 0;
        int iIndex = 0;

        pDef->ref();

        if (NULL == val->GetValue())
        {
            val->SetValue(pDef);
        }

        pDef->unref();
        
        iVal = val->GetValue()->ToInt();
        
        if (!_valMaps.TryGetValue(iVal, iIndex))
        {
            iIndex = iVal;
        }

        return iIndex;
    }

private:

    suic::ValueDic<int, int> _valMaps;
    suic::Array<suic::String> _options;
};

struct HoriAlignDpItem : public OptionDpItem
{
public:

    HoriAlignDpItem(const suic::String& propName)
        : OptionDpItem(propName)
    {
        editor = _U("AlignHori");

        AddOption(_U("Left"));
        AddOption(_U("Right"));
        AddOption(_U("Center"));
        AddOption(_U("Stretch"));
    }
};

struct VertAlignDpItem : public OptionDpItem
{
public:

    VertAlignDpItem(const suic::String& propName)
        : OptionDpItem(propName)
    {
        editor = _U("AlignVert");

        AddOption(_U("Top"));
        AddOption(_U("Bottom"));
        AddOption(_U("Center"));
        AddOption(_U("Stretch"));
    }
};

#endif

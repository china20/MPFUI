// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

#include <System/Tools/Debug.h>
#include <System/Types/Structure.h>
#include <System/Windows/DpObject.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/DpProperty.h>
#include <System/Windows/Exception.h>
#include <System/Windows/Expression.h>
#include <System/Windows/UncommonField.h>
#include <System/Windows/RoutedEventArg.h>
#include <System/Windows/MemberVisitor.h>

namespace suic
{

UncommonProperty::UncommonProperty()
    : _hasBeenSet(false)
{
    ADDREFOBJ(DpProperty::UnsetValue());
    _globalIndex = DpProperty::GetUniqueGlobalIndex(NULL, _U("")); 
    DpProperty::_uncommonDpPropertyList.Add(NULL);
}

UncommonProperty::UncommonProperty(Object* val)
{
    _default = val;
    _hasBeenSet = false;
    ADDREFOBJ(_default);
    _globalIndex = DpProperty::GetUniqueGlobalIndex(NULL, _U("")); 
    DpProperty::_uncommonDpPropertyList.Add(NULL);
}

void UncommonProperty::SetValue(DpObject* d, Object* val)
{
    if (d == NULL || val == NULL)
    {
        throw ArgumentNullException(_U("d"), __FILELINE__);
    }

    val->ref();

    EntryIndex entry = MemberVisitor::FindEntryIndex(d, _globalIndex);

    if (_default != val)
    {
        MemberVisitor::SetEV(d, entry, NULL, _globalIndex, NULL, val, BaseValueType::btLocal);
        _hasBeenSet = true;
    }
    else
    {
        MemberVisitor::RemoveEV(d, entry, NULL);
    }

    val->unref();
}

Object* UncommonProperty::GetValue(DpObject* d) 
{
    if (d == NULL)
    {
        throw ArgumentNullException(_U("d"), __FILELINE__);
    }

    if (_hasBeenSet) 
    {
        EntryIndex entry = MemberVisitor::FindEntryIndex(d, _globalIndex);
        if (entry.IsFound())
        {
            Object* val = MemberVisitor::GetLocalValue(d, entry.GetIndex());
            if (val != DpProperty::UnsetValue())
            {
                return val;
            }
        }
    } 

    return _default;
}

void UncommonProperty::ClearValue(DpObject* d)
{ 
    if (d == NULL)
    {
        throw new ArgumentNullException(_U("d"), __FILELINE__);
    }

    EntryIndex entry = MemberVisitor::FindEntryIndex(d, _globalIndex);
    MemberVisitor::RemoveEV(d, entry, NULL);
}

//=================================================================
//

Object* DpProperty::_unsetValue = NULL;
static PropMetadata* unsetMetadata = NULL;

PropMetadata::PropMetadata()
    : _flag(0)
    , _defaultValue(NULL)
{
    _forType = Object::RTTIType();
    DpProperty::CheckUnsetValue(_defaultValue);
}

PropMetadata::PropMetadata(Object* val)
    : _defaultValue(val)
    , _flag(0)
    , _forType(NULL)
{
    ConvertFlag(_flag);
    ADDREFOBJ(_defaultValue);
    DpProperty::CheckUnsetValue(_defaultValue);
}

PropMetadata::PropMetadata(Object* val, int flag)
    : _defaultValue(val)
    , _flag(flag)
    , _forType(NULL)
{
    ConvertFlag(_flag);
    ADDREFOBJ(_defaultValue);
    DpProperty::CheckUnsetValue(_defaultValue);
}

PropMetadata::
PropMetadata(Object* val, int flag, PropChangedCallback propChangedCb)
    : _defaultValue(val)
    , _flag(flag)
    , _forType(NULL)
{
    ConvertFlag(flag);
    ADDREFOBJ(_defaultValue);
    DpProperty::CheckUnsetValue(_defaultValue);
    _dpPropChangedCallback += propChangedCb;
}

PropMetadata::
PropMetadata(Object* val, int flag, PropChangedCallback propChangedCb, CoerceValueCallback coerceValueCb)
    : _defaultValue(val)
    , _flag(flag)
    , _forType(NULL)
{
    ConvertFlag(flag);
    ADDREFOBJ(_defaultValue);
    DpProperty::CheckUnsetValue(_defaultValue);
    _dpPropChangedCallback += propChangedCb;
    _coerceValueCallback += coerceValueCb;
}

PropMetadata::~PropMetadata()
{
    FREEREFOBJ(_defaultValue);
}

void DpProperty::StaticInit()
{
    if (_unsetValue == NULL)
    {
        _unsetValue = new Object(false);
        unsetMetadata = new PropMetadata();
    }

    //unsetMetadata->setAutoDelete(false);
    //_unsetValue = new Object(false);
    //PropFromNames.Init(880);
}

void PropMetadata::ConvertFlag(int flag)
{
    _flag |= flag;

    if ((_flag & PropMetadataOptions::AffectsMeasure) == PropMetadataOptions::AffectsMeasure)
    {
        _flag |= PropMetadataOptions::AffectsArrange;// | PropMetadataOptions::AffectsRender;
    }

    if ((_flag & PropMetadataOptions::AffectsArrange) == PropMetadataOptions::AffectsArrange)
    {
        _flag |= PropMetadataOptions::AffectsRender;
    }

    if ((_flag & PropMetadataOptions::AffectsParentMeasure) == PropMetadataOptions::AffectsParentMeasure)
    {
        _flag |= PropMetadataOptions::AffectsParentArrange;
    }
}

void DpProperty::SetConvertValueCb(IConvertValue* convertValueCb)
{
    _convertValue = convertValueCb;
}

bool DpProperty::CanConvert() const
{
    return (_convertValue != NULL);
}

//String DpProperty::ConvertBackString(Object* val)
//{
//    String strVal;
//
//    if (CanConvert())
//    {
//        strVal = _convertValue->ToString(this, val);
//    }
//
//    return strVal;
//}

void DpProperty::ConvertValue(Object* val, ObjectPtr& obj)
{
    if (NULL != val)
    {
        val->ref();
        if (CanConvert())
        {
            Expression* expr = RTTICast<Expression>(val);
            if (NULL == expr)
            {
                obj = _convertValue->ConvertValue(this, val);
            }
        }
        val->unref();
    }
}

void DpProperty::CheckUnsetValue(Object*& val)
{
    if (NULL == val)
    {
        val = DpProperty::UnsetValue();
        val->ref();
    }
}

void DpProperty::GetProperties(Collection<DpProperty*>& dps)
{
    _propFromNames.GetValues(dps);
}

bool PropMetadata::IsInherited()
{
    return  (_flag & PropMetadataOptions::AffectsInherits) != 0;
}

void PropMetadata::SetInherited(bool val)
{
    if (val)
    {
        _flag |= PropMetadataOptions::AffectsInherits;
    }
    else
    {
        _flag &= ~(PropMetadataOptions::AffectsInherits);
    }
}

bool PropMetadata::IsAffectsArrange()
{
    return ((_flag & AffectsArrange) == AffectsArrange);
}

bool PropMetadata::IsAffectsMeasure()
{
    return ((_flag & AffectsMeasure) == AffectsMeasure);
}

bool PropMetadata::IsAffectsRender()
{
    return ((_flag & AffectsRender) == AffectsRender);
}

bool PropMetadata::IsAffectsParentArrange()
{
    return ((_flag & AffectsParentArrange) == AffectsParentArrange);
}

bool PropMetadata::IsAffectsParentMeasure()
{
    return ((_flag & AffectsParentMeasure) == AffectsParentMeasure);
}

bool PropMetadata::IsAffectsParentRender()
{
    return ((_flag & AffectsParentRender) == AffectsParentRender);
}

void PropMetadata::SetAffectsArrange(bool val)
{
}

void PropMetadata::SetAffectsMeasure(bool val)
{
}

void PropMetadata::SetAffectsRender(bool val)
{
}

void PropMetadata::SetAffectsParentArrange(bool val)
{
}

void PropMetadata::SetAffectsParentMeasure(bool val)
{
}

void PropMetadata::SetAffectsParentRender(bool val)
{
    
}

//======================================================================
//

Array<DpProperty*, false> DpProperty::_uncommonDpPropertyList;
ValueDic<int, DpProperty*> DpProperty::_propFromNames;
int DpProperty::_globalIndex = 0;
Object* DpProperty::NullValue = new Object(false);

DpProperty::~DpProperty()
{
    //if (_defmetadata)
    //{
    //    _defmetadata->unref();
    //}

    //_metadatas.Clear();
}

DpProperty::DpProperty(const String& strName, int flag, RTTIOfInfo* owner, RTTIOfInfo* prop, PropMetadata* defValue, ValidateValueCallback validateValueCallback)
{
    _convertValue = NULL;
    _resParser = NULL;

    _cacheFlag = 0;
    _name = strName;
    _flag = flag;
    _validateValueCallback = validateValueCallback;

    if (!defValue)
    {
        _defmetadata = new PropMetadata();
        _defmetadata->SetDefaultValue(DpProperty::UnsetValue());
    }
    else
    {
        _defmetadata = defValue;   
    }

    //_defmetadata->ref();

    _ownerType = owner;
    _propType = prop;

    _hashCode = CalcHashCode(_name, _ownerType);  

    _globalIndex++;
    _index = _globalIndex;
}

int DpProperty::CalcHashCode(const String& name, RTTIOfInfo* ownerType)
{
    return (name.GetHashCode() ^ ownerType->GetHashCode());
}

void DpProperty::SetName(const String& val)
{
    _name = val;
}

int DpProperty::GetHashCode()
{
    return static_cast<int>((DWORD_PTR)this);
}

String DpProperty::GetName()
{
    return _name;
}

DpProperty* DpProperty::Register(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue)
{
    return Register(name, ownerType, propType, defValue, ValidateValueCallback(), 0);
}

DpProperty* DpProperty::Register(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue, ValidateValueCallback validateValueCb)   
{   
    return Register(name, ownerType, propType, defValue, validateValueCb, 0);
}

DpProperty* DpProperty::RegisterAttached(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue)
{
    return Register(name, ownerType, propType, defValue, ValidateValueCallback(), 2);
}

DpProperty* DpProperty::RegisterReadOnly(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue)
{
    return Register(name, ownerType, propType, defValue, ValidateValueCallback(), 1);
}

#ifdef _DEBUG
void __CheckAttachedDp(const String& name, DpProperty* dp)
{
    static StringDic<DpProperty*> _debugDps;
    DpProperty* debugDp = NULL;
    if (_debugDps.TryGetValue(name, debugDp))
    {
        if (debugDp->IsAttached() || dp->IsAttached())
        {
            throw suic::InvalidValueException(String().Format(_U("invalid property name(%s)")
                , name.c_str()), __FILELINE__);
        }
    }
    else
    {
        _debugDps.Add(name, dp);
    }
}
#endif

DpProperty* DpProperty::Register(const String& name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue, ValidateValueCallback validateValueCb, int flag)
{
    DpProperty* dp = DpPropMemory::Create(name, ownerType, propType, defValue, validateValueCb, flag);

    if (_propFromNames.GetCount() == 0)
    {
        _propFromNames.Initialize(512);
    }
#ifdef _DEBUG
    if (name.IndexOf(_U("Column")) == -1 && 
        name.IndexOf(_U("Row")) == -1 &&
        !name.Equals(_U("IsSelected")))
    {
        __CheckAttachedDp(name, dp);
    }
    if (_propFromNames.Contains(dp->_hashCode)) 
    {
        Debug::Trace(_U("Trace: DpProperty has name '%s'\n"), name.c_str());
    }
#endif

    _propFromNames.Add(dp->_hashCode, dp);
    defValue->SetType(ownerType);

    return dp;
}

DpProperty* DpProperty::Lookup(const String& name, RTTIOfInfo* ownerType)
{
    DpProperty* retDp = NULL;
    RTTIOfInfo* tmpType = ownerType;

    while (tmpType)
    {
        int hashCode(CalcHashCode(name, tmpType));
        if (_propFromNames.TryGetValue(hashCode, retDp))
        {
            break;
        }
        if (tmpType == DpObject::RTTIType())
        {
            break;
        }
        tmpType = (*tmpType->baseType)();
    }

    return retDp;
}

int DpProperty::GetUniqueGlobalIndex(RTTIOfInfo* ownerType, String name)
{
    return ++_globalIndex;
}

bool DpProperty::IsReadOnly() const
{
    return ((_flag & 1) == 1);
}

bool DpProperty::IsAttached() const
{
    return ((_flag & 2) == 2);
}

void DpProperty::OverrideMetadata(RTTIOfInfo* forType, PropMetadata* metadata)
{
    if (metadata->GetType() == NULL)
    {
        metadata->SetType(forType);
    }
    _metadatas.Add(forType, metadata);
}

DpProperty* DpProperty::AddOwner(RTTIOfInfo* ownerType)
{
    return AddOwner(ownerType, NULL);
}

DpProperty* DpProperty::AddOwner(RTTIOfInfo* ownerType, PropMetadata* typeMetadata)
{
    if (ownerType == NULL)
    {
        throw ArgumentNullException(_U("ownerType"), __FILELINE__);
    }

    DpProperty* dp = this;
    int hashCode = CalcHashCode(GetName(), ownerType);

    // 检查是否存在
    /*if (PropertyFromName.Contains(key))
    { 
        throw ArgumentException(_U("PropertyAlreadyRegistered"));
    }*/ 

    if (typeMetadata != NULL) 
    {
        OverrideMetadata(ownerType, typeMetadata);
    }
    else
    {
        if (ownerType->InheritFrom(_defmetadata->GetType()))
        {
            _metadatas.Add(ownerType, _defmetadata);
        }
        else
        {
            typeMetadata = new PropMetadata(_defmetadata->_defaultValue, _defmetadata->_flag);
            OverrideMetadata(ownerType, typeMetadata);
        }
    }

    _propFromNames.Add(hashCode, const_cast<DpProperty*>(dp));

    return dp;
}

PropMetadata* DpProperty::GetMetadata(RTTIOfInfo* forType)
{
    PropMetadata* metadata = NULL;
    RTTIOfInfo* tmpType = forType;

    if (_metadatas.GetCount() > 0)
    {
        while (tmpType && tmpType != GetOwnerType())
        {
            if (_metadatas.TryGetValue(tmpType, metadata))
            {
                return metadata;
            }

            tmpType = (*tmpType->baseType)();
        }
    }

    if (IsAttached() || forType->InheritFrom(_defmetadata->GetType()))
    {
        return _defmetadata;
    }
    else
    {
        return unsetMetadata;
    }
}

PropMetadata* DpProperty::FindMetadata(RTTIOfInfo* forType)
{
    PropMetadata* metadata = NULL;
    RTTIOfInfo* tmpType = forType;

    if (_metadatas.GetCount() > 0)
    {
        while (tmpType && tmpType != GetOwnerType())
        {
            if (_metadatas.TryGetValue(tmpType, metadata))
            {
                return metadata;
            }
            tmpType = (*tmpType->baseType)();
        }
    }

    if (forType->InheritFrom(_defmetadata->GetType()))
    {
        return _defmetadata;
    }
    else
    {
        return NULL;
    }
}

IResourceParser* DpProperty::GetResourceParser()
{
    return _resParser;
}

bool DpProperty::IsValidValueInternal(DpObject* d, Object* val)
{
    if (DynamicCast<Expression>(val))
    {
        return false;
    }
    else if (_validateValueCallback.safe_bool())
    {
        return _validateValueCallback(d, val);
    }
    else
    {
        return true;
    }
}

bool DpProperty::IsValidValue(DpObject* d, Object* val)
{
    return IsValidValueInternal(d, val);
}

//=====================================================
// DpPropChangedEventArg

DpPropChangedEventArg::DpPropChangedEventArg(DpProperty* prop, PropMetadata* metadata, Object* oldValue, Object* newValue)
{
    _flags = 1;
    _property = prop;
    _metadata = metadata;
    if (oldValue == NULL)
    {
        _oldValue = DpProperty::UnsetValue();
    }
    else
    {
        _oldValue = oldValue;
    }

    if (newValue == NULL)
    {
        _newValue = DpProperty::UnsetValue();
    }
    else
    {
        _newValue = newValue;
    }
}

DpPropChangedEventArg::~DpPropChangedEventArg()
{

}

bool DpPropChangedEventArg::IsAValueChange()
{
    return ((_flags & 1) == 1);
}

void DpPropChangedEventArg::SetIsAValueChange(bool val)
{
    if (val)
    {
        _flags |= 1;
    }
    else
    {
        _flags &= ~1;
    }
}

String DpPropChangedEventArg::GetName()
{
    return _property->GetName();
}

Object* DpPropChangedEventArg::GetNewValue()
{
    return _newValue;
}

Object* DpPropChangedEventArg::GetOldValue()
{
    return _oldValue;
}

DpProperty* DpPropChangedEventArg::GetProp()
{
    return _property;
}

PropMetadata* DpPropChangedEventArg::GetMetadata()
{
    return _metadata;
}

//==============================================================
// DpPropertyMemory

int DpPropMemory::_capacity = 380;
int DpPropMemory::_propCount = 0;
int DpPropMemory::_metaCount = 0;
Byte* DpPropMemory::_memory = NULL;
Byte* DpPropMemory::_mempos = NULL;

int DpPropMemory::GetPropCount()
{
    return _propCount;
}

int DpPropMemory::GetMetaCount()
{
    return _metaCount;
}

int DpPropMemory::CheckMemory()
{
    int iBytes = 0;         // 剩余字节数
    int iMallocBytes = 0;

    if (_memory == NULL)
    {
        iMallocBytes = _capacity * (sizeof(DpProperty) + sizeof(PropMetadata));
        _memory = (Byte*)suic::uiMalloc(iMallocBytes);
        _mempos = _memory;
    }

    iBytes = _memory - _mempos;

    return iBytes;
}

DpProperty* DpPropMemory::Create(String name, RTTIOfInfo* ownerType, RTTIOfInfo* propType, PropMetadata* defValue, ValidateValueCallback validateValueCb, int flag)
{
    DpProperty* dp = NULL;
    int iBytes = CheckMemory();

    if (iBytes >= sizeof(DpProperty))
    {
        dp = new (_mempos)DpProperty(name, flag, ownerType, propType, defValue, validateValueCb);
        _mempos += sizeof(DpProperty);
    }
    else
    {
        dp = new DpProperty(name, flag, ownerType, propType, defValue, validateValueCb);
    }

    ++_propCount;

    return dp;
}

PropMetadata* DpPropMemory::GetPropMeta(Object* val)
{
    return DpPropMemory::GetPropMeta(val, 0);
}

PropMetadata* DpPropMemory::GetPropMeta(Object* val, int flag)
{
    PropChangedCallback propCb;
    return DpPropMemory::GetPropMeta(val, flag, propCb);
}

PropMetadata* DpPropMemory::GetPropMeta(Object* val, int flag, PropChangedCallback propChangedCb)
{
    CoerceValueCallback coerceCb;
    return DpPropMemory::GetPropMeta(val, flag, propChangedCb, coerceCb);
}

PropMetadata* DpPropMemory::GetPropMeta(Object* val, int flag, PropChangedCallback propChangedCb, CoerceValueCallback coerceValueCb)
{
    PropMetadata* meta = NULL;
    int iBytes = CheckMemory();

    if (iBytes >= sizeof(PropMetadata))
    {
        meta = new (_mempos)PropMetadata(val, flag, propChangedCb, coerceValueCb);
        _mempos += sizeof(PropMetadata);
    }
    else
    {
        meta = new PropMetadata(val, flag, propChangedCb, coerceValueCb);
    }

    ++_metaCount;

    return meta;
}

DpPropMemory::DpPropMemory()
{

}

//==============================================================
//
PropertyNotifyEventArg::
PropertyNotifyEventArg(const String& name, const String& propname, Uint32 flag)
    : _elemName(name)
    , _flag(flag)
    , _propName(propname)
{
}

bool PropertyNotifyEventArg::IsSelfElement()
{
    return true;
}

bool PropertyNotifyEventArg::IsNeedUpdate()
{
    return true;
}

}


#include <Framework/Controls/DefinitionBase.h>
#include <Framework/Controls/Grid.h>

namespace suic
{

ImplementRTTIOfClass(DefinitionBase, DpObject)
ImplementRTTIOfClass(GridSize, Object)

DpProperty* DefinitionBase::NameProperty;

GridSizeConvert* GridSizeConvert::Convert = new GridSizeConvert();

void DefinitionBase::StaticInit()
{
    if (NULL == NameProperty)
    {
        NameProperty = Element::NameProperty->AddOwner(RTTIType(), NULL);
    }
}

AutoObj GridSizeConvert::ConvertValue(DpProperty* dp, Object* val)
{
    int iLen = 0;
    String strVal = val->ToString();
    strVal.Trim();

    iLen = strVal.Length();

    if (strVal.Equals(_U("Auto")))
    {
        return new GridSize(GridSizeType::Auto, 0);
    }
    else if (iLen > 0 && strVal[iLen - 1] == _U('*'))
    {
        Float fVal = 1.0f;
        
        if (iLen > 1)
        {
            fVal = max(0, strVal.Substring(0, iLen - 1).ToFloat());
        }

        return new GridSize(GridSizeType::Star, fVal);
    }
    else
    {
        return new GridSize(GridSizeType::Pixel, strVal.ToFloat());
    }
}

String GridSizeConvert::ToString(DpProperty* dp, Object* val)
{
    return String();
}

DefinitionBase::DefinitionBase()
    : _flags(0)
    , _measureSize(0)
    , _minSize(0)
    , _offset(0)
    , _parentIndex(0)
    , _sizeCache(1)
    , _owner(NULL)
    , _isColDef(true)
    , _sizeType(GridSizeType::Star)
    , _sharedState(NULL)
    , _gridSize(NULL)
    , _userMin(0)
    , _userMax(Numeric::MeasureInt)
{
    SetDefaultGridSize();
}

DefinitionBase::DefinitionBase(bool isColDef)
    : _flags(0)
    , _measureSize(0)
    , _minSize(0)
    , _offset(0)
    , _parentIndex(0)
    , _sizeCache(1)
    , _owner(NULL)
    , _sharedState(NULL)
    , _isColDef(isColDef)
    , _sizeType(GridSizeType::Star)
    , _gridSize(NULL)
    , _userMin(0)
    , _userMax(Numeric::MeasureInt)
{
    SetDefaultGridSize();
}

DefinitionBase::~DefinitionBase()
{
    FREEREFOBJ(_gridSize)
}

String DefinitionBase::GetName()
{
    return GetValue(NameProperty)->ToString();
}

void DefinitionBase::SetName(String name)
{
    SetValue(NameProperty, new OString(name));
}

bool DefinitionBase::IsStar() const
{
    return (_sizeType == GridSizeType::Star);
}

bool DefinitionBase::IsAuto() const
{
    return (_sizeType == GridSizeType::Auto);
}

Float DefinitionBase::GetUserMinSize()
{
    return _userMin;
}

Float DefinitionBase::GetUserMaxSize()
{
    return _userMax;
}

GridSize* DefinitionBase::GetUserSize()
{
    return _gridSize;
}

bool GridSize::Equals(Object* Other)
{
    GridSize* gSize = NULL;

    if (Other == this)
    {
        return true;
    }
    
    gSize = RTTICast<GridSize>(Other);

    if (NULL != gSize && gSize->_unitValue == _unitValue && gSize->_unitType == _unitType)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DefinitionBase::SetUserMinSize(Float val, bool bWid)
{
    _userMin = val;

    if (!FloatUtil::Equals(val, Numeric::MeasureInt))
    {
        if (bWid)
        {
            _userMin = SystemParameters::TransformXToDevice(_userMin);
        }
        else
        {
            _userMin = SystemParameters::TransformYToDevice(_userMin);
        }
    }
}

void DefinitionBase::SetUserMaxSize(Float val, bool bWid)
{
    _userMax = val;

    if (!FloatUtil::Equals(val, Numeric::MeasureInt))
    {
        if (bWid)
        {
            _userMax = SystemParameters::TransformXToDevice(_userMax);
        }
        else
        {
            _userMax = SystemParameters::TransformYToDevice(_userMax);
        }
    }
}

void DefinitionBase::SetUserSize(GridSize* gSize, bool bWid)
{
    Float val = 0;
    GridSize* newSize = NULL;

    if (gSize != NULL)
    {
        val = gSize->GetValue();

        if (gSize->GetGridSizeType() == GridSizeType::Pixel && 
            !FloatUtil::Equals(val, Numeric::MeasureInt))
        {
            if (bWid)
            {
                val = SystemParameters::TransformXToDevice(val);
            }
            else
            {
                val = SystemParameters::TransformYToDevice(val);
            }
        }

        newSize = new GridSize(gSize->GetGridSizeType(), val);

        SETREFOBJ(_gridSize, newSize);

        if (NULL != newSize)
        {
            SetSizeType(newSize->GetGridSizeType());
        }
    }
    else
    {
        SetDefaultGridSize();
    }
}

void DefinitionBase::SetDefaultGridSize()
{
    if (NULL != _gridSize)
    {
        _gridSize->unref();
    }

    _gridSize = new GridSize(GridSizeType::Star, 1.0f);
    _gridSize->ref();
}

Float DefinitionBase::GetPreferredSize() const
{
    Float minSize = GetMinSize();
    if ((_sizeType != GridSizeType::Auto) && (minSize < _measureSize))
    {
        minSize = _measureSize;
    }
    return minSize;
}

Float DefinitionBase::GetMinSizeForArrange() const
{
    return _minSize;
}

}

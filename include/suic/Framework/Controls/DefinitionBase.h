// 华勤科技版权所有 2008-2022
// 
// 文件名：DefinitionBase.h
// 功  能：实现列和行的基类逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIDEFINITIONBASE_H_
#define _UIDEFINITIONBASE_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class DefinitionBase;
struct SharedSizeState
{
    // 缓存共享最小值
    int _minSize;
    int _userSize;
    bool _userSizeValid;
    String _sharedSizeGroupId;

    // 存储共享
    XArray _registry;
};

enum GridSizeType : Byte
{
    None = 0,
    Pixel = 1,
    Auto = 2,
    Star = 4,
};

class SUICORE_API GridSize : public Object
{
public:

    RTTIOfClass(GridSize)

    GridSize()
        : _unitType(GridSizeType::Auto)
        , _unitValue(0)
    {
    }

    GridSize(suic::Byte unitType, Float unitValue)
        : _unitType(unitType)
        , _unitValue(unitValue)
    {
    }

    suic::Byte GetGridSizeType() const;
    bool IsAuto() const;
    bool IsStar() const;
    Float GetValue() const;

    void SetGridSizeType(suic::Byte val);
    void SetValue(Float val);

    bool Equals(Object* Other);

private:

    Float _unitValue;
    suic::Byte _unitType;
};

inline suic::Byte GridSize::GetGridSizeType() const
{
    return _unitType;
}

inline bool GridSize::IsAuto() const
{
    return (GridSizeType::Auto == _unitType);
}

inline bool GridSize::IsStar() const
{
    return (GridSizeType::Star == _unitType);
}

inline Float GridSize::GetValue() const
{
    return _unitValue;
}

inline void GridSize::SetGridSizeType(suic::Byte val)
{
    _unitType = val;
}

inline void GridSize::SetValue(Float val)
{
    _unitValue = val;
}

class GridSizeConvert : public IConvertValue
{
public:

    static GridSizeConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);
    String ToString(DpProperty* dp, Object* val);
};

class SUICORE_API DefinitionBase : public DpObject
{
public:

    static DpProperty* NameProperty;

    static bool StaticInit();

    RTTIOfAbstractClass(DefinitionBase)

    DefinitionBase();
    DefinitionBase(bool isColDef);
    ~DefinitionBase();

    void SetOwner(FrameworkElement* owner);
    FrameworkElement* GetOwner() const;

    String GetName();
    void SetName(String name);

    bool IsStar() const;
    bool IsAuto() const;

    int GetFlags() const;
    void SetFlags(int val);

    Float GetMeasureSize() const;
    void SetMeasureSize(Float val);

    Float GetMinSize() const;
    void SetMinSize(Float val);

    Float GetOffset() const;
    void SetOffset(Float val);

    Float GetSizeCache() const;
    void SetSizeCache(Float val);

    Byte GetSizeType() const;
    void SetSizeType(Byte val);

    int GetParentIndex() const;
    void SetParentIndex(Float val);

    void UpdateMinSize(Float minSize);
    void Reset();

    Float GetPreferredSize() const;
    Float GetMinSizeForArrange() const;

    Float GetUserMinSize();
    Float GetUserMaxSize();
    GridSize* GetUserSize();

protected:

    void SetUserMinSize(Float val, bool bWid);
    void SetUserMaxSize(Float val, bool bWid);
    void SetUserSize(GridSize* gridSize, bool bWid);

    void SetDefaultGridSize();

private:

    Float _userMin;
    Float _userMax;
    GridSize* _gridSize;

private:

    int _flags;
    int _parentIndex;
    
    // 相对于Grid的偏移
    Float _offset;
    // 最小尺寸(百分比使用)
    Float _minSize;
    // 缓存百分比临时数值
    Float _sizeCache;
    // 实际测量尺寸
    Float _measureSize;

    Byte _sizeType;
    bool _isColDef;

    FrameworkElement* _owner;
    SharedSizeState* _sharedState;
};

inline void DefinitionBase::SetOwner(FrameworkElement* owner)
{
    _owner = owner;
}

inline FrameworkElement* DefinitionBase::GetOwner() const
{
    return _owner;
}

inline int DefinitionBase::GetFlags() const
{
    return _flags;
}

inline void DefinitionBase::SetFlags(int val)
{
    _flags = val;
}

inline Float DefinitionBase::GetMeasureSize() const
{
    return _measureSize;
}

inline void DefinitionBase::SetMeasureSize(Float val)
{
    _measureSize = val;
}

inline Float DefinitionBase::GetMinSize() const
{
    return _minSize;
}

inline void DefinitionBase::SetMinSize(Float val)
{
    _minSize = val;
}

inline Float DefinitionBase::GetOffset() const
{
    return _offset;
}

inline void DefinitionBase::SetOffset(Float val)
{
    _offset = val;
}

inline Float DefinitionBase::GetSizeCache() const
{
    return _sizeCache;
}

inline void DefinitionBase::SetSizeCache(Float val)
{
    _sizeCache = val;
}

inline Byte DefinitionBase::GetSizeType() const
{
    return _sizeType;
}

inline void DefinitionBase::SetSizeType(Byte val)
{
    _sizeType = val;
}

inline int DefinitionBase::GetParentIndex() const
{
    return _parentIndex;
}

inline void DefinitionBase::SetParentIndex(Float val)
{
    _parentIndex = val;
}

inline void DefinitionBase::UpdateMinSize(Float minSize)
{
    _minSize = max(_minSize, minSize);
}

inline void DefinitionBase::Reset()
{
    _minSize = 0;
    _measureSize = 0;
    _sizeCache = 0;
    _offset = 0;
}

class SUICORE_API GridDefinitions : public AutoColl<DefinitionBase>
{
public:

    GridDefinitions()
        : _owner(NULL)
    {
    }

    ~GridDefinitions()
    {
    }

    void SetOwner(FrameworkElement* owner)
    {
        _owner = owner;
    }

    FrameworkElement* GetOwner() const
    {
        return _owner;
    }

private:

    FrameworkElement* _owner;
};

};

#endif

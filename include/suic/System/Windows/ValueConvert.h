// 华勤科技版权所有 2010-2011
// 
// 文件名：ValueConvert.h
// 功  能：依赖属性值转换器。
// 
// 作  者：MPF开发组
// 时  间：2011-05-02
// 
// ============================================================================

#ifndef _UIVALUECONVERT_H_
#define _UIVALUECONVERT_H_

#include <System/Windows/Object.h>

namespace suic
{

class DpProperty;

class SUICORE_API IConvertValue
{
public:

    virtual ~IConvertValue() {}
    virtual AutoObj ConvertValue(DpProperty* dp, Object* val) = 0;
};

class SUICORE_API AlignmentConvert : public IConvertValue
{
public:

    static AlignmentConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

protected:

    AlignmentConvert();
};

class SUICORE_API OrientationConvert : public IConvertValue
{
public:

    static OrientationConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

protected:

    OrientationConvert();
};

class WindowStateConvert : public IConvertValue
{
public:

    static WindowStateConvert* Convert;
    
    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    WindowStateConvert() {}
};

class SUICORE_API VisibilityConvert : public IConvertValue
{
public:

    static VisibilityConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

protected:

    VisibilityConvert();
};

class SUICORE_API DockConvert : public IConvertValue
{
public:

    static DockConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

protected:

    DockConvert();
};

class SUICORE_API AutoP0Convert : public IConvertValue
{
public:

    static AutoP0Convert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

protected:

    AutoP0Convert();
};

class SUICORE_API AutoN1Convert : public IConvertValue
{
public:

    static AutoN1Convert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

protected:

    AutoN1Convert();
};

class SUICORE_API StretchConvert : public IConvertValue
{
public:

    static StretchConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    StretchConvert() {}
};

class SUICORE_API IntAutoArrayConvert : public IConvertValue
{
public:

    static IntAutoArrayConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    IntAutoArrayConvert() {}
};

class SUICORE_API FloatAutoArrayConvert : public IConvertValue
{
public:

    static FloatAutoArrayConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    FloatAutoArrayConvert() {}
};

class SUICORE_API FontWeightConvert : public IConvertValue
{
public:

    static FontWeightConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    FontWeightConvert() {}
};

class SUICORE_API FontStyleConvert : public IConvertValue
{
public:

    static FontStyleConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    FontStyleConvert() {}
};

class SUICORE_API TextTrimmingConvert : public IConvertValue
{
public:

    static TextTrimmingConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    TextTrimmingConvert() {}
};

class SUICORE_API TextAlignmentConvert : public IConvertValue
{
public:

    static TextAlignmentConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    TextAlignmentConvert() {}
};

class SUICORE_API TextWrappingConvert : public IConvertValue
{
public:

    static TextWrappingConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    TextWrappingConvert() {}
};

class SUICORE_API TextDecorationsConvert : public IConvertValue
{
public:

    static TextDecorationsConvert* Convert;

    AutoObj ConvertValue(DpProperty* dp, Object* val);

private:

    TextDecorationsConvert() {}
};

}

#endif

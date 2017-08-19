// 华勤科技版权所有 2010-2011
// 
// 文件名：Resource.h
// 功  能：定义资源基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIRESOURCE_H_
#define _UIRESOURCE_H_

#include <System/Types/Structure.h>

namespace suic
{

enum TextTrimming
{
    tNone,
    tCharacterEllipsis,
    tWordEllipsis,
    tPathEllipsis,
};

enum TextWrapping
{
    tNoWrap,
    tWrap,
    tWrapWithOverflow,
};

enum TextAlignment
{
    tLeft,
    tRight,
    tCenter,
    tJustify
};

enum TextDecorationLocation
{
    Underline,
    OverLine,
    Strikethrough,
    Baseline
};

enum TmFlag
{
    tmWordWrap = 1,
};

struct SUICORE_API TmParam
{
    fSize size;
    int tmFlag;
    int charCount;
    fSize constaint;

    TmParam();
};

enum PenLineCap
{
    plcFlat,
    plcRound,
    plcSquare,
    plcTriangle,
};

enum PenLineJoin
{
    pljMiter,
    pljRound,
    pljBevel,
};

class SUICORE_API Resource : public Object
{
public:

    enum ResType
    {
        resUnknown,
        resTbError,
        resError,
        resUserDefine,
        resObject,
        resNullable,
        resInteger,
        resFloat,
        resBoolean,
        resSize,
        resPoint,
        resRect,
        resfSize,
        resfPoint,
        resfRect,
        resColor,
        resPath,
        resTransform,
        resEffect,
        resTextEffect,
        resCursor,
        resStyle,
        resString,
        resImageSource,
        resArray,
        resDictionary,
        resBrush,
        resLayout,
        resLocal,
        resFrameworkTemplate,
        
        resExtension,
        resTemplateBinding,
        resBinding,
        resStaticResource,
        resDynamicResource,
    };

    RTTIOfAbstractClass(Resource)

    virtual ~Resource() {}

    inline String GetKey() const;
    inline void SetKey(const String& key);

    inline String GetName() const;

protected:

    String _key;
    String _name;
};

struct SUICORE_API ResourceItem
{
    ObjectPtr res;
    Resource::ResType type;
    ResourceItem();
};

inline String Resource::GetKey() const 
{ 
    return _key; 
}

inline void Resource::SetKey(const String& key) 
{ 
    _key = key; 
}

inline String Resource::GetName() const 
{ 
    return _name; 
}

}

#endif

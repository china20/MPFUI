// 华勤科技版权所有 2010-2011
// 
// 文件名：ImageBrush.h
// 功  能：实现图像刷。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIIMAGEBRUSH_H_
#define _UIIMAGEBRUSH_H_

#include <System/Graphics/Brush.h>
#include <System/Graphics/Bitmap.h>
#include <System/Graphics/Drawing.h>

#include <System/Windows/ResourceUri.h>

namespace suic
{

class SUICORE_API ImageBrush : public TileBrush
{
public:

    RTTIOfClass(ImageBrush)

    ImageBrush();
    ImageBrush(String path);
    virtual ~ImageBrush();

    bool Parse(const String& xml);
    bool Parse(IXamlNode* pNode);

    ImageBrush* Clone() const;

    void SetValue(const String& key, const String& value);

    ImageSource* GetImageSource() const;
    void SetImageSource(ImageSource* source);

    Rect GetContentBrounds();

    void SetGrey(bool val);
    bool GetGrey() const;

    Byte GetOpacity() const;
    void SetOpacity(Byte val);

    Rect GetViewCorner() const;
    void SetViewcorner(Rect val);

    bool HasViewCorner() const;

protected:

    Int16 ParseStretch(const String& value);
    void InitImageSource(const String& value);

protected:

    ImageSource* _source;

    // 是否做灰度处理
    Byte _grey;
    Byte _flag;
    Byte _opacity;

    Color _maskColor;
    Rect _viewCorner;
};

}

#endif

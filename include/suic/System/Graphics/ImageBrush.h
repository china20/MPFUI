// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ImageBrush.h
// ��  �ܣ�ʵ��ͼ��ˢ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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

    // �Ƿ����Ҷȴ���
    Byte _grey;
    Byte _flag;
    Byte _opacity;

    Color _maskColor;
    Rect _viewCorner;
};

}

#endif

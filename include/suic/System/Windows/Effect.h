// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Effect.h
// ��  �ܣ�������Ŀ����Ч�����ࡣ
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIEFFECT_H_
#define _UIEFFECT_H_

#include <System/Graphics/Resource.h>

namespace suic 
{

class IXamlNode;

class SUICORE_API Effect : public Resource
{
public:

    RTTIOfAbstractClass(Effect)

    virtual ~Effect() {}

    virtual void Parse(IXamlNode* node) = 0;
};

class SUICORE_API TextEffect : public Effect
{
public:

    RTTIOfAbstractClass(TextEffect)

    virtual ~TextEffect() {}
};

/// <summary>
///  ��˹ģ��Ч��
/// </summary>
class SUICORE_API BlurEffect : public Effect
{
public:

    RTTIOfClass(BlurEffect)

    BlurEffect();

    void Parse(IXamlNode* node);

    Float GetSigmaX() const { return _sigmaX; }
    void SetSigmaX(Float val) { _sigmaX = val; }

    Float GetSigmaY() const { return _sigmaY; }
    void SetSigmaY(Float val) { _sigmaY = val; }

    String ToString();

private:

    Float _sigmaX;
    Float _sigmaY;
};

/// <summary>
///  ����Ч��
/// </summary>
class SUICORE_API LightingEffect : public Effect
{
public:

    RTTIOfClass(LightingEffect)

    LightingEffect();

    void Parse(IXamlNode* node);

    Float GetX() const { return _x; }
    void SetX(Float val) { _x = val; }

    Float GetY() const { return _y; }
    void SetY(Float val) { _y = val; }

    Float GetZ() const { return _z; }
    void SetZ(Float val) { _z = val; }

    Float GetScale() const { return _scale; }
    void SetScale(Float val) { _scale = val; }

    String ToString();

private:

    Float _x;
    Float _y;
    Float _z;

    Color _color;

    Float _kd;
    Float _scale;
};

}

#endif

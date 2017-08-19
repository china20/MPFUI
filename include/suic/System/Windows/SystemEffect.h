// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SystemEffect.h
// ��  �ܣ�������Ŀ����Ч�����ࡣ
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISYSTEMEFFECT_H_
#define _UISYSTEMEFFECT_H_

#include <System/Windows/Effect.h>

namespace suic 
{

/// <summary>
///  ��ӰЧ��
/// </summary>
class SUICORE_API DropShadowEffect : public Effect
{
public:

    RTTIOfClass(DropShadowEffect)

    DropShadowEffect();

    void Parse(IXamlNode* node);

    Float GetDx() const { return _dx; }
    void SetDx(Float val) { _dx = val; }

    Float GetDy() const { return _dy; }
    void SetDy(Float val) { _dy = val; }

    Float GetSigmaX() const { return _sigmaX; }
    void SetSigmaX(Float val) { _sigmaX = val; }

    Float GetSigmaY() const { return _sigmaY; }
    void SetSigmaY(Float val) { _sigmaY = val; }

    Color GetColor() const { return _color; }
    void SetColor(Color val) { _color = val; }

    String ToString();

private:

    Float _dx;
    Float _dy;
    Float _sigmaX;
    Float _sigmaY;
    Color _color;
};

/// <summary>
///  ����Ч��
/// </summary>
class SUICORE_API EmbossEffect : public Effect
{
public:

    RTTIOfClass(EmbossEffect)

    EmbossEffect();

    void Parse(IXamlNode* node);

    Float GetX() const { return _x; }
    void SetX(Float val) { _x = val; }

    Float GetY() const { return _y; }
    void SetY(Float val) { _y = val; }

    Float GetZ() const { return _z; }
    void SetZ(Float val) { _z = val; }

    Float GetBlurSigma() const { return _blurSigma; }
    void SetBlurSigma(Float val) { _blurSigma = val; }

    String ToString();

private:

    Float _x;
    Float _y;
    Float _z;

    Float _blurSigma;
};

}

#endif

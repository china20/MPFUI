// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uieffects.h
// ��  �ܣ���װһ��Ĵ��ڶ���Ч����
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIEFFECTS_H_
#define _UIEFFECTS_H_

#include <System/Types/Const.h>
#include <System/Windows/FrameworkElement.h>

namespace suic 
{

SUICORE_API void EFRenderWindow(FrameworkElement* root, Bitmap* dib);
SUICORE_API BitmapPtr EFRotateWindow(Float angle, const Bitmap* srcbmp);

}

#endif

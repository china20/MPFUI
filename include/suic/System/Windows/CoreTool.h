// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uicorehelper.h
// ��  �ܣ�������Ŀ����������
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UICORETOOL_H_
#define _UICORETOOL_H_

#include <System/Tools/Debug.h>
#include <System/Types/Interface.h>
#include <System/Windows/RenderTarget.h>

#include <Framework/Controls/Tooltip.h>
#include <Framework/Animation/Storyboard.h>

namespace suic
{

/// <summary>
///  ����UI���У�������Ϣ�ַ�
/// </summary>
/// <remarks>
///  �˷��������LoadComponent�Ƚ���Ԫ�أ�Ȼ�󴴽����������Ϣѭ����
/// </remarks>
/// <param name="root">Ĭ�ϸ�Ԫ��</param>
/// <param name="path">·��</param>
/// <returns>��</returns>
SUICORE_API void RunApplication(Object* root, const String& path);

//-----------------------------------------------------------

/// <summary>
///  ȡ��Ԫ�ص���Ⱦ�߽�
/// </summary>
/// <param name="elem">Ŀ��Ԫ��</param>
/// <returns>Ԫ�ص���Ⱦ�߽�</returns>
SUICORE_API Rect GetRenderBound(Element*);

/// <summary>
///  ˢ������������ʾ
/// </summary>
/// <param name="elem">Ŀ��Ԫ��</param>
/// <returns>��</returns>
SUICORE_API void InvalidateWindow(Element*);

/// <summary>
///  ��ȡԪ�ص���Ⱦ����
/// </summary>
/// <param name="elem">Ŀ��Ԫ��</param>
/// <param name="bmp">Ԫ�ص���Ⱦ����</param>
/// <returns>��</returns>
SUICORE_API void RenderToImage(Element* elem, Bitmap& bmp);

SUICORE_API bool IsMouseInElement(Element*);
SUICORE_API void SetElementTopMost(Element*, bool);
SUICORE_API void uiEnableWindow(Element*, bool);

//-----------------------------------------------------------

SUICORE_API void* uiMalloc(int);
SUICORE_API void uiFree(void*);

SUICORE_API void UIInit(int level);
SUICORE_API void UIClear();

SUICORE_API Handle GetUIFont();

SUICORE_API Point ToScreenPoint(Element* element, Point pt);
SUICORE_API Point GetCursorPoint(Element* element);
SUICORE_API bool PointInWindow(Element* element);
SUICORE_API Point CalcuCusorPoint(Element* element);
SUICORE_API Point CalcElementAlign(Element* element, int flag);

SUICORE_API int RegisterWindowFilter(Element*, MessageHook);
SUICORE_API void RemoveWindowFilter(Element*, MessageHook);

SUICORE_API void CloseAssignerFrame(AssignerFrame* frame);

}

#endif

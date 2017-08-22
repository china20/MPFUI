// 华勤科技版权所有 2010-2011
// 
// 文件名：ValueBox.h
// 功  能：定义核心库用到的常量。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIVALUEBOX_H_
#define _UIVALUEBOX_H_

#include <System/Types/Const.h>

namespace suic 
{

class Integer;

struct SUICORE_API DockBox
{
    static Integer* LeftBox;
    static Integer* TopBox;
    static Integer* RightBox;
    static Integer* BottomBox;

    static Integer* From(int index);
};

struct SUICORE_API VisibilityBox
{
    static Integer* CollapsedBox;
    static Integer* HiddenBox;
    static Integer* VisibleBox;

    static Integer* From(int index);
};

struct SUICORE_API OrientationBox
{
    static Integer* VerticalBox;
    static Integer* HorizontalBox;

    static Integer* From(int index);
};

struct SUICORE_API HoriAlignBox
{
     static Integer* LeftBox;
     static Integer* RightBox;
     static Integer* CenterBox;
     static Integer* StretchBox;

     static Integer* From(int index);
};

struct SUICORE_API VertAlignBox
{
    static Integer* TopBox;
    static Integer* BottomBox;
    static Integer* CenterBox;
    static Integer* StretchBox;

    static Integer* From(int index);
};

struct SUICORE_API TileModeBox
{
    static Integer* NoneBox;
    static Integer* FlipXBox;
    static Integer* FlipYBox;
    static Integer* FlipXYBox;
    static Integer* TileBox;

    static Integer* From(int index);
};

struct SUICORE_API AlignmentXBox
{
    static Integer* LeftBox;
    static Integer* CenterBox;
    static Integer* RightBox;

    static Integer* From(int index);
};

struct SUICORE_API AlignmentYBox
{
    static Integer* TopBox;
    static Integer* CenterBox;
    static Integer* BottomBox;

    static Integer* From(int index);
};

struct SUICORE_API StretchBox
{
    static Integer* NoneBox;
    static Integer* FillBox;
    static Integer* UniformBox;
    static Integer* UniformToFillBox;

    static Integer* From(int index);
};

struct SUICORE_API StretchDirectionBox
{
    static Integer* UpOnlyBox;
    static Integer* DownOnlyBox;
    static Integer* BothBox;

    static Integer* From(int index);
};

struct SUICORE_API ResizeModeBox
{
    static Integer* NoResizeBox;
    static Integer* CanMinimizeBox;
    static Integer* CanResizeBox;
    static Integer* CanResizeWithGripBox;

    static Integer* From(int index);
};

struct SUICORE_API WindowStateBox
{
    static Integer* NormalBox;
    static Integer* MinimizedBox;
    static Integer* MaximizedBox;

    static Integer* From(int index);
};

struct SUICORE_API WindowStyleBox
{
    static Integer* NoneBox;
    static Integer* SingleBorderWindowBox;
    static Integer* ThreeDBorderWindowBox;
    static Integer* ToolWindowBox;

    static Integer* From(int index);
};

}

#endif

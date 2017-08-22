// 华勤科技版权所有 2010-2011
// 
// 文件名：HoldControl.h
// 功  能：占位控件（UI设计器用）
// 
// 作  者：MPF开发组
// 时  间：2015-02-12
// 
// =============================================================

#ifndef _UIHOLDCONTROL_H_
#define _UIHOLDCONTROL_H_

#include <Framework/Controls/Content.h>

namespace suic
{

class SUICORE_API HoldControl : public suic::FrameworkElement
{
public:

	RTTIOfClass(HoldControl)

	HoldControl();
    HoldControl(const String& name);
	~HoldControl();

    suic::String GetName() const;

protected:

    void OnRender(Drawing* drawing);

private:

    suic::String _name;
};

}

#endif

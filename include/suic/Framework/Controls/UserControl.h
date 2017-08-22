// 华勤科技版权所有 2010-2011
// 
// 文件名：UserControl.h
// 功  能：UserControl
// 
// 作  者：MPF开发组
// 时  间：2015-02-12
// 
// =============================================================

#ifndef _UIUSERCONTROL_H_
#define _UIUSERCONTROL_H_

#include <Framework/Controls/Content.h>

namespace suic
{

class SUICORE_API UserControl : public suic::ContentControl
{
public:

	RTTIOfClass(UserControl)

    static bool StaticInit();

	UserControl();
	~UserControl();
};

}

#endif

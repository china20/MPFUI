// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UserControl.h
// ��  �ܣ�UserControl
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2015-02-12
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

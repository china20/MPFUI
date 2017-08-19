// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����HoldControl.h
// ��  �ܣ�ռλ�ؼ���UI������ã�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2015-02-12
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

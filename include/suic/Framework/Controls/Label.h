// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Label.h
// ��  �ܣ���ǩ�ؼ���֧����Ϸ�ʽʵ�֡�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================

#ifndef _UILABEL_H_
#define _UILABEL_H_

#include <Framework/Controls/Content.h>
   
namespace suic
{

class SUICORE_API Label : public ContentControl
{
public:

    RTTIOfClass(Label)

    Label();
    virtual ~Label();

    static bool StaticInit();

    Label* Clone();
};

}

#endif

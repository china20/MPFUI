// ���ڿƼ���Ȩ���� 2010-2016
// 
// �ļ�����DirBrowser.h
// ��  �ܣ�Ŀ¼����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
// 
// ============================================================================

#ifndef _UIDIRGROWSER_H_
#define _UIDIRGROWSER_H_

#include <Extend/suiwgxconfig.h>
#include <System/Windows/Element.h>

namespace suic
{

class SUIWGX_API DirBrowser : public DpObject
{
public:

    DirBrowser();
    virtual ~DirBrowser();

    bool Open(const Element* owner=0, String dir=String(), bool bMulti=false);
    bool Open(const Element* owner, String Title, String dir=String(), bool bMulti=false);

    suic::String GetDir() const;

protected:

    String _dir;

};

};

#endif

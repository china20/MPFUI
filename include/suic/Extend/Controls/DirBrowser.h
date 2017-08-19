// 华勤科技版权所有 2010-2016
// 
// 文件名：DirBrowser.h
// 功  能：目录管理。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
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

// 华勤科技版权所有 2010-2016
// 
// 文件名：FileBrowser.h
// 功  能：文件管理。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
// 
// ============================================================================

#ifndef _UIFILEGROWSER_H_
#define _UIFILEGROWSER_H_

#include <Extend/suiwgxconfig.h>
#include <System/Windows/Element.h>

namespace suic
{

class SUIWGX_API FileBrowser : public DpObject
{
public:

    FileBrowser(bool val=false);

    virtual ~FileBrowser();

    bool Open(const Element* parent=0, Char* filter=_U("All Files(*.*)\0*.*\0\0"));
    bool Save(const Element* parent=0, Char* filter=_U("All Files(*.*)\0*.*\0\0"));

    void SetMultiFiles(bool val);
    String GetFilePath() const;

protected:

    bool InternalOpen(const Element* parent, bool bOpen, Char* filter);

protected:

    // 选择多个文件
    bool _selMulti;
    String _path;

};

inline void FileBrowser::SetMultiFiles(bool val)
{
    _selMulti = val;
}

};

#endif

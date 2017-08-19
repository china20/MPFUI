// ���ڿƼ���Ȩ���� 2010-2016
// 
// �ļ�����FileBrowser.h
// ��  �ܣ��ļ�����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
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

    // ѡ�����ļ�
    bool _selMulti;
    String _path;

};

inline void FileBrowser::SetMultiFiles(bool val)
{
    _selMulti = val;
}

};

#endif

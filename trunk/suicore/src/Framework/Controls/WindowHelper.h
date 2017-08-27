// 华勤科技版权所有 2008-2022
// 
// 文件名：WndHelper.h
// 功  能：窗口操作。
// 
// 作  者：MPF开发组
// 时  间：2010-09-23
// 
// ============================================================================

#ifndef _UIWNDHELPER_H_
#define _UIWNDHELPER_H_

#include <Framework/Controls/Control.h>

namespace suic
{

class WndHelper
{
public:

    WndHelper(Element* p);
    virtual ~WndHelper();

    bool OpenClipboard();
    void CloseClipboard();

    int PasteText(String & text);
    int CutText(String & text);
    int CopyText(const String & text);

protected:

    Element* _owner;
};

};

#endif

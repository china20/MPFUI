// 华勤科技版权所有 2008-2022
// 
// 文件名：Label.h
// 功  能：标签控件，支持组合方式实现。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
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

    static void StaticInit();

    Label* Clone();
};

}

#endif

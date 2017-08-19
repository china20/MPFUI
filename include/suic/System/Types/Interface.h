// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Interface.h
// ��  �ܣ�������Ŀ���û�ע��ӿڡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIINTERFACE_H_
#define _UIINTERFACE_H_

#include <System/Types/XamlNode.h>
#include <System/Windows/Element.h>

namespace suic 
{

class NOVTABLE IReadNodeCb
{
public:

    virtual ~IReadNodeCb() {}

    virtual void CreateElement(IXamlNode* node, ObjectPtr& elem) = 0;
    virtual ResourceItem ConvertExpression(RTTIOfInfo* d, DpProperty* dp, const String& expr) = 0;
};

/// <summary>
/// ϵͳ��־������
/// </summary>
class LogLevel 
{
public:

    enum {None, Error, Warn, Info, Trace};
};

}

#endif

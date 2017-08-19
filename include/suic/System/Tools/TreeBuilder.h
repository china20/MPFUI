// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uibuilder.h
// ��  �ܣ����湹���ࡣ
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2012-08-02
// 
// ============================================================================

#ifndef _UIBUILDER_H_
#define _UIBUILDER_H_

#include <System/Windows/Object.h>
#include <System/Windows/CoreTool.h>
#include <System/Tools/Collection.h>
#include <System/Types/StreamDef.h>
#include <Framework/Controls/Window.h>

namespace suic
{

struct EncryItem
{
    Byte* input;
    Uint32 inputSize;
    ByteStream output;
};

class IResDecryption : public Object
{
public:

    virtual ~IResDecryption() {}
    
    virtual bool Decrypt(const String& pwd, EncryItem& item) = 0;
};

class SUICORE_API Builder
{
public:

    static void SetResDecryption(IResDecryption* pDecrypt);

    static AutoObj BuildUri(Object* elem, const String& uri);
    static AutoObj BuildMemory(Object* elem, const Byte* data, int size);
    static AutoObj BuildNode(Object* elem, IXamlNode* node);

private:

    Builder();
    ~Builder();
};

class SUICORE_API XamlReader
{
public:

    static AutoObj LoadUri(Element* parent, Object* component, const String& uri);
    static AutoObj LoadMemory(Element* parent, Object* component, const Byte* data, int size);
    static AutoObj LoadNode(Element* parent, Object* component, IXamlNode* node);
};

}

#endif

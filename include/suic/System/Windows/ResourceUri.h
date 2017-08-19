// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ResourceUri.h
// ��  �ܣ���Դ·��������/MPF;/image/bk.png��
//         MPF��ʾ��Դ����/image/bk.png��ʾ��Դ·��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIRESOURCEURI_H_
#define _UIRESOURCEURI_H_

#include <System/Windows/Object.h>

namespace suic
{

enum UriKind
{
    ukAbsolute,
    ukRelative,
};

class SUICORE_API ResourceUri
{
public:

    ResourceUri();
    ResourceUri(const String& uri);
    ResourceUri(const ResourceUri& Other);

    ResourceUri& operator=(const ResourceUri& Other);
    bool operator==(const ResourceUri& Other);
    bool operator!=(const ResourceUri& Other);

    bool Euqals(const ResourceUri& Other) const;

    bool Parse(const String& uri);

    String ToString();
    String GenerateKey();

    String GetCompoment() const;
    void SetComponent(const String& val);

    String GetResourcePath() const;
    void SetResourcePath(const String& val);

    UriKind GetUriKind() const;
    void SetUriKind(UriKind val);

protected:

    void CheckValue(String& val);

protected:

    String _component;
    String _resPath;
    UriKind _uriKind;
};

};

#endif

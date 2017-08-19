// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����XamlNode.h
// ��  �ܣ�������Ŀ���Դ�ļ���д�ӿڡ�
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIIXAMLPROP_H_
#define _UIIXAMLPROP_H_

#include <System/Types/Structure.h>
#include <System/Graphics/Resource.h>
#include <System/Resources/ThemeInfo.h>

namespace suic 
{

class NOVTABLE IXamlBase : public Object
{
public:

    virtual ~IXamlBase() {}
    
    virtual Object* GetOwner() = 0;
    virtual void SetOwner(Object* owner) = 0;
};

class IXamlNode;
typedef shared<IXamlNode> XamlNodePtr;

class NOVTABLE IAttrisOp
{
public:

    virtual ~IAttrisOp() {}

    virtual void Clear() = 0;
    virtual void RemoveAttri(const String& name) = 0;
    virtual void AddAttri(const String& name, const String& val) = 0;
};

class NOVTABLE IXamlAttris : public IXamlBase
{
public:

    virtual ~IXamlAttris() {}

    virtual bool Contains(const String& name) = 0;
    virtual String FindAttri(const String& name) = 0;

    virtual void Reset() = 0;
    virtual bool HasNext() = 0;
    virtual bool HasAttris() = 0;
    virtual String GetValue() = 0;
    virtual String GetName() = 0;

    virtual IAttrisOp* GetAttriOp() = 0;
};

class NOVTABLE IXamlNodeOp
{
public:

    virtual ~IXamlNodeOp() {}

    virtual IXamlNode* AddChild(const String& name) = 0;

    /// <summary>
    ///   ��ָ����Ԫ��֮ǰ����һ���յ���Ԫ��
    /// </summary>
    /// <param name="child">��ǰ��Ԫ��</param>
    /// <param name="name">��Ԫ������</param>
    /// <returns>��Ԫ�ض���</returns>
    virtual IXamlNode* InsertChild(IXamlNode* child, const String& name) = 0;

    /// <summary>
    ///   �Ƴ�ָ������Ԫ��
    /// </summary>
    /// <param name="child">��ǰ��Ԫ��</param>
    /// <returns>��Ԫ�ض���</returns>
    virtual void RemoveChild(IXamlNode* child) = 0;
    virtual void RemoveChild(const String& name) = 0;
    virtual void RemoveChildren() = 0;

    virtual void SetName(const String& name) = 0;
};

class NOVTABLE IXamlNode : public IXamlBase
{
public:

    virtual ~IXamlNode() {}

    /// <summary>
    ///   ��ֵ�ڵ㱾��
    /// </summary>
    /// <returns>�ڵ�Ŀ�������</returns>
    virtual XamlNodePtr CloneNode() = 0;

    /// <summary>
    ///   �Ƿ����ָ�����ֵ���Ԫ��
    /// </summary>
    /// <param name="name">��Ԫ������</param>
    /// <returns>��������true;����false</returns>
    virtual bool Contains(const String& name) = 0;

    /// <summary>
    ///   �ҵ�ָ�����ֵ���Ԫ��
    /// </summary>
    /// <param name="name">��Ԫ������</param>
    /// <returns>��Ԫ�ض���</returns>
    virtual XamlNodePtr FindChild(const String& name) = 0;

    /// <summary>
    ///   ��õ�ǰ��Ԫ������
    /// </summary>
    /// <returns>��Ԫ������</returns>
    virtual String GetName() = 0;
    virtual String GetValue() = 0;

    /// <summary>
    ///   ����ָ�����Ƶ�Ԫ������ֵ
    /// </summary>
    /// <param name="name">Ԫ����������</param>
    /// <returns>Ԫ������ֵ</returns>
    virtual String FindAttri(const String& name) = 0;
    virtual bool ContainsAttri(const String& name) = 0;

    /// <summary>
    ///   ��ȡ��Ԫ�����Լ��ϵ���ٶ���
    /// </summary>
    /// <returns>���Լ��ϵ���ٶ���</returns>
    virtual IXamlAttris* GetAttris() = 0;

    virtual String GetComment() = 0;
    virtual void SetComment(const String& comment) = 0;

    virtual String GetXml(const String& prefix) = 0;

    virtual void Reset() = 0;
    virtual bool HasNext() = 0;
    virtual bool HasChildren() = 0;
    virtual String CurrentName() = 0;
    virtual IXamlNode* Current() = 0;

    virtual IXamlNodeOp* GetXamlNodeOp() = 0;

};

class NOVTABLE IXamlDoc : public Object
{
public:

    virtual ~IXamlDoc() {}

    virtual bool LoadUri(const String& uri) = 0;
    virtual bool LoadMemory(const Byte* data, int size) = 0;

    /// <summary>
    ///   ��ȡ���ڵ�
    /// </summary>
    /// <returns>���ڵ����</returns>
    virtual IXamlNode* GetRootNode() = 0;

    /// <summary>
    ///   ���Ϊָ�����ļ�
    /// </summary>
    /// <param name="uri">�ļ�·��</param>
    /// <returns>�ɹ�����true;����false</returns>
    virtual bool SaveAs(const String& uri) = 0;
    virtual void Close() = 0;
};

class SUICORE_API XamlTools
{
public:

    static IXamlDoc* CreateXamlDoc();

private:

    XamlTools() {}
};

class SUICORE_API IResourceParser
{
public:

    IResourceParser(Resource::ResType t);
    virtual ~IResourceParser();

    Resource::ResType GetResourceType() const;

    virtual void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem) = 0;
    virtual void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem) = 0;

protected:

    Resource::ResType _resType;
};

/// <summary>
/// ��Դ�ڵ�����
/// </summary>
enum NodeType 
{ 
    eNone, 
    eApplication, 
    eElement, 
    eResourceDic, 
    eResourceMerge, 
    eResource, 
    eStyle, 
    eProperty,
};

/// <summary>
/// �ؼ�������ԭ������
/// </summary>
typedef delegate<void(IXamlNode*, NodeType,ObjectPtr&)> BuilderCb;

}

#endif

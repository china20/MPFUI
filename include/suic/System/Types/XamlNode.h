// 华勤科技版权所有 2010-2011
// 
// 文件名：XamlNode.h
// 功  能：定义核心库资源文件读写接口。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
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
    ///   在指定子元素之前插入一个空的子元素
    /// </summary>
    /// <param name="child">当前子元素</param>
    /// <param name="name">子元素名称</param>
    /// <returns>子元素对象</returns>
    virtual IXamlNode* InsertChild(IXamlNode* child, const String& name) = 0;

    /// <summary>
    ///   移除指定的子元素
    /// </summary>
    /// <param name="child">当前子元素</param>
    /// <returns>子元素对象</returns>
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
    ///   赋值节点本身
    /// </summary>
    /// <returns>节点的拷贝对象</returns>
    virtual XamlNodePtr CloneNode() = 0;

    /// <summary>
    ///   是否包含指定名字的子元素
    /// </summary>
    /// <param name="name">子元素名称</param>
    /// <returns>包含返回true;否则false</returns>
    virtual bool Contains(const String& name) = 0;

    /// <summary>
    ///   找到指定名字的子元素
    /// </summary>
    /// <param name="name">子元素名称</param>
    /// <returns>子元素对象</returns>
    virtual XamlNodePtr FindChild(const String& name) = 0;

    /// <summary>
    ///   获得当前子元素名称
    /// </summary>
    /// <returns>子元素名称</returns>
    virtual String GetName() = 0;
    virtual String GetValue() = 0;

    /// <summary>
    ///   查找指定名称的元素属性值
    /// </summary>
    /// <param name="name">元素属性名称</param>
    /// <returns>元素属性值</returns>
    virtual String FindAttri(const String& name) = 0;
    virtual bool ContainsAttri(const String& name) = 0;

    /// <summary>
    ///   获取子元素属性集合的麦举对象
    /// </summary>
    /// <returns>属性集合的麦举对象</returns>
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
    ///   获取根节点
    /// </summary>
    /// <returns>根节点对象</returns>
    virtual IXamlNode* GetRootNode() = 0;

    /// <summary>
    ///   另存为指定的文件
    /// </summary>
    /// <param name="uri">文件路径</param>
    /// <returns>成功返回true;否则false</returns>
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
/// 资源节点类型
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
/// 控件创建的原型声明
/// </summary>
typedef delegate<void(IXamlNode*, NodeType,ObjectPtr&)> BuilderCb;

}

#endif

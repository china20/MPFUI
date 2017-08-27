// 华勤科技版权所有 2008-2022
// 
// 文件名：xamlnodeimp.h
// 功  能：定义核心库的节点访问接口。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIMARKUPNODEIMP_H_
#define _UIMARKUPNODEIMP_H_

#include <System/Types/Const.h>
#include <System/Types/Interface.h>
#include <System/Types/Structure.h>
#include <System/Tools/TiXmlWrapper.h>

#include <System/Types/StringCore.h>

typedef rapidxml::xml_attribute<char> XmlAttri;

namespace suic 
{

/*class XamlPropImp : public IXamlProp
{
public:

    XamlPropImp(XmlNode* node);
    virtual ~XamlPropImp();

    virtual bool IsValid() { return _attri != NULL; }
    virtual void Reset();
    virtual bool HasNext();
    virtual bool HasProps();
    virtual String Value();
    virtual String Name();
    virtual String Lookup(const String&);
    virtual bool HasAttri(const String& strName);
    virtual int GetCount();

protected:

    bool _bReset;
    XmlNode* _node;
    XmlAttri* _attri;
};*/

class XamlAttris : public IXamlAttris
{
public:

    XamlAttris(XmlNode* node);
    virtual ~XamlAttris();

    virtual Object* GetOwner();
    virtual void SetOwner(Object* owner);

    virtual bool Contains(const String& name);
    virtual String FindAttri(const String& name);

    virtual void Reset();
    virtual bool HasNext();
    virtual bool HasAttris();
    virtual String GetValue();
    virtual String GetName();

    virtual String GetXml(const String& prefix);

    virtual IAttrisOp* GetAttriOp();

private:

    void InitAttris();

protected:

    XmlNode* _node;
    StringDic<String> _attris;
    StringDic<String>::Enumerator* _enumer;
};

class XamlNode : public IXamlNode
{
public:

    XamlNode(XmlNode* node);
    virtual ~XamlNode();

    virtual Object* GetOwner();
    virtual void SetOwner(Object* owner);

    /// <summary>
    ///   赋值节点本身
    /// </summary>
    /// <returns>节点的拷贝对象</returns>
    virtual XamlNodePtr CloneNode();

    /// <summary>
    ///   是否包含指定名字的子元素
    /// </summary>
    /// <param name="name">子元素名称</param>
    /// <returns>包含返回true;否则false</returns>
    virtual bool Contains(const String& name);

    /// <summary>
    ///   找到指定名字的子元素
    /// </summary>
    /// <param name="name">子元素名称</param>
    /// <returns>子元素对象</returns>
    virtual XamlNodePtr FindChild(const String& name);

    /// <summary>
    ///   获得当前子元素名称
    /// </summary>
    /// <returns>子元素名称</returns>
    virtual String GetName();
    virtual void SetName(const String& name);
    virtual String GetValue();

    /// <summary>
    ///   查找指定名称的元素属性值
    /// </summary>
    /// <param name="name">元素属性名称</param>
    /// <returns>元素属性值</returns>
    virtual String FindAttri(const String& name);
    virtual bool ContainsAttri(const String& name);

    /// <summary>
    ///   获取子元素属性集合的麦举对象
    /// </summary>
    /// <returns>属性集合的麦举对象</returns>
    virtual IXamlAttris* GetAttris();

    virtual String GetComment();
    virtual void SetComment(const String& comment);

    virtual String GetXml(const String& prefix);

    virtual void Reset();
    virtual bool HasNext();
    virtual bool HasChildren();
    virtual String CurrentName();
    virtual IXamlNode* Current();

    virtual IXamlNodeOp* GetXamlNodeOp();

protected:

    bool _bReset;
    XmlNode* _node;
    XamlNode* _curr;
    XamlAttris* _attris;
    Mulstr _name;
    String _comment;
};

class XamlDoc : public IXamlDoc
{
public:

    XamlDoc();
    XamlDoc(bool autoDel);
    virtual ~XamlDoc();

    virtual bool LoadUri(const String& uri);
    virtual bool LoadMemory(const Byte* data, int size);

    /// <summary>
    ///   获取根节点
    /// </summary>
    /// <returns>根节点对象</returns>
    virtual IXamlNode* GetRootNode();

    /// <summary>
    ///   另存为指定的文件
    /// </summary>
    /// <param name="uri">文件路径</param>
    /// <returns>成功返回true;否则false</returns>
    virtual bool SaveAs(const String& uri);
    virtual void Close();

    bool LoadUri(ResContext& resCtx, const String& name);

private:

    XamlNode* _node;
    TiXmlWrapper _doc;
};

/*
class XamlNodeImp : public IXamlDoc
{
public:

    XamlNodeImp(XmlNode* node);
    virtual ~XamlNodeImp();

    virtual bool IsValid() { return _curr != NULL; }
    virtual XamlNodePtr GetParentNode() const;
    virtual String Attribute(const String& key) const;
    virtual XamlPropPtr Attributes();
    virtual String Name();
    virtual String ChildName();
    virtual void Reset();
    virtual bool HasNext();
    virtual bool HasChildren();
    virtual XamlNodePtr ChildNode();

    //----------------------------------------------

    virtual XamlNodePtr AddChild(const String& name);
    virtual XamlNodePtr InsertChild(const IXamlNode* node);

    virtual void RemoveChildren();
    virtual void RemoveChild(const String& name, bool first);
    virtual void RemoveChild(const IXamlNode* node);


    virtual void AddAttribute(const String& key, const String& val);

    virtual void RemoveAttributes();
    virtual void RemoveAttribute(const String& key);

    virtual Mulstr GetXml();

    virtual Object* GetElement();
    virtual void SetElement(Object* elem);

    //----------------------------------------------

    virtual bool LoadUri(const String& uri);
    virtual bool LoadMemory(const Mulstr& xml);

    virtual bool SaveAs(const String& uri);
    virtual void Close();

protected:

    bool _bReset;
    XmlNode* _node;
    XmlNode* _curr;
};

inline XamlNodePtr XamlNodeImp::GetParentNode() const
{
    if (!_node || !_node->parent())
    {
        return XamlNodePtr();
    }
    else
    {
        return new XamlNodeImp(_node->parent());
    }
}

class XamlNodeDoc : public XamlNodeImp
{
public:

    XamlNodeDoc();
    virtual ~XamlNodeDoc();

    bool LoadMemory(const char* xml);
    bool LoadUri(ResContext& resCtx, const String& name);

    //----------------------------------------------

    virtual bool LoadUri(const String& uri);
    virtual bool LoadMemory(const Mulstr& xml);

    virtual bool SaveAs(const String& uri);

    virtual void Close();

protected:

    TiXmlWrapper _doc;
};
*/

}

#endif


#include <System/Resources/XamlNodeImp.h>
#include <System/Types/String.h>

namespace suic 
{

/*XamlPropImp::XamlPropImp(XmlNode* node)
    : _node(node)
    , _attri(NULL)
    , _bReset(true)
{
}

XamlPropImp::~XamlPropImp()
{

}

void XamlPropImp::Reset()
{
    _bReset = true;
    _attri = NULL;
}

bool XamlPropImp::HasNext()
{
    if (_bReset)
    {
        _bReset = false;
        _attri = _node->first_attribute();
    }
    else if (_attri)
    {
        _attri = _attri->next_attribute();
    }

    return (NULL != _attri);
}

bool XamlPropImp::HasProps()
{
    return (_node->first_attribute() != NULL);
}

String XamlPropImp::Value()
{
    String val;
    val.FromUtf8(_attri->value(), (int)_attri->value_size());
    return val;
}

String XamlPropImp::Name()
{
    String val;
    val.FromUtf8(_attri->name(), (int)_attri->name_size());
    return val;
}

String XamlPropImp::Lookup(const String& strName)
{
    XmlAttri* pAttri = _node->first_attribute(Mulstr(strName.c_str()).c_str());
    String val;

    if (pAttri)
    {
        val.FromUtf8(pAttri->value(), (int)pAttri->value_size());
    }
    
    return val;
}

bool XamlPropImp::HasAttri(const String& strName)
{
    XmlAttri* pAttri = _node->first_attribute(Mulstr(strName.c_str()).c_str());
    return (pAttri != NULL);
}

int XamlPropImp::GetCount()
{
    return 0;
}
*/
/////////////////////////////////////////////////
// XamlAttris

XamlAttris::XamlAttris(XmlNode* node)
    : _node(node)
    , _enumer(NULL)
{
    InitAttris();
}

XamlAttris::~XamlAttris()
{
    if (NULL != _enumer)
    {
        delete _enumer;
    }
}

Object* XamlAttris::GetOwner()
{
    return NULL;
}

void XamlAttris::SetOwner(Object* owner)
{

}

bool XamlAttris::Contains(const String& name)
{
    //XmlAttri* pAttri = _node->first_attribute(Mulstr(name.c_str()).c_str());
    //return (pAttri != NULL);
    return _attris.Contains(name);
}

String XamlAttris::FindAttri(const String& name)
{
    String val;
    _attris.TryGetValue(name, val);
    return val;
    /*XmlAttri* pAttri = _node->first_attribute(Mulstr(name.c_str()).c_str());
    String val;
    if (pAttri)
    {
        val.FromUtf8(pAttri->value(), (int)pAttri->value_size());
    }
    return val;*/
}

void XamlAttris::Reset()
{
    //_bReset = true;
    //_attri = NULL;
    if (_enumer == NULL)
    {
        _enumer = new StringDic<String>::Enumerator(&_attris, 0);
    }
    else
    {
        _enumer->Reset();
    }
}

bool XamlAttris::HasNext()
{
    if (_enumer == NULL)
    {
        _enumer = new StringDic<String>::Enumerator(&_attris, 0);
    }
    return _enumer->HasNext();
    /*if (_bReset)
    {
        _bReset = false;
        _attri = _node->first_attribute();
    }
    else if (_attri)
    {
        _attri = _attri->next_attribute();
    }

    return (NULL != _attri);*/
}

bool XamlAttris::HasAttris()
{
    return (_node->first_attribute() != NULL);
}

String XamlAttris::GetName()
{
    return _enumer->Current()->key;
    /*String name;
    name.FromUtf8(_attri->name(), (int)_attri->name_size());
    return name;*/
}

String XamlAttris::GetValue()
{
    return _enumer->Current()->val;
    String val;
    //val.FromUtf8(_attri->value(), (int)_attri->value_size());
    return val;
}

String XamlAttris::GetXml(const String& prefix)
{
    return String();
}

IAttrisOp* XamlAttris::GetAttriOp()
{
    return NULL;
}

void XamlAttris::InitAttris()
{
    if (NULL != _node)
    {
        XmlAttri* pAttri = _node->first_attribute();
        while (NULL != pAttri)
        {
            String name;
            String val;

            name.FromUtf8(pAttri->name(), (int)pAttri->name_size());
            val.FromUtf8(pAttri->value(), (int)pAttri->value_size());
            _attris.Add(name, val);

            pAttri = pAttri->next_attribute();
        }
        _node->remove_all_attributes();
    }
}

/////////////////////////////////////////////////
// XamlNode

XamlNode::XamlNode(XmlNode* node)
    : _node(node)
    , _curr(NULL)
    , _bReset(true)
    , _attris(NULL)
{
}

XamlNode::~XamlNode()
{
    if (NULL != _attris)
    {
        _attris->unref();
    }

    if (NULL != _curr)
    {
        _curr->unref();
    }
}

Object* XamlNode::GetOwner()
{
    return NULL;
}

void XamlNode::SetOwner(Object* owner)
{

}

XamlNodePtr XamlNode::CloneNode()
{    
    return NULL;
}


bool XamlNode::Contains(const String& name)
{
    if (NULL != _node)
    {
        return _node->first_node(Mulstr(name.c_str()).c_str()) != NULL;
    }
    else
    {
        return false;
    }
}

XamlNodePtr XamlNode::FindChild(const String& name)
{
    XamlNodePtr xamlNode;
    XmlNode* pNode = NULL;
    if (NULL != _node)
    {
        pNode = _node->first_node(Mulstr(name.c_str()).c_str());
        if (NULL != pNode)
        {
            xamlNode = new XamlNode(pNode);
        }
    }
    return xamlNode;
}

String XamlNode::GetName()
{
    String strName;
    if (NULL != _node)
    {
        strName.FromUtf8(_node->name(), (int)_node->name_size());
    }
    return strName;
}

String XamlNode::GetValue()
{
    String strVal;
    if (NULL != _node && _node->value() != NULL)
    {
        strVal.FromUtf8(_node->value(), (int)_node->value_size());
    }
    return strVal;
}

void XamlNode::SetName(const String& name)
{
    _name = name.c_str();
    if (NULL != _node)
    {
        _name.EncodeUtf8();
        _node->name(_name.c_str(), _name.Length());
    }
}

String XamlNode::FindAttri(const String& name)
{
    return GetAttris()->FindAttri(name);
    /*String strAttri;
    if (NULL != _node)
    {
        XmlAttri* pAttri = _node->first_attribute(Mulstr(name.c_str()).c_str());
        if (NULL != pAttri)
        {
            strAttri.FromUtf8(pAttri->value(), (int)pAttri->value_size());
        }
    }
    return strAttri;*/
}

bool XamlNode::ContainsAttri(const String& name)
{
    return GetAttris()->Contains(name);
    /*if (NULL != _node)
    {
        XmlAttri* pAttri = _node->first_attribute(Mulstr(name.c_str()).c_str());
        if (NULL != pAttri)
        {
            return true;
        }
    }
    return false;*/
}

//-----------------------------------------

String XamlNode::CurrentName()
{
    String strName;
    if (NULL == _curr)
    {
        _curr = NULL;
    }
    strName.FromUtf8(_curr->_node->name(), (int)_curr->_node->name_size());
    return strName;
}

void XamlNode::Reset()
{
    _bReset = true;
    if (NULL != _curr)
    {
        _curr->unref();
    }
    _curr = NULL;
}

bool XamlNode::HasNext()
{
    XmlNode* pNext = NULL;

    if (_bReset)
    {
        _bReset = false;
        pNext = _node->first_node();
    }
    else if (_curr != NULL)
    {
        pNext = _curr->_node->next_sibling();
    }

    if (NULL != _curr)
    {
        _curr->unref();
        _curr = NULL;
    }

    if (NULL != pNext)
    {
        _curr = new XamlNode(pNext);
        _curr->ref();
    }

    return (_curr != NULL);
}

bool XamlNode::HasChildren()
{
    return (_node->first_node() != NULL);
}

IXamlNode* XamlNode::Current()
{
    return _curr;
}

IXamlNodeOp* XamlNode::GetXamlNodeOp()
{
    return NULL;
}

//-------------------------------------------------------

IXamlAttris* XamlNode::GetAttris()
{
    if (NULL == _attris && NULL != _node)
    {
        _attris = new XamlAttris(_node);
        _attris->ref();
    }
    return _attris;
}

String XamlNode::GetComment()
{
    return _comment;
}

void XamlNode::SetComment(const String& comment)
{
    _comment = comment;
}

String XamlNode::GetXml(const String& prefix)
{
    return String();
}

/////////////////////////////////////////////////
// XamlDoc

XamlDoc::XamlDoc()
    : _node(NULL)
{

}

XamlDoc::XamlDoc(bool autoDel)
    : _node(NULL)
{
    setAutoDelete(autoDel);
}

XamlDoc::~XamlDoc()
{
    if (NULL != _node)
    {
        _node->unref();
    }
}

IXamlNode* XamlDoc::GetRootNode()
{
    return _node;
}

bool XamlDoc::LoadUri(const String& uri)
{
    ResContext resCtx;
    return LoadUri(resCtx, uri);
}

bool XamlDoc::SaveAs(const String& uri)
{
    return false;
}

void XamlDoc::Close()
{
    _doc.Close();
    _node = NULL;
}

bool XamlDoc::LoadMemory(const Byte* data, int size)
{
    if (_doc.LoadMemory(data, size))
    {
        _node = new XamlNode(_doc.GetRoot());
        _node->ref();
        return true;
    }
    else
    {
        InvalidValueException e(_U("invalidate resource content"), __FILELINE__);
        return false;
    }
}

bool XamlDoc::LoadUri(ResContext& resCtx, const String& name)
{
    if (_doc.LoadUri(resCtx, name))
    {
        _node = new XamlNode(_doc.GetRoot());
        _node->ref();
        return true;
    }
    else
    {
        if (name.Length() <= 256)
        {
            String strName(name.c_str());
            String strErr;

            strErr.Format(_U("resource path[%s] don't exist"), strName.c_str());
            InvalidValueException e(strErr, __FILELINE__);
        }
        else
        {
            InvalidValueException e(_U("invalidate resource content"), __FILELINE__);
            throw e;
        }
        return false;
    }
}

IXamlDoc* XamlTools::CreateXamlDoc()
{
    IXamlDoc* xamlDoc = new XamlDoc();
    xamlDoc->ref();
    return xamlDoc;
}

/////////////////////////////////////////////////
//
/*XamlNodeImp::XamlNodeImp(XmlNode* node)
    : _node(node)
    , _curr(NULL)
    , _bReset(true)
{
}

XamlNodeImp::~XamlNodeImp()
{

}

String XamlNodeImp::Attribute(const String& key) const
{
    XmlAttri* pAttri = _node->first_attribute(Mulstr(key.c_str()).c_str());
    String val;

    if (pAttri)
    {
        val.FromUtf8(pAttri->value(), (int)pAttri->value_size());
    }
    
    return val;
}

XamlPropPtr XamlNodeImp::Attributes()
{
    return new XamlPropImp(_node);
}

String XamlNodeImp::Name()
{
    String strName;
    strName.FromUtf8(_node->name(), (int)_node->name_size());
    return strName;
}

String XamlNodeImp::ChildName()
{
    String strName;
    strName.FromUtf8(_curr->name(), (int)_curr->name_size());
    return strName;
}

void XamlNodeImp::Reset()
{
    _bReset = true;
    _curr = NULL;
}

bool XamlNodeImp::HasNext()
{
    if (_bReset)
    {
        _bReset = false;
        _curr = _node->first_node();
    }
    else if (_curr)
    {
        _curr = _curr->next_sibling();
    }
    return (_curr != NULL);
}

bool XamlNodeImp::HasChildren()
{
    return (_node->first_node() != NULL);
}

XamlNodePtr XamlNodeImp::ChildNode()
{
    return new XamlNodeImp(_curr);
}

//---------------------------------------------------

XamlNodePtr XamlNodeImp::AddChild(const String& name)
{
    return NULL;
}

XamlNodePtr XamlNodeImp::InsertChild(const IXamlNode* node)
{
    return NULL;
}

void XamlNodeImp::RemoveChild(const String& name, bool first)
{

}

void XamlNodeImp::RemoveChild(const IXamlNode* node)
{

}

void XamlNodeImp::RemoveChildren()
{

}

void XamlNodeImp::AddAttribute(const String& key, const String& val)
{

}

void XamlNodeImp::RemoveAttributes()
{

}

void XamlNodeImp::RemoveAttribute(const String& key)
{

}

Mulstr XamlNodeImp::GetXml()
{
    Mulstr xml;
    return xml;
}

Object* XamlNodeImp::GetElement()
{
    return NULL;
}

void XamlNodeImp::SetElement(Object* elem)
{

}

//----------------------------------------------

bool XamlNodeImp::LoadUri(const String& uri)
{
    return false;
}

bool XamlNodeImp::LoadMemory(const Mulstr& xml)
{
    return false;
}

bool XamlNodeImp::SaveAs(const String& uri)
{
    return false;
}

void XamlNodeImp::Close()
{

}

XamlNodeDoc::XamlNodeDoc()
    : XamlNodeImp(NULL)
{

}

XamlNodeDoc::~XamlNodeDoc()
{

}

//----------------------------------------------

IXamlDoc* GetXamlDoc()
{
    IXamlDoc* xamlDoc = new XamlNodeDoc();
    xamlDoc->ref();
    return xamlDoc;
}

XamlDocPtr XamlHelper::CreateXamlDoc()
{
    IXamlDoc* xamlDoc = new XamlNodeDoc();
    return xamlDoc;
}

bool XamlNodeDoc::LoadUri(const String& uri)
{
    ResContext resCtx;
    return LoadUri(resCtx, uri);
}

bool XamlNodeDoc::LoadMemory(const Mulstr& xml)
{
    return LoadMemory(xml.c_str());
}

bool XamlNodeDoc::SaveAs(const String& uri)
{
    return false;
}

void XamlNodeDoc::Close()
{
    _doc.Close();
    _node = NULL;
    _curr = NULL;
}

bool XamlNodeDoc::LoadMemory(const char* xml)
{
    if (_doc.LoadMemory(xml))
    {
        _node = _doc.GetRoot();
        return true;
    }
    else
    {
        InvalidValueException e(_U("invalidate resource content"), __FILELINE__);
        return false;
    }
}

bool XamlNodeDoc::LoadUri(ResContext& resCtx, const String& name)
{
    if (_doc.LoadUri(resCtx, name))
    {
        _node = _doc.GetRoot();
        return true;
    }
    else
    {
        if (name.Length() <= 256)
        {
            String strName(name.c_str());
            String strErr;

            strErr.Format(_U("resource path[%s] don't exist"), strName.c_str());
            InvalidValueException e(strErr, __FILELINE__);
        }
        else
        {
            InvalidValueException e(_U("invalidate resource content"), __FILELINE__);
            throw e;
        }
        return false;
    }
}*/

}

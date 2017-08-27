///
/// 文件名：ColorNode.h
///
/// 功  能：Color资源
///
/// 作  者：汪荣
///

#ifndef _RESNODE_H
#define _RESNODE_H

#include <Core/XamlSceneNode.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include <System/Windows/ResourceUri.h>
#include <System/Graphics/RadialGradientBrush.h>

#include <Framework/Controls/GridView.h>

class ResNode;
class DesignElement;

typedef rapidxml::xml_document<char> XmlDoc;
typedef rapidxml::xml_node<char> XmlNode;
typedef rapidxml::xml_attribute<char> XmlAttr;

typedef rapidxml::xml_document<suic::Char> UXmlDoc;
typedef rapidxml::xml_node<suic::Char> UXmlNode;
typedef rapidxml::xml_attribute<suic::Char> UXmlAttr;

typedef suic::shared<ResNode> ResNodePtr;

int ReadResFromFile(const char* filename, const char* mode, Mulstr& buff);

class FileWriter
{
public:

    FileWriter()
    {
        _f = NULL;
    }

    ~FileWriter()
    {
        Close();
    }

    void Write(const String& info)
    {
        if (_f != NULL)
        {
            Mulstr str(info.c_str());
            str.EncodeUtf8();
            fwrite(str.c_str(), str.Length(), 1, _f);
        }
    }

    void WriteByte(Byte* info, int iSize)
    {
        if (_f != NULL)
        {
            fwrite(info, iSize, 1, _f);
        }
    }

    bool Open(String strFile)
    {
        Close();

        _f = fopen(Mulstr(strFile.c_str()).c_str(), "wb");

        return (NULL != _f);
    }

    void Close()
    {
        if (_f)
        {
            fclose(_f);
            _f = NULL;
        }
    }

private:

    FILE* _f;
};

class FileReader
{
public:

    FileReader()
    {
        _f = NULL;
    }

    ~FileReader()
    {
        Close();
    }

    void Read(Mulstr& info)
    {
        if (_f != NULL)
        {
            suic::Byte buff[512] = {0};

            size_t iSize = fread(buff, 1, 512, _f);
            while (iSize > 0)
            {
                info.Append((const char*)buff, (int)iSize);
                iSize = fread(buff, 512, 1, _f);
            }
        }
    }

    bool Open(String strFile)
    {
        Close();

        _f = fopen(Mulstr(strFile.c_str()).c_str(), "rb");

        return (NULL != _f);
    }

    void Close()
    {
        if (_f)
        {
            fclose(_f);
            _f = NULL;
        }
    }

private:

    FILE* _f;
};

class ResNode : public XamlNodeItem
{
public:

    static String OFFSET1;
    static String OFFSET2;
    static String OFFSET3;
    
    RTTIOfAbstractClass(ResNode)

    virtual ~ResNode() {}

    static ResNode* CreateResNode(suic::Object* val, ResNodePtr& obj);

    virtual void CloneNode(ResNodePtr& obj);
    virtual bool IsSingleValue() = 0;
    virtual void SetValue(suic::Object* val) {}
    virtual suic::Object* GetValue() { return NULL; }

    virtual suic::String GetSingleXml()
    {
        return suic::String();
    }

    virtual suic::String GetResXml(const String& offset)
    {
        return suic::String();
    }

    virtual RTTIOfInfo* GetValueRTTIType()
    {
        if (GetValue() == NULL)
        {
            return NULL;
        }
        else
        {
            return GetValue()->GetRTTIType();
        }
    }

    suic::String GetKey() const
    {
        return _key;
    }

    void SetKey(const suic::String& key)
    {
        _key = key;
    }

    String ToString()
    {
        if (NULL == GetValue())
        {
            return GetKey();
        }
        return GetKey() + _U("(") + GetValue()->GetRTTIType()->typeName + _U(")");
    }

private:

    suic::String _key;
};

class ResNodePool
{
public:

    static ResNodePool* Ins();

    ResNodePool();
    ~ResNodePool();

    bool CreateResNode(suic::DpProperty* dp, suic::Object* val, ResNodePtr& obj);

private:

    suic::PointerDic<suic::RTTIOfInfo*, suic::RTTIOfInfo*> _resNodes;
};

class NullResNode : public ResNode
{
public:

    static NullResNode* Value;

    RTTIOfClass(NullResNode)

    virtual bool IsSingleValue()
    {
        return true;
    }

    virtual void SetValue(suic::Object* val)
    {
    }

    virtual suic::Object* GetValue()
    {
        return DpProperty::UnsetValue();
    }

    virtual suic::String GetSingleXml()
    {
        return suic::String();
    }

    virtual suic::String GetResXml(const String& offset)
    {
        return suic::String();
    }

private:

    NullResNode()
    {
        ref();
        setAutoDelete(false);
        sealAutoDelete();
    }
};

class ResValueNode : public ResNode
{
public:

    RTTIOfAbstractClass(ResValueNode)

    ResValueNode();
    ResValueNode(suic::Object* val);
    virtual ~ResValueNode();

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

protected:

    void Dispose();

private:

    suic::Object* _value;
};

class SingleResNode : public ResValueNode
{
public:

    RTTIOfClass(SingleResNode)

    SingleResNode();
    SingleResNode(suic::Object* val);
    virtual ~SingleResNode();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief String类型资源
 *
 */
class StringResNode : public SingleResNode
{
public:

    RTTIOfClass(StringResNode)

    StringResNode();
    StringResNode(suic::OString* val);
    virtual ~StringResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief Integer类型资源
 *
 */
class IntegerResNode : public SingleResNode
{
public:

    RTTIOfClass(IntegerResNode)

    IntegerResNode();
    IntegerResNode(suic::Integer* val);
    virtual ~IntegerResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief Integer类型资源
 *
 */
class WHIntegerResNode : public SingleResNode
{
public:

    RTTIOfClass(WHIntegerResNode)

    WHIntegerResNode();
    WHIntegerResNode(suic::Integer* val);
    virtual ~WHIntegerResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief Double类型资源
 *
 */
class DoubleResNode : public SingleResNode
{
public:

    RTTIOfClass(DoubleResNode)

    DoubleResNode();
    DoubleResNode(suic::OFloat* val);
    virtual ~DoubleResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief Boolean类型资源
 *
 */
class BooleanResNode : public SingleResNode
{
public:

    RTTIOfClass(BooleanResNode)

    BooleanResNode();
    BooleanResNode(suic::Boolean* val);
    virtual ~BooleanResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief ORect类型资源
 *
 */
class RectResNode : public SingleResNode
{
public:

    RTTIOfClass(RectResNode)

    RectResNode();
    RectResNode(suic::ORect* val);
    virtual ~RectResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief OSize类型资源
 *
 */
class SizeResNode : public SingleResNode
{
public:

    RTTIOfClass(SizeResNode)

    SizeResNode();
    SizeResNode(suic::OSize* val);
    virtual ~SizeResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};


/**
 * @brief OPoint类型资源
 *
 */
class PointResNode : public SingleResNode
{
public:

    RTTIOfClass(PointResNode)

    PointResNode();
    PointResNode(suic::OPoint* val);
    virtual ~PointResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};


/**
 * @brief OfRect类型资源
 *
 */
class fRectResNode : public SingleResNode
{
public:

    RTTIOfClass(fRectResNode)

    fRectResNode();
    fRectResNode(suic::OfRect* val);
    virtual ~fRectResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

/**
 * @brief OfSize类型资源
 *
 */
class fSizeResNode : public SingleResNode
{
public:

    RTTIOfClass(fSizeResNode)

    fSizeResNode();
    fSizeResNode(suic::OfSize* val);
    virtual ~fSizeResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};


/**
 * @brief OfPoint类型资源
 *
 */
class fPointResNode : public SingleResNode
{
public:

    RTTIOfClass(fPointResNode)

    fPointResNode();
    fPointResNode(suic::OfPoint* val);
    virtual ~fPointResNode();

    virtual suic::String GetNodeName();
    virtual suic::String GetFormatValue();
};

class CursorResNode : public ResNode
{
public:

    RTTIOfClass(CursorResNode)

    CursorResNode();
    CursorResNode(suic::OCursor* val);
    virtual ~CursorResNode();

    static String CursorToString(suic::OCursor* pCursor);

    virtual bool IsSingleValue();
    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

private:

    suic::OCursor* _value;
};

class ExtensionResNode : public ResNode
{
public:

    RTTIOfClass(ExtensionResNode)

    ExtensionResNode();
    ExtensionResNode(suic::Extension* val);

    virtual ~ExtensionResNode();

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

    suic::String GetResourceKey();

protected:

    suic::String GetBindingModeXml(suic::Binding* binding);
    suic::String GetUpdateSourceTriggerXml(suic::Binding* binding);

private:

    suic::Extension* _value;
};

class GridViewResNode : public ResNode
{
public:

    RTTIOfClass(GridViewResNode)

    GridViewResNode();
    GridViewResNode(suic::GridView* val);

    virtual ~GridViewResNode();

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

private:

    suic::GridView* _value;
};

class BrushResNode : public ResNode
{
public:

    RTTIOfClass(BrushResNode)

    BrushResNode();
    BrushResNode(suic::Brush* val);

    virtual ~BrushResNode();

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

protected:

    bool GetSolidColorBrushResXml(const String& offset, String& strXml);

private:

    suic::Brush* _value;
};

class TransformResNode : public ResNode
{
public:

    TransformResNode();
    TransformResNode(Transform* val);

    virtual ~TransformResNode();

    RTTIOfClass(TransformResNode)

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

private:

    suic::Transform* _transform;
};

class SolidColorBrushResNode : public BrushResNode
{
public:

    SolidColorBrushResNode();
    SolidColorBrushResNode(SolidColorBrush* val);

    virtual ~SolidColorBrushResNode();

    RTTIOfClass(SolidColorBrushResNode)
    
    virtual bool IsSingleValue();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);
};

class LinearGradientBrushResNode : public BrushResNode
{
public:

    LinearGradientBrushResNode();
    LinearGradientBrushResNode(suic::LinearGradientBrush* val);

    virtual ~LinearGradientBrushResNode();

    RTTIOfClass(LinearGradientBrushResNode)

    virtual suic::String GetResXml(const String& offset);
};

class RadialGradientBrushResNode : public BrushResNode
{
public:

    RadialGradientBrushResNode();
    RadialGradientBrushResNode(suic::RadialGradientBrush* val);

    virtual ~RadialGradientBrushResNode();

    RTTIOfClass(RadialGradientBrushResNode)

    virtual suic::String GetResXml(const String& offset);
};

class ImageBrushResNode : public BrushResNode
{
public:

    ImageBrushResNode();
    ImageBrushResNode(suic::ImageBrush* val);
    virtual ~ImageBrushResNode();

    RTTIOfClass(ImageBrushResNode)

    ResourceUri* GetResPath();
    suic::ImageBrush* GetImageBrush();

    suic::ImageBrush* RefleshImageSource();
    void SetImageBrush(suic::ImageBrush* val);
    //void SetImageSource(const String& imgSource);

    virtual void SetValue(suic::Object* val);
    virtual suic::String GetResXml(const String& offset);

private:

    suic::ResourceUri _resPath;
};

class ImageSourceResNode : public ResNode
{
public:

    RTTIOfClass(ImageSourceResNode)

    ImageSourceResNode();
    ImageSourceResNode(suic::ImageSource* val);
    virtual ~ImageSourceResNode();

    virtual void CloneNode(ResNodePtr& obj);

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();
    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

    ImageSource* GetImageSource();
    void ParseResPath(const String& path);
    void SetResPath(const String& comp, const String& path);

private:

    suic::ImageSource* _source;
};

#endif

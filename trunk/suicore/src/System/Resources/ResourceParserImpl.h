// 华勤科技版权所有 2008-2022
// 
// 文件名：ResourceParserImpl.h
// 功  能：读取、解析资源。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _RESOURCEPARSERIMPL_H_
#define _RESOURCEPARSERIMPL_H_

#include <System/Tools/ResourceParser.h>
#include <System/Resources/Style.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

class ObjectParser : public IResourceParser
{
public:

    ObjectParser(Resource::ResType t)
        : IResourceParser(t)
    {
    }

    ObjectParser()
        : IResourceParser(Resource::ResType::resObject)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem);
};

template<typename T>
class BaseTypeParser : public ObjectParser
{
public:

    BaseTypeParser(Resource::ResType t)
        : ObjectParser(t)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        suic::String strVal = pNode->GetValue();
        ParseBaseType(strVal, resItem);
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
        ObjectParser::ParseValue(riTemp, val, resItem);
        if (Resource::ResType::resUnknown == resItem.type)
        {
            ParseBaseType(val, resItem);
        }
    }

private:

    void ParseBaseType(const String& val, ResourceItem& resItem)
    {
        resItem.type = GetResourceType();
        resItem.res = new T(val);
    }
};

class BrushParser : public ObjectParser
{
public:

    BrushParser(RTTIOfInfo* target, Resource::ResType t)
        : ObjectParser(t)
        , _target(target)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        Brush* brush = (Brush*)(_target->Create());

        brush->ref();
        brush->Parse(pNode);
        resItem.type = GetResourceType();
        resItem.res = brush;
        brush->unref();
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
        ObjectParser::ParseValue(riTemp, val, resItem);
        if (Resource::ResType::resUnknown == resItem.type)
        {
            resItem.type = GetResourceType();
            resItem.res = new suic::SolidColorBrush(Color::Parse(val));
        }
    }

private:

    RTTIOfInfo* _target;
};

class ImageSourceParser : public ObjectParser
{
public:

    ImageSourceParser()
        : ObjectParser(Resource::ResType::resImageSource)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        String strSource = pNode->FindAttri(_U("Source"));
        if (strSource.Empty())
        {
            strSource = pNode->GetValue();
        }
        ParseBitmap(strSource, resItem);
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
        ObjectParser::ParseValue(riTemp, val, resItem);
        if (Resource::ResType::resUnknown == resItem.type)
        {
            ParseBitmap(val, resItem);
        }
    }

private:

    void ParseBitmap(const String& val, ResourceItem& resItem)
    {
        ImageSource* pImg = new ImageSource(val);
        resItem.res = pImg;
        resItem.type = Resource::ResType::resImageSource;
    }
};

class TransformGroupParser : public ObjectParser
{
public:

    TransformGroupParser()
        : ObjectParser(Resource::ResType::resTransform)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        TransformGroup* trans = new TransformGroup();
        resItem.res = trans;
        resItem.type = Resource::ResType::resTransform;

        pNode->Reset();

        while (pNode->HasNext())
        {
            IXamlNode* curNode = pNode->Current();
            String strType = curNode->GetName();
            IResourceParser* resParser = ResourceParser::FindResourceParser(strType);
            if (NULL != resParser)
            {
                ResourceItem resItem;
                resParser->ParseNode(riTemp, curNode, resItem);
                if (resItem.type == Resource::ResType::resTransform)
                {
                    trans->AddChild(resItem.res.get());
                }
            }
        }
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class ScaleTransformParser : public ObjectParser
{
public:

    ScaleTransformParser()
        : ObjectParser(Resource::ResType::resTransform)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        ScaleTransform* trans = new ScaleTransform();
        resItem.res = trans;
        resItem.type = Resource::ResType::resTransform;

        IXamlAttris* pAttris = pNode->GetAttris();
        while (pAttris->HasNext())
        {
            String key = pAttris->GetName();
            String val = pAttris->GetValue();
            if (key.Equals(_U("CenterX")))
            {
                trans->SetCenterX(val.ToFloat());
            }
            else if (key.Equals(_U("CenterY")))
            {
                trans->SetCenterY(val.ToFloat());
            }
            else if (key.Equals(_U("ScaleX")))
            {
                trans->SetScaleX(val.ToFloat());
            }
            else if (key.Equals(_U("ScaleY")))
            {
                trans->SetScaleY(val.ToFloat());
            }
        }
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
        
    }
};

class TranslateTransformParser : public ObjectParser
{
public:

    TranslateTransformParser()
        : ObjectParser(Resource::ResType::resTransform)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        TranslateTransform* trans = new TranslateTransform();
        resItem.res = trans;
        resItem.type = Resource::ResType::resTransform;

        IXamlAttris* pAttris = pNode->GetAttris();
        while (pAttris->HasNext())
        {
            String key = pAttris->GetName();
            String val = pAttris->GetValue();
            if (key.Equals(_U("X")))
            {
                trans->SetX(val.ToFloat());
            }
            else if (key.Equals(_U("Y")))
            {
                trans->SetY(val.ToFloat());
            }
        }
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class SkewTransformParser : public ObjectParser
{
public:

    SkewTransformParser()
        : ObjectParser(Resource::ResType::resTransform)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        SkewTransform* trans = new SkewTransform();
        resItem.res = trans;
        resItem.type = Resource::ResType::resTransform;

        IXamlAttris* pAttris = pNode->GetAttris();
        while (pAttris->HasNext())
        {
            String key = pAttris->GetName();
            String val = pAttris->GetValue();
            if (key.Equals(_U("CenterX")))
            {
                trans->SetCenterX(val.ToFloat());
            }
            else if (key.Equals(_U("CenterY")))
            {
                trans->SetCenterY(val.ToFloat());
            }
            else if (key.Equals(_U("AngleX")))
            {
                trans->SetAngleX(val.ToFloat());
            }
            else if (key.Equals(_U("AngleY")))
            {
                trans->SetAngleY(val.ToFloat());
            }
        }
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class RotateTransformParser : public ObjectParser
{
public:

    RotateTransformParser()
        : ObjectParser(Resource::ResType::resTransform)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        RotateTransform* trans = new RotateTransform();
        resItem.res = trans;
        resItem.type = Resource::ResType::resTransform;

        IXamlAttris* pAttris = pNode->GetAttris();
        while (pAttris->HasNext())
        {
            String key = pAttris->GetName();
            String val = pAttris->GetValue();
            if (key.Equals(_U("CenterX")))
            {
                trans->SetCenterX(val.ToFloat());
            }
            else if (key.Equals(_U("CenterY")))
            {
                trans->SetCenterY(val.ToFloat());
            }
            else if (key.Equals(_U("Angle")))
            {
                trans->SetAngle(val.ToFloat());
            }
        }
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class Rotate3DTransformParser : public ObjectParser
{
public:

    Rotate3DTransformParser()
        : ObjectParser(Resource::ResType::resTransform)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {

    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class EffectParser : public ObjectParser
{
public:

    EffectParser()
        : ObjectParser(Resource::ResType::resEffect)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {

    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class CursorParser : public ObjectParser
{
public:

    CursorParser()
        : ObjectParser(Resource::ResType::resCursor)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        String strSource = pNode->FindAttri(_U("Source"));
        if (strSource.Empty())
        {
            strSource = pNode->GetValue();
        }
        ParseCursor(strSource, resItem);
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
        ObjectParser::ParseValue(riTemp, val, resItem);
        if (Resource::ResType::resUnknown == resItem.type)
        {
            ParseCursor(val, resItem);
        }
    }

private:

    void ParseCursor(const String& val, ResourceItem& resItem)
    {
        OCursor* cursor = OCursor::GetCusor(val);
        if (NULL == cursor)
        {
            cursor = new OCursor();
            cursor->ref();
            if (cursor->Load(val))
            {
                resItem.res = cursor;
            }
            cursor->unref();
        }
        else
        {
            resItem.res = cursor;
        }
        resItem.type = GetResourceType();
    }
};

class BooleanParser : public ObjectParser
{
public:

    BooleanParser()
        : ObjectParser(Resource::ResType::resBoolean)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
    {
        String strVal = pNode->GetValue();
        ParseBoolean(strVal, resItem);
    }

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
        ObjectParser::ParseValue(riTemp, val, resItem);
        if (Resource::ResType::resUnknown == resItem.type)
        {
            ParseBoolean(val, resItem);
        }
    }

private:

    void ParseBoolean(const String& val, ResourceItem& resItem)
    {
        if (val.CompareI(_U("True")) == 0)
        {
            resItem.res = Boolean::True;
        }
        else if (val.Equals(_U("x:Null")))
        {
            resItem.res = Boolean::Nullable;
        }
        else 
        {
            resItem.res = Boolean::False;
        }
        resItem.type = GetResourceType();
    }
};

class ArrayParser : public ObjectParser
{
public:

    ArrayParser()
        : ObjectParser(Resource::ResType::resArray)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem);

    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {
    }
};

class DictionaryParser : public ObjectParser
{
public:

    DictionaryParser()
        : ObjectParser(Resource::ResType::resDictionary)
    {
    }

    void ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem);
    void ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
    {

    }
};

class ResourceParserI
{
public:

    static void RemoveResourceParser(const String& name);
    static void RegisterResourceParser(const String& name, IResourceParser* resParser);

    static IResourceParser* FindResourceParser(const String& name);

    static void InitResourceParser();
    static void Clear();

private:

    ResourceParserI();

    static StringDic<IResourceParser*> _bases;
    static StringDic<IResourceParser*> _reses;
};

}

#endif

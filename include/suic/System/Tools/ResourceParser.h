
#ifndef _UIRESOURCEPARSER_H_
#define _UIRESOURCEPARSER_H_

#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>

#include <System/Types/XamlNode.h>
#include <System/Windows/ResourceUri.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{ 

class SUICORE_API ResourceParser
{
public:

    static void RemoveResourceParser(const String& name);
    static void RegisterResourceParser(const String& name, IResourceParser* resParser);
    static IResourceParser* FindResourceParser(const String& name);
};

inline Resource::ResType IResourceParser::GetResourceType() const
{
    return _resType;
}

class SUICORE_API ResourceParserOp
{
public:

    static int ParseReference(const String& strType, String& strKey);

    static DpProperty* DoAttachProp(const String& name);
    static void DoCommand(FrameworkElement* fe, const String& val);
    static void DoRoutedEvent(FrameworkElement* fe, RoutedEvent* routedEvt, const suic::String& name);

    static void CreateElement(const suic::String& name, suic::ObjectPtr& elem);
    
    static void ReadResource(suic::RTTIOfInfo* rInfo, const suic::String& name, IXamlNode* node, ResourceItem& resItem);
    static void ReadResource(suic::RTTIOfInfo* rInfo, suic::DpProperty* dp, const suic::String& val, ResourceItem& resItem);

    static void ParseExtension(suic::RTTIOfInfo* riTemp, suic::DpProperty* dp, const String& val, ResourceItem& resItem);
};

}

#endif


#include <System/Tools/TreeBuilder.h>
#include <System/Types/String.h>
#include <System/Interop/System.h>
#include <System/Windows/BuildFactory.h>
#include <System/Resources/XamlWalker.h>

namespace suic
{

Builder::Builder()
{
}

Builder::~Builder()
{
}

void Builder::SetResDecryption(IResDecryption* pDecrypt)
{
    suic::CoreSystem::Ins()->SetResourceDecryption(pDecrypt);
}

AutoObj Builder::BuildUri(Object* elem, const String& uri)
{
    return XamlReader::LoadUri(NULL, elem, uri);
}

AutoObj Builder::BuildMemory(Object* elem, const Byte* data, int size)
{
    return XamlReader::LoadMemory(NULL, elem, data, size);
}

AutoObj Builder::BuildNode(Object* elem, IXamlNode* node)
{
    return XamlReader::LoadNode(NULL, elem, node);
}

AutoObj XamlReader::LoadUri(Element* parent, Object* component, const String& uri)
{
    XamlWalker walk;
    walk.WalkUri(parent, component, uri);
    return walk.GetRoot();
}

AutoObj XamlReader::LoadMemory(Element* parent, Object* component, const Byte* data, int size)
{
    XamlWalker walk;
    walk.WalkMemory(parent, component, data, size);
    return walk.GetRoot();
}

AutoObj XamlReader::LoadNode(Element* parent, Object* component, IXamlNode* node)
{
    XamlWalker walk;
    walk.WalkNode(parent, component, node);
    return walk.GetRoot();
}

}


#include <System/Windows/Exception.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{ 

class SUICORE_API LogicalTreeOp 
{
public:

    static FrameworkElement* FindLogicalNode(FrameworkElement* logicalNode, String elementName);
    static FrameworkElement* GetParent(DpObject* current);
};

} 

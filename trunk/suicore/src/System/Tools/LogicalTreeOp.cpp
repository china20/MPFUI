
#include <System/Tools/LogicalTreeOp.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{ 

FrameworkElement* LogicalTreeOp::FindLogicalNode(FrameworkElement* logicalNode, String elementName)
{ 
    if (logicalNode == NULL) 
    {
        throw ArgumentNullException(_U("logicalTreeNode"), __FILELINE__); 
    }

    if (elementName.Empty())
    {
        throw ArgumentException(_U("elementName"), __FILELINE__); 
    }

    FrameworkElement* namedElement = NULL;
    FrameworkElement* childNode = NULL;

    if (logicalNode->GetName().Equals(elementName))
    {
        namedElement = logicalNode;
    }

    if (namedElement == NULL) 
    {
        for (int i = 0; i < logicalNode->GetLogicalChildrenCount(); ++i)
        {
            childNode = DynamicCast<FrameworkElement>(logicalNode->GetLogicalChild(i));

            if (childNode != NULL) 
            {
                namedElement = FindLogicalNode(childNode, elementName); 
            } 
        }
    }

    return namedElement; 
}

FrameworkElement* LogicalTreeOp::GetParent(DpObject* current)
{
    FrameworkElement* parent(RTTICast<FrameworkElement>(current));
    if (parent == NULL)
    {
        parent = CASTTOFE(VisualTreeOp::GetParent(current));
    }

    if (parent == NULL)
    {
        throw ArgumentNullException(_U("current"), __FILELINE__); 
    }

    FrameworkElement* tfe = RTTICast<FrameworkElement>(parent->GetTemplatedParent());

    while (tfe != NULL)
    {
        parent = tfe;
        tfe = RTTICast<FrameworkElement>(tfe->GetTemplatedParent());
    }

    if (current == parent)
    {
        parent = parent->GetParent();
    }

    return parent;
}

} 

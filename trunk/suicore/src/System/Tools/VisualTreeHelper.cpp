
#include <System/Tools/VisualTreeHelper.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{ 

Element* VisualTreeHelper::GetParent(DpObject* visualNode)
{
    Element* element(CastElem<Element>(visualNode));
    if (element)
    {
        return element->_parent;
    } 
    else
    {
        return NULL;
    }
} 

bool VisualTreeHelper::IsAncestorOf(Element* reference, Element* descendant)
{
    return false;
}

Element* VisualTreeHelper::FindName(Element* root, String name)
{
    if (name.Equals(root->GetName()))
    {
        return root;
    }

    int iCount = root->GetVisualChildrenCount();

    for (int i = 0; i < iCount; ++i)
    {
        Element* elem(root->GetVisualChild(i));
        if (elem)
        {
            Element* ret(VisualTreeHelper::FindName(elem, name));

            if (ret)
            {
                return ret;
            }
        }
    }

    return NULL;
}

}

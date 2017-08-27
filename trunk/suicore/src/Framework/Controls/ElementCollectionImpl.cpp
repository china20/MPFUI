/////////////////////////////////////////////////////////////////////////
// uielementcollection.cpp

#include <System/Tools/Debug.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/ElementCollection.h>

#include <System/Windows/CoreTool.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalOper.h>

namespace suic
{

ElementColl::ElementColl()
    : _visualParent(NULL)
    , _logicalParent(NULL)
    , _size(0)
{
    _items.Resize(4);
}

ElementColl::ElementColl(Element* parent, FrameworkElement* logicalParent)
    : _visualParent(parent)
    , _logicalParent(logicalParent)
    , _size(0)
{
    _items.Resize(4);
}

ElementColl::~ElementColl()
{
    Clear();
}

FrameworkElement* ElementColl::GetLogicalParent() const
{
    return _logicalParent;
}

Element* ElementColl::GetVisualParent() const
{
    return _visualParent;
}

void ElementColl::CloneTo(ElementColl* Other)
{
    Other->_visualParent = _visualParent;
    Other->_size = _size;

    int index = 0;

    // 拷贝子元素
    for (int i = 0; i < _items.Length(); i++)
    { 
        if (_items[i] != NULL)
        {
            Element* pElem(_items[i]->Clone());
            Other->_items.Add(pElem);
            pElem->ref();
            ++index;
        }
    }
}

int ElementColl::GetCount() const
{
    return _items.Length();
}

void ElementColl::ConnectChild(int index, Element* value)
{
    Debug::Assert(value != NULL);
    Debug::Assert(_items[index] == NULL); 
    Debug::Assert(value->GetUIParent() == NULL);

    value->_parentIndex = index;
    _items[index] = value;
    value->ref();

    SetLogicalParent(value);
    if (NULL != _visualParent)
    {
        _visualParent->AddVisualChild(value);
    }
}

void ElementColl::DisconnectChild(int index)
{
    Element* child = _items[index]; 
    Element* oldParent = child->GetUIParent();
    int oldParentIndex = child->_parentIndex; 

    //
    // oldParent是个智能指针，可能会提前释放
    //
    //Debug::Assert(oldParent == _visualParent);
    Debug::Assert(child != NULL);

    _items[index] = NULL;

#ifdef _DEBUG
    child->_parentIndex = -1;
#endif 

    ClearLogicalParent(child);
    _visualParent->RemoveVisualChild(child);

    child->unref();
}

void ElementColl::ClearInternal()
{
    for (int i=0; i < _items.Length(); i++)
    { 
        if (_items[i] != NULL)
        {
            DisconnectChild(i); 
        }
    }

    _items.Clear();
    _items.Resize(4);
}

void ElementColl::SetLogicalParent(Element* element)
{
    if (_logicalParent != NULL)
    {
        _logicalParent->AddLogicalChild(element);
    }
}

void ElementColl::ClearLogicalParent(Element* element)
{
    if (_logicalParent != NULL)
    {
        _logicalParent->RemoveLogicalChild(element);
    }
}

int ElementColl::AddInternal(Element* element)
{
    int newIndex = -1;

    if (element != NULL)
    {
        _items.Add(NULL);
        newIndex = _items.Length() - 1; 
        ConnectChild(newIndex, element);
    }

    _visualParent->InvalidateMeasure();

    return newIndex;
}

void ElementColl::SetInternal(int index, Element* element)
{
    if (index >= 0 && index < _items.Length()) 
    {
        Element* oldElem = _items[index];
        if (oldElem != NULL)
        {
            DisconnectChild(index);
        }

        ConnectChild(index, element);
    } 
}

void ElementColl::CheckInit(EventArg* e)
{
    if (NULL != _visualParent && _visualParent->IsInitialized())
    {
        for (int i = 0; i < _items.Length(); i++)
        { 
            if (_items[i] != NULL) 
            {
                _items[i]->RecursiveInit();
            }
        }
    }
}

static void RecursiveOnLoaded(Element* elem, suic::LoadedEventArg* e)
{
    if (!elem->IsLoaded())
    {
        elem->OnLoaded(e);
        for (int i = 0; i < elem->GetVisualChildrenCount(); ++i)
        {
            Element* child = elem->GetVisualChild(i);
            if (child != NULL)
            {
                RecursiveOnLoaded(child, e);
            }
        }
    }
}

void ElementColl::CheckLoaded(suic::LoadedEventArg* e)
{
    if (NULL != _visualParent && _visualParent->IsLoaded())
    {
        for (int i = 0; i < _items.Length(); i++)
        { 
            if (_items[i] != NULL) 
            {
                RecursiveOnLoaded(_items[i], e);
            }
        }
    }
}

int ElementColl::Add(Element* elem)
{
    return AddInternal(elem);
}

void ElementColl::Insert(int index, Element* visual)
{
    if (index < 0 || !visual) 
    {
        return;
    }

    if (visual->GetUIParent() != NULL)
    {
        return;
    }

    if (index >= _items.Length())
    {
        index = _items.Length();
        _items.Add(NULL);
    }
    else 
    {
        _items.Insert(index, NULL);
    }

    for (int i = _items.Length() - 1; i > index; i--)
    { 
        Element* child = _items[i];

        if (child != NULL) 
        { 
            child->_parentIndex = i;
        }
    }

    ConnectChild(index, visual);
}

void ElementColl::Remove(Element* visual)
{
    int indexToRemove = -1;

    if (visual != NULL) 
    {
        if (visual->GetUIParent() != _visualParent) 
        { 
            return;
        }

        Debug::Assert(visual->GetUIParent() != NULL);

        indexToRemove = visual->_parentIndex; 
        DisconnectChild(indexToRemove);
    } 
    else
    {
        for (int i = 0; i < _items.Length(); i++)
        { 
            if (_items[i] == NULL) 
            {
                indexToRemove = i; 
                break;
            }
        }
    } 

    if (indexToRemove != -1) 
    {
        for (int i = indexToRemove; i < _items.Length() - 1; i++)
        {
            Element* child = _items[i+1];
            if (child != NULL) 
            {
                child->_parentIndex = i; 
            } 
            _items[i] = child;
        }

        _items[_items.Length() - 1] = NULL;
        _items.RemoveLength(1);
    }
}

void ElementColl::RemoveAt(int index)
{
    Remove(_items[index]);
}

void ElementColl::RemoveRange(int index, int count)
{
    if (index < 0 || count < 0)
    { 
        return;
    }

    if (_items.Length() - index < count) 
    {
        return;
    }

    if (count > 0)
    { 
        int i = 0;

        for (i = index; i < index + count; i++)
        { 
            if (_items[i] != NULL) 
            {
                DisconnectChild(i); 
            }
        }

        _items.RemoveRange(index, count);

        for (i = index; i < _items.Length(); i++) 
        { 
            Element* child = _items[i];
            if (child != NULL) 
            {
                child->_parentIndex = i;
            }
        } 
    }
}

bool ElementColl::Move(Element* visual, Element* destination)
{
    int newIndex = -1;
    int oldIndex = -1; 

    if (visual->GetUIParent() == _visualParent)
    { 
        oldIndex = visual->_parentIndex;
        newIndex = destination != NULL ? destination->_parentIndex : _items.Length() - 1;

        Debug::Assert(visual->GetUIParent() != NULL); 
        Debug::Assert(destination == NULL || destination->GetUIParent() == visual->GetUIParent());
        Debug::Assert(newIndex >= 0 && newIndex < _items.Length()); 

        if (oldIndex != newIndex)
        {
            if (oldIndex < newIndex)
            {
                newIndex--;

                for (int i = oldIndex; i < newIndex; i++) 
                {
                    Element* child = _items[i + 1]; 
                    if (child != NULL)
                    {
                        child->_parentIndex = i;
                    }
                    _items[i] = child;
                } 
            } 
            else
            { 
                for (int i = oldIndex; i > newIndex; i--)
                { 
                    Element* child = _items[i - 1]; 
                    if (child != NULL)
                    { 
                        child->_parentIndex = i;
                    }
                    _items[i] = child;
                } 
            }

            visual->_parentIndex = newIndex; 
            _items[newIndex] = visual;
        } 
        return true;
    }
    else
    {
        return false;
    }
}

bool ElementColl::Swap(Element* src, Element* destination)
{
    if (src && destination 
        && src->GetUIParent() == _visualParent
        && destination->GetUIParent() == _visualParent)
    {
        int count = 0;
        Element* elem1(src);
        Element* elem2(destination);

        int parentsrc = src->_parentIndex;
        src->_parentIndex = destination->_parentIndex;
        destination->_parentIndex = parentsrc;

        _items[src->_parentIndex] = src;
        _items[destination->_parentIndex] = destination;
        return true;
    }
    else
    {
        return false;
    }
}

int ElementColl::IndexOf(Element* visual)
{
    if (visual == NULL)
    {
        for (int i = 0; i < _items.Length(); i++) 
        { 
            if (_items[i] == NULL)
            { 
                return i;
            }
        }

        // 没有找到空位置返回-1
        return -1;
    }
    // 不是子元素
    else if (visual->GetUIParent() != _visualParent) 
    {
        return -1; 
    }
    else
    {
        return visual->_parentIndex; 
    }
}

bool ElementColl::Contains(const Element* visual)
{
    if (visual == NULL) 
    { 
        for (int i = 0; i < _items.Length(); i++)
        { 
            if (_items[i] == NULL)
            {
                return true;
            } 
        }
        return false; 
    } 
    else
    {
        return (visual->GetUIParent() == _visualParent);
    }
}

void ElementColl::Clear()
{
    ClearInternal();
}

void ElementColl::Reset()
{
    for (int i=0; i < _items.Length(); i++)
    { 
        if (_items[i] != NULL)
        {
            DisconnectChild(i); 
        }
    }
    _items.Reset();
}

}

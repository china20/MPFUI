// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uielementcollection.h
// ��  �ܣ�ʵ�ֽ���Ԫ�صļ��ϡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIELEMENTCOLLECTION_H_
#define _UIELEMENTCOLLECTION_H_

#include <System/Types/Structure.h>
#include <System/Windows/Element.h>

namespace suic
{

class SUICORE_API ElementColl
{
public:

    enum eAction 
    {
        eAdd = 0,
        eRemove = 1,
    };

    ElementColl();
    ElementColl(Element* parent, FrameworkElement* logicalParent);
    virtual ~ElementColl();

    FrameworkElement* GetLogicalParent() const;
    Element* GetVisualParent() const;

    void CloneTo(ElementColl* Other);

    int GetCount() const;

    Element* GetAt(int index);
    Element* operator[](int index);
    const Element* operator[](int index) const;

    int Add(Element* visual);
    void Insert(int index, Element* visual);
    void Remove(Element* visual);
    void RemoveAt(int index);
    void RemoveRange(int index, int count);

    // �ƶ�visual��destination��destination��λ�ý��ᱻvisual�滻
    bool Move(Element* visual, Element* destination);
    // ��������Ԫ��
    bool Swap(Element* src, Element* destination);

    int IndexOf(Element* visual);
    bool Contains(const Element* visual);

    void Clear();
    void Reset();

    int AddInternal(Element* element);
    void SetInternal(int index, Element* element);

    void CheckInit(EventArg* e);
    void CheckLoaded(suic::LoadedEventArg* e);

protected:

    void ConnectChild(int index, Element* value);
    void DisconnectChild(int index);

    void SetLogicalParent(Element* element);
    void ClearLogicalParent(Element* element);
    
    void ClearInternal();

protected:

    Array<Element*> _items;
    
    Element* _visualParent;
    FrameworkElement* _logicalParent;

    int _size;

    friend class Panel;
    friend class VirtualizingStackPanel;
};

inline Element* ElementColl::operator[](int index)
{
    return _items[index];
}

inline const Element* ElementColl::operator[](int index) const
{
    return _items[index];
}

inline Element* ElementColl::GetAt(int index)
{
    return _items[index];
}

}

#endif

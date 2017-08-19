// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Decorator.h
// ��  �ܣ�������Ŀ�װ���ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
// 
// ============================================================================

#ifndef _UIDECORATOR_H_
#define _UIDECORATOR_H_

#include <Framework/Controls/Control.h>
#include <System/Tools/ElementCollection.h>

namespace suic
{

/// <summary>
/// �Զ���װ��������
/// </summary>
class SUICORE_API Adorner : public FrameworkElement
{
private:

    Element* _adornedElement;

public:

    RTTIOfClass(Adorner)

    Adorner();
    Adorner(Element* elem);
    ~Adorner();

    Element* GetAdornedElement() const;

protected:

    Size OnMeasure(const Size& availableSize);

};

struct AdornerInfo
{
    Point pos;
    Size size;
    int zOrder;
    Adorner* adorner;

    AdornerInfo(Adorner* val);
    ~AdornerInfo();
};

/// <summary>
/// װ�����㣬����װ�οؼ��ã����Ͽ����������װ��Ԫ��
/// </summary>
class SUICORE_API AdornerLayer : public FrameworkElement
{
public:

    RTTIOfClass(AdornerLayer)

    AdornerLayer();
    ~AdornerLayer();

    static AdornerLayer* GetAdornerLayer(Element* elem);

    void Add(Adorner* adorner);
    void Add(Adorner* adorner, int zOrderIndex);
    void Remove(Adorner* adorner);

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    void Clear();

protected:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

    /// <summary>
    /// ���±�װ��Ԫ�ص����ꡢ��С����Ϣ
    /// </summary>
    /// <param name="elem">��װ�ε�Ŀ��Ԫ��</param>
    /// <returns>��</returns> 
    void UpdateElementAdorners(Element* elem);
    void ComputeClipGeometry(Adorner* adorner, Element* elem);
    void UpdateAdornerInfo(AdornerInfo* aInfo);

private:

    Array<AdornerInfo*> _adornerInfos;
};

/// <summary>
/// �ؼ���װ��������
/// </summary>
class SUICORE_API Decorator : public FrameworkElement, public IAddChild
{
public:

    Decorator();
    virtual ~Decorator();

    RTTIOfClass(Decorator)

    virtual Element* GetChild();
    virtual void SetChild(Element* child);

public:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

protected:

    void AddChild(Object* child);
    void AddText(String val);
    void RemoveChild(Object* child);

    Element* GetVisualChild(int index);
    int GetVisualChildrenCount();

    Element* GetLogicalChild(int index);
    int GetLogicalChildrenCount();

protected:

    Element* _child;
};

inline Element* Decorator::GetChild()
{
    return _child;
}

/// <summary>
/// װ�����ؼ���ͨ�������ļ����ԶԿؼ���������
/// </summary>
class SUICORE_API AdornerDecorator : public Decorator
{
private:
    
    AdornerLayer* _adornerLayer;

public:

    RTTIOfClass(AdornerDecorator)

    AdornerDecorator();
    ~AdornerDecorator();

    AdornerLayer* GetAdornerLayer() const;

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    Element* GetChild();
    void SetChild(Element* child);

protected:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);
};

}

#endif

// 华勤科技版权所有 2010-2011
// 
// 文件名：Decorator.h
// 功  能：定义核心库装饰类。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
// 
// ============================================================================

#ifndef _UIDECORATOR_H_
#define _UIDECORATOR_H_

#include <Framework/Controls/Control.h>
#include <System/Tools/ElementCollection.h>

namespace suic
{

/// <summary>
/// 自定义装饰器基类
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
/// 装饰器层，用来装饰控件用，其上可以有任意个装饰元素
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
    /// 更新被装饰元素的坐标、大小等信息
    /// </summary>
    /// <param name="elem">被装饰的目标元素</param>
    /// <returns>无</returns> 
    void UpdateElementAdorners(Element* elem);
    void ComputeClipGeometry(Adorner* adorner, Element* elem);
    void UpdateAdornerInfo(AdornerInfo* aInfo);

private:

    Array<AdornerInfo*> _adornerInfos;
};

/// <summary>
/// 控件类装饰器基类
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
/// 装饰器控件，通过配置文件可以对控件进行配置
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

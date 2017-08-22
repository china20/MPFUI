// 华勤科技版权所有 2010-2011
// 
// 文件名：uiadapter.h
// 功  能：数据适配器基类，用于映射数据到界面的显示
// 
// 作  者：Sharpui开发组
// 时  间：2012-08-31
// 
// ============================================================================

#ifndef _UIADAPTER_H_
#define _UIADAPTER_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

/// <summary>
/// 实现用户数据到实际UI控件的映射（用户通过注册自己的数据适配器
/// 适配数据到真实的UI控件）
/// </summary>
class SUICORE_API ItemAdapter : public RefBase
{
public:

    virtual ~ItemAdapter() {}

    /// <summary>
    ///  估算子项的大小尺寸
    /// </summary>
    /// <remarks>
    ///  对于已知大小的子项，可通过此方法估算大小，虚拟显示会用到
    /// </remarks>
    /// <param name="item">数据子项</param>
    /// <returns>子项估计大小</returns>
    virtual Size Estimate(RefBase* item);

    /// <summary>
    ///  允许用户创建自己的子项UI容器元素
    /// </summary>
    /// <remarks>
    ///  如果有特殊需求，可以重载此函数返回合适的UI显示元素
    ///  否则，返回NULL由系统根据数据模板自动创建
    /// </remarks>
    /// <param name="container">项容器</param>
    /// <param name="item">数据项</param>
    /// <returns>无</returns>
    virtual Element* GetElement(Element* container, RefBase* item);

    /// <summary>
    ///  根据数据内容更新UI显示
    /// </summary>
    /// <remarks>
    ///  窗口布局发生改变时，UI元素对应的数据项可能发生改变，可以
    ///  通过此方法更新用户数据显示
    /// </remarks>
    /// <param name="container">项容器</param>
    /// <param name="elem">根据子项映射的元素</param>
    /// <param name="item">数据项</param>
    /// <param name="index">数据项索引</param>
    /// <returns>无</returns>
    virtual void UpdateElement(Element* container, Element* elem, RefBase* item, int index);
    virtual void SelectItemStyle(Element* elem, int index);

    /// <summary>
    ///  根据用户模板完成UI元素映射，通知Adapter
    /// </summary>
    /// <remarks>
    ///  用户可以根据此函数修改容器的属性，只有模版元素改变才会调用此方法
    /// </remarks>
    /// <param name="owner">项容器的控件（比如ListBox等）</param>
    /// <param name="container">项容器</param>
    /// <param name="elem">根据子项映射的元素</param>
    /// <returns>无</returns>
    virtual void OnElementAdapt(const Element* owner, Element* container, Element* elem);

    virtual void OnStartAdapt(Element* owner);
    virtual void OnEndAdapt(Element* owner);

};

typedef shared<ItemAdapter> ItemAdapterPtr;

};

#endif

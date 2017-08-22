
#ifndef _DELEMENT_H_
#define _DELEMENT_H_

class DElement : public suic::RefBase
{
public:

    DElement();
    virtual ~DElement();

protected:

    // 元素资源

    // 元素样式

    // 子元素
    suic::Vector<DElement*> _child;
};

#endif

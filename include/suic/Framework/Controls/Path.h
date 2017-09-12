// 华勤科技版权所有 2008-2022
// 
// 文件名：Path.h
// 功  能：路径控件
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIPATH_H_
#define _UIPATH_H_

#include <Framework/Controls/Shape.h>
#include <System/Graphics/PathFigure.h>

namespace suic
{

class SUICORE_API Path : public Shape
{
public:

    static DpProperty* DataProperty;

    static void StaticInit();
    static void OnDataPropChanged(DpObject* d, DpPropChangedEventArg* e);

    Path();
    virtual ~Path();

    RTTIOfClass(Path)

    PathGeometry* GetData();
    void SetData(PathGeometry* data);

protected:

    Size OnMeasure(const Size& size);
    void OnRender(Drawing * drawing);

private:

    PathGeometry* _path;
};

}

#endif

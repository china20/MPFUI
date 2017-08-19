// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Path.h
// ��  �ܣ�·���ؼ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
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

    static bool StaticInit();
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

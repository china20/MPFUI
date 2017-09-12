// 华勤科技版权所有 2008-2022
// 
// 文件名：TabPanel.h
// 功  能：Tab控件面板。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UITABPANEL_H_
#define _UITABPANEL_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Panel.h>

namespace suic
{

enum TabArrangeMode
{
    tamMultiple,
    tamSingle,
    tamMenu,
};

class SUIWGX_API TabPanel : public Panel
{
public:

    static DpProperty* TabArrangeModeProperty;

    TabPanel();
    ~TabPanel();

    RTTIOfClass(TabPanel);

    void ScrollToPrev();
    void ScrollToNext();

    void ScrollToHome();
    void ScrollToEnd();

    void ScrollToIntoView(Element* pItem);

    TabArrangeMode GetTabArrangeMode();
    void SetTabArrangeMode(TabArrangeMode val);

    void UpdateTabPanel();

    Dock GetTabStripPlacement();

protected:

    void OnArrange(const Size& arrangeSize);
    Size OnMeasure(const Size& constraint);

    void OnMouseWheel(MouseWheelEventArg* e);

    Size MeasureMulti(Size constraint);
    Size MeasureMenu(Size constraint);
    Size MeasureSingle(Size constraint);

    void ArrangeSingle(Size arrangeSize);

    void ArrangeVertical(Size arrangeSize);
    void ArrangeHorizontal(Size arrangeSize);

    void ArrangeMultiVertical(Size arrangeSize);
    void ArrangeMultiHorizontal(Size arrangeSize);

    static void StaticInit();

    bool AllTabsInVisible();

    friend class TabControl;

private:

    int FindFirstVisibleTab();
    int FindLastVisibleTab();

    int GetActiveRow(Array<int>& solution);
    void GetHeadersSize(Array<Float>& numArray);
    Size GetDesiredSizeWithoutMargin(Element* elem);
    void CalculateHeaderDistribution(Float rowWidthLimit, Array<Float>& headerWidth, Array<int>& numArray);

private:

    int _numHeaders;
    int _numRows;

    int _allSize;
    int _lastVisibleTab;
    int _firstVisibleTab;

    float _rowHeight;
};

}

#endif

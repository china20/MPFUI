// 华勤科技版权所有 2010-2011
// 
// 文件名：ControlAssetsWindow.h
// 功  能：控件资源库选择界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _CONTROLASSETSWINDOW_H_
# define _CONTROLASSETSWINDOW_H_

#include <main/CommonInterface.h>

using namespace suic;

class ControlAssetsWindow : public suic::Window
{
public:

    ControlAssetsWindow(IControlWindow* ctrlWnd);
    ~ControlAssetsWindow();

    void OnPreviewMouseDoubleClick(Element* sender, MouseButtonEventArg* e);

    void OnSelControlsChanged(Element* sender, SelectionChangedEventArg* e);
    void OnSelectedItemChanged(suic::Element* sender, suic::RoutedPropChangedEventArg* e);

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void RefleshControls();

private:

    IControlWindow* _controlWnd;
};

#endif

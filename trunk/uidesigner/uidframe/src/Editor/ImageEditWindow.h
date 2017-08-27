// 华勤科技版权所有 2010-2011
// 
// 文件名：ImageEditWindow.h
// 功  能：绑定编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _IMAGEEDITWINDOW_H_
# define _IMAGEEDITWINDOW_H_

#include <Main/FilterNode.h>
#include <Main/SlnTreeManager.h>
#include <Core/TypeControls.h>
#include <Main/XamlRootItem.h>

#include <Editor/ImageEditPanel.h>

using namespace suic;

class ImageEditWindow : public suic::Window
{
public:

    ImageEditWindow(Element* hostElem);
    ~ImageEditWindow();

    void OnButtonClick(Element* sender, RoutedEventArg* e);

    void SetEditBitmap(suic::ImageBrush* bmp);

    void NotifyViewboxChanged(suic::fRect rect);
    void NotifyViewcornerChanged(suic::fRect rect);

    bool IsCommit() const;

protected:

    void UpdateImagePreview();

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnViewboxChanged(Object* sender, EventArg* e);
    void OnViewcornerChanged(Object* sender, EventArg* e);

private:

    bool _isCommit;

    Element* _hostElem;
    CheckBox* _viewboxUnits;
    fRectEditor* _viewBox;
    fRectEditor* _viewCorner;

    ImageEditPanel* _imgPanel;
};

#endif

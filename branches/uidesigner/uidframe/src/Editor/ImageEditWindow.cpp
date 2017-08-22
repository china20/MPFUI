/////////////////////////////////////////////////////
// ImageEditWindow.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ImageEditWindow.h>

ImageEditWindow::ImageEditWindow(Element* hostElem)
{
    _viewBox = NULL;
    _viewCorner = NULL;
    _imgPanel = NULL;
    _viewboxUnits = NULL;
    _hostElem = hostElem;

    _isCommit = false;
}

ImageEditWindow::~ImageEditWindow()
{
}

bool ImageEditWindow::IsCommit() const
{
    return _isCommit;
}

void ImageEditWindow::OnButtonClick(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    if (sender->GetName().Equals(_U("absoluteCheck")))
    {
        if (!_viewboxUnits->IsChecked())
        {
            _imgPanel->GetEditBitmap()->SetViewboxUnits(BrushMappingMode::bmmAbsolute);
        }
        else
        {
            _imgPanel->GetEditBitmap()->SetViewboxUnits(BrushMappingMode::bmmRelativeToBoundingBox);
        }
        return;
    }
    else if (sender->GetName().Equals(_U("okBtn")))
    {
        _imgPanel->Commit();
        _isCommit = true;
    }
    else
    {
        _imgPanel->Restore();
    }

    AsyncClose();
}

void ImageEditWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    _viewBox = FindElem<RectEditor>(_U("viewBox"));
    _viewCorner = FindElem<RectEditor>(_U("viewCorner"));
    _imgPanel = FindElem<ImageEditPanel>(_U("ImgEditor"));
    _viewboxUnits = FindElem<CheckBox>(_U("absoluteCheck"));

    _viewBox->RectValueChanged += EventHandler(this, &ImageEditWindow::OnViewboxChanged);
    _viewCorner->RectValueChanged += EventHandler(this, &ImageEditWindow::OnViewcornerChanged);

    Button* okBtn = FindElem<Button>(_U("okBtn"));
    if (NULL != okBtn)
    {
        okBtn->AddClick(new ClickEventHandler(this, &ImageEditWindow::OnButtonClick));
    }

    Button* cancelBtn = FindElem<Button>(_U("cancelBtn"));
    if (NULL != cancelBtn)
    {
        cancelBtn->AddClick(new ClickEventHandler(this, &ImageEditWindow::OnButtonClick));
    }

    if (NULL != _viewboxUnits)
    {
        _viewboxUnits->AddClick(new ClickEventHandler(this, &ImageEditWindow::OnButtonClick));
    }
}

void ImageEditWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

void ImageEditWindow::OnViewboxChanged(Object* sender, EventArg* e)
{
    String strMode = _viewBox->GetChangedVal();
    suic::Rect changedRc = _viewBox->GetORect()->ToRect();
    suic::Rect coercedRc = changedRc;

    if (strMode.Equals(_U("left")))
    {
        coercedRc = _imgPanel->UpdateViewbox(changedRc.TofRect(), ImageDragMode::idmImgLeft);
    }
    else if (strMode.Equals(_U("top")))
    {
        coercedRc = _imgPanel->UpdateViewbox(changedRc.TofRect(), ImageDragMode::idmImgTop);
    }
    else if (strMode.Equals(_U("right")))
    {
        coercedRc = _imgPanel->UpdateViewbox(changedRc.TofRect(), ImageDragMode::idmImgRight);
    }
    else if (strMode.Equals(_U("bottom")))
    {
        coercedRc = _imgPanel->UpdateViewbox(changedRc.TofRect(), ImageDragMode::idmImgBottom);
    }

    if (!coercedRc.Equal(&changedRc))
    {
        _viewBox->SetORect(new ORect(coercedRc));
    }
    UpdateImagePreview();
}

void ImageEditWindow::OnViewcornerChanged(Object* sender, EventArg* e)
{
    String strMode = _viewCorner->GetChangedVal();
    suic::Rect changedRc = _viewCorner->GetORect()->ToRect();
    suic::Rect coercedRc = changedRc;

    if (strMode.Equals(_U("left")))
    {
        coercedRc = _imgPanel->UpdateViewcorner(changedRc, ImageDragMode::idmGapLeft);
    }
    else if (strMode.Equals(_U("top")))
    {
        coercedRc = _imgPanel->UpdateViewcorner(changedRc, ImageDragMode::idmGapTop);
    }
    else if (strMode.Equals(_U("right")))
    {
        coercedRc = _imgPanel->UpdateViewcorner(changedRc, ImageDragMode::idmGapRight);
    }
    else if (strMode.Equals(_U("bottom")))
    {
        coercedRc = _imgPanel->UpdateViewcorner(changedRc, ImageDragMode::idmGapBottom);
    }

    if (!coercedRc.Equal(&changedRc))
    {
        _viewCorner->SetORect(new ORect(coercedRc));
    }
    UpdateImagePreview();
}

void ImageEditWindow::UpdateImagePreview()
{
    suic::Rectangle* rect = FindElem<suic::Rectangle>(_U("imgPreview"));
    if (NULL != rect)
    {
        _imgPanel->Commit();
        rect->InvalidateVisual();
        if (NULL != _hostElem)
        {
            _hostElem->InvalidateVisual();
        }
    }
}

void ImageEditWindow::SetEditBitmap(suic::ImageBrush* bmp)
{
    ImageEditPanel* imgPanel = RTTICast<ImageEditPanel>(FindName(_U("ImgEditor")));
    if (NULL != imgPanel)
    {
        imgPanel->SetEditBitmap(bmp);
    }

    suic::Rectangle* rect = FindElem<suic::Rectangle>(_U("imgPreview"));
    if (NULL != rect)
    {
        rect->SetFill(bmp);
    }

    if (bmp->GetViewboxUnits() == BrushMappingMode::bmmAbsolute)
    {
        _viewboxUnits->SetChecked(Boolean::False);
    }
    else
    {
        _viewboxUnits->SetChecked(Boolean::True);
    }
}

void ImageEditWindow::NotifyViewboxChanged(suic::fRect rect)
{
    if (_viewBox != NULL)
    {
        _viewBox->SetORect(new ORect(rect.ToRect()));
        UpdateImagePreview();
    }
}

void ImageEditWindow::NotifyViewcornerChanged(suic::Rect rect)
{
    if (_viewCorner != NULL)
    {
        _viewCorner->SetORect(new ORect(rect));
        UpdateImagePreview();
    }
}

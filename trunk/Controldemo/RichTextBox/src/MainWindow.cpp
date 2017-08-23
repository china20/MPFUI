/////////////////////////////////////////////////////
// MainWindow.cpp

#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void MainWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
    
}

void MainWindow::OnInitialized(suic::EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void MainWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    //
    // 窗口居中
    //
    CenterWindow();

    //
    // 注册按钮事件处理
    //
    RegisterButtonEvent();

    InsertAniImage();
}

void MainWindow::RegisterButtonEvent()
{
    suic::Button* sendInfo = FindElem<suic::Button>(_U("sendBtn"));
    if (sendInfo)
    {
        sendInfo->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnSendInfoClick));
    }

    suic::Button* insertImg = FindElem<suic::Button>(_U("insertImgBtn"));
    if (insertImg)
    {
        insertImg->AddClick(new suic::ClickEventHandler(this, &MainWindow::OnInsertImageClick));
    }
}

void MainWindow::OnPreviewTextInput(suic::KeyboardEventArg* e)
{
    suic::Window::OnPreviewTextInput(e);
}

void MainWindow::OnPreviewKeyDown(suic::KeyboardEventArg* e)
{
    // 下面代码处理alt+enter键发送消息
    if (e->GetKey() == suic::Key::kNumPad0)
    {
        suic::Element* oriElem = suic::RTTICast<suic::Element>(e->GetOriginalSource());

        // 如果是信息输入框发出的消息事件，处理回车键
        if (NULL != oriElem && oriElem->GetName().Equals(_U("inputRich")))
        {
            if (e->IsControlDown())
            {
                SendInfo();
                e->SetHandled(true);
            }
        }
    }
}

void MainWindow::SendInfo()
{
    suic::RichTextBox* pInput = FindElem<suic::RichTextBox>(_U("inputRich"));
    suic::RichTextBox* pOutput = FindElem<suic::RichTextBox>(_U("RichAni"));

    if (NULL != pInput && NULL != pOutput)
    {
        suic::String strText = pInput->GetTextDoc()->GetText();

        if (!strText.Empty())
        {
            suic::ByteStream stm;

            pInput->GetTextDoc()->GetStream(&stm, false);
            pOutput->GetTextDoc()->SetStream(&stm, true);
        }

        pInput->GetTextDoc()->SetText(_U(""), false);
        pInput->Focus();
        // 隐藏焦点虚线
        suic::KeyboardNavigation::Current()->HideFocusVisual();
    }
}

void MainWindow::OnSendInfoClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    SendInfo();
    e->SetHandled(true);
}

void MainWindow::OnInsertImageClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::RichTextBox* pOutput = FindElem<suic::RichTextBox>(_U("RichAni"));
    suic::FileBrowser fb;
    if (NULL != pOutput && fb.Open(this))
    {
        suic::ImageEmbbed* imgEmb = new suic::ImageEmbbed();
        imgEmb->ref();
        if (imgEmb->Load(fb.GetFilePath()))
        {
            pOutput->InsertEmbbed(imgEmb);
        }
        imgEmb->unref();
    }
}

suic::PlayImageEmbbed* MainWindow::InsertOneImage(suic::String strDir, int count, int delay)
{
    suic::PlayImageEmbbed* imEmbbed = new suic::PlayImageEmbbed();

    for (int i = 0; i < count; ++i)
    {
        suic::String strPath;
        strPath.Format(_U("%s\\Frame%d.png"), strDir.c_str(), i);
        suic::ImageSource* bmp = new suic::ImageSource();
        bmp->SetUri(strPath);
        imEmbbed->AddImage(bmp, suic::Point(), suic::Size(), delay);
    }

    return imEmbbed;
}

void MainWindow::InsertAniImage()
{
    suic::RichTextBox* pRichOne = FindElem<suic::RichTextBox>(_U("RichAni"));

    if (NULL != pRichOne)
    {
        suic::String strDir;
        strDir = suic::FileDir::CalculatePath(_U("ControlDemo\\RichTextBox\\Gif\\"));

        suic::PlayImageEmbbed* imEmbbed = NULL;
        imEmbbed = InsertOneImage(strDir + _U("1"), 8, 300);
        pRichOne->InsertEmbbed(imEmbbed);

        imEmbbed = InsertOneImage(strDir + _U("2"), 3, 300);
        pRichOne->InsertEmbbed(imEmbbed);

        imEmbbed = InsertOneImage(strDir + _U("3"), 20, 200);
        pRichOne->InsertEmbbed(imEmbbed);

        imEmbbed = InsertOneImage(strDir + _U("4"), 22, 300);
        pRichOne->InsertEmbbed(imEmbbed);

        imEmbbed = InsertOneImage(strDir + _U("5"), 11, 200);
        pRichOne->InsertEmbbed(imEmbbed);
    }
}
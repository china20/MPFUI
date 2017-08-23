// 华勤科技版权所有 2010-2011
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：Sharpui开发组
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

class MainWindow : public suic::Window
{
public:

    MainWindow();
    ~MainWindow();

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

protected:

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

    // 处理键盘消息，截获回车键发送信息
    void OnPreviewTextInput(suic::KeyboardEventArg* e);
    void OnPreviewKeyDown(suic::KeyboardEventArg* e);
    void OnSendInfoClick(suic::Element* sender, suic::RoutedEventArg* e);
    void OnInsertImageClick(suic::Element* sender, suic::RoutedEventArg* e);

private:

    void RegisterButtonEvent();
    void InsertAniImage();

    void SendInfo();

    suic::PlayImageEmbbed* InsertOneImage(suic::String strDir, int count, int delay);
};

#endif


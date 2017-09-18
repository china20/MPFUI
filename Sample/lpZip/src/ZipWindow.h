// 华勤科技版权所有 2010-2022
// 
// 文件名：ZipWindow.h
// 功  能：压缩界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-09-16
// 
// ============================================================================

#ifndef _ZIPWINDOW_H_
#define _ZIPWINDOW_H_

#include "DriverItem.h"
#include <Framework/Controls/Menu.h>

class ZipData : public  suic::NotifyPropChanged
{
public:

    BeginMember(ZipData, suic::NotifyPropChanged)
        MemberInt(Second)
        MemberGetInt(ZipVisible)
        MemberGetInt(ZippingVisible)
        MemberFloat(ZipProgress)
        MemberGetString(ZipSpeed)
        MemberGetString(UseTime)
        MemberGetString(ZipBefore)
        MemberGetString(ZipRate)
        MemberGetString(ZipAfter)
        MemberString(ZipFile)
        MemberString(CurrentZipFile)
        MemberString(ZipFileDir)
        MemberBool(MostSpeed)
        MemberBool(MinBulk)
    EndMember()

    RTTIOfClass(ZipData)

    DefineBool(MinBulk);
    DefineBool(MostSpeed);
    DefineString(ZipFile);
    DefineString(CurrentZipFile);
    DefineString(ZipFileDir);
    DefineFloat(ZipProgress);

    ZipData()
    {
        _second = 0;
        _zipSpeed = 0;
        _useTime = 0;
        _zipRate = 0.1f;
        _zipBefore = 0.1f;
        _zipRate = 0.1f;
        _zipAfter = 0.1f;

        _zipVisible = suic::Visibility::Visible;
        _zippingVisible = suic::Visibility::Collapsed;
    }

    void DoZip()
    {
        _zipVisible = suic::Visibility::Collapsed;
        _zippingVisible = suic::Visibility::Visible;
        // 
        // 通知更新界面
        // 
        NotifyChanged("ZipVisible");
        NotifyChanged("ZippingVisible");
    }

    int GetZipVisible()
    {
        return _zipVisible;
    }

    int GetZippingVisible()
    {
        return _zippingVisible;
    }

    int GetSecond()
    {
        return _second;
    }

    void SetSecond(int val)
    {
        _second = val;
    }

    suic::String GetZipSpeed()
    {
        suic::String strSpeed;
        strSpeed.Format(_U("%.2fMB/s"), _zipSpeed);
        return strSpeed;
    }

    void SetZipSpeed(suic::Float val)
    {
        _zipSpeed = val;
        NotifyChanged("ZipSpeed");
    }

    suic::String GetUseTime()
    {
        suic::String strVal;
        strVal.Format(_U("已用时间：%02d:%02d"), _useTime / 60, _useTime % 60);
        return strVal;
    }

    void SetUseTime(int val)
    {
        _useTime = val;
        NotifyChanged("UseTime");
    }

    suic::String GetZipBefore()
    {
        suic::String strSpeed;
        strSpeed.Format(_U("压缩前：%.2f MB"), _zipBefore);
        return strSpeed;
    }

    void SetZipBefore(suic::Float val)
    {
        _zipBefore = val;
        NotifyChanged("ZipBefore");
    }

    suic::String GetZipRate()
    {
        suic::String strVal;
        strVal.Format(_U("压缩比率：%.2f"), _zipRate * 100);
        return strVal;
    }

    void SetZipRate(suic::Float val)
    {
        _zipRate = val;
        NotifyChanged("ZipRate");
    }

    suic::String GetZipAfter()
    {
        suic::String strVal;
        strVal.Format(_U("压缩后：%.2f MB"), _zipAfter);
        return strVal;
    }

    void SetZipAfter(suic::Float val)
    {
        _zipAfter = val;
        NotifyChanged("ZipAfter");
    }

private:

    int _second;
    int _zipVisible;
    int _zippingVisible;

    int _useTime;

    suic::Float _zipSpeed;
    suic::Float _zipBefore;
    suic::Float _zipRate;
    suic::Float _zipAfter;
};

// 
// 此线程用来模拟测试数据
// 
class ZipThread : public suic::Thread
{
public:

    ZipThread(ZipData* zipData, suic::InvokeProxy* reflesh)
    {
        _zipData = zipData;
        _zipData->ref();
        _reflesh = reflesh;
    }

    virtual ~ZipThread()
    {
        _zipData->ref();
    }

    void Run()
    {
        int iLoopCount = 0;

        for (;;)
        {
            if (iLoopCount > 1000)
            {
                break;
            }

            if (IsStopped())
            {
                break;
            }

            Sleep(20);

            _zipData->SetZipSpeed(iLoopCount);
            ++iLoopCount;
            _zipData->SetUseTime(iLoopCount * 0.2);
            _zipData->SetZipProgress(iLoopCount * 100 / (float)1000);
            _zipData->SetCurrentZipFile(suic::String().Format(_U("%s/%d.txt"), 
                _zipData->GetZipFileDir().c_str(), 
                iLoopCount));

            _reflesh->PostInvoker(0, NULL);
        }
    }

private:

    ZipData* _zipData;
    suic::InvokeProxy* _reflesh;
};

class ZipWindow : public suic::Window
{
public:

    // 事件映射
    BeginRoutedEvent(ZipWindow, suic::Window)
        MemberRouted(OnClickChangeDir)
        MemberRouted(OnClickZip)
        MemberRouted(OnClickPause)
        MemberRouted(OnClickCancel)
    EndRoutedEvent()

    ZipWindow();
    ~ZipWindow();

    void StopZip();

private:

    void UpdateWindowTitle();
    void OnInvoker(suic::Object* sender, suic::InvokerArg* e);

private:

    void OnClickChangeDir(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickZip(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickPause(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnCheckedDown(suic::DpObject* sender, suic::RoutedEventArg* e);

protected:

    void OnLoaded(suic::LoadedEventArg* e);

    // 创建自己的控件对象
    bool OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj);
    void OnConnect(suic::IXamlNode* pNode, suic::Object* target);

    // 窗口关闭事件
    void OnClosing(suic::CancelEventArg* e);

private:

    ZipData* _zipData;
    ZipThread* _zipThread;
    suic::InvokeProxy* _reflesh;
};

#endif

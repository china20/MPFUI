// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// InfoBox.cpp

#include <Extend/Controls/InfoBox.h>
#include <System/Tools/HwndHelper.h>

#include <Framework/Controls/ButtonBase.h>
#include <Framework/Animation/AnimationAsSeries.h>

namespace suic
{

/////////////////////////////////////////////////////////
// InfoBox

ImplementRTTIOfClass(InfoBox, Window)
ImplementRTTIOfClass(Toast, Window)

DpProperty* InfoBox::ButtonTypeProperty;

class ButtonTypeConvert : public IConvertValue
{
public:

    ButtonTypeConvert()
    {
    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_U("OKCancel")))
        {
            return Integer::GetPosInt(InfoBox::ButtonType::IB_OKCancel);
        }
        else if (val.Equals(_U("YesNo")))
        {
            return Integer::GetPosInt(InfoBox::ButtonType::IB_YesNo);
        }
        else if (val.Equals(_U("YesNoCancel")))
        {
            return Integer::GetPosInt(InfoBox::ButtonType::IB_YesNoCancel);
        }
        else
        {
            return Integer::GetPosInt(InfoBox::ButtonType::IB_OK);
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String strVal;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case InfoBox::ButtonType::IB_OKCancel:
            strVal = _U("OKCancel");
            break;

        case InfoBox::ButtonType::IB_YesNo:
            strVal = _U("YesNo");
            break;

        case InfoBox::ButtonType::IB_YesNoCancel:
            strVal = _U("YesNoCancel");
            break;

        default:

            strVal = _U("OK");
        }

        return strVal;
    }
};

InfoBox::InfoBox()
{
    _options = 0;
    SetDialogResult(InfoBox::Result::None);
}

InfoBox::~InfoBox()
{
}

void InfoBox::StaticInit()
{
    if (ButtonTypeProperty == NULL)
    {
        Window::StaticInit();
        ButtonTypeProperty = DpProperty::Register(_T("ButtonType"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetPosInt(ButtonType::IB_OK), PropMetadataOptions::AffectsNone));
        ButtonTypeProperty->SetConvertValueCb(new ButtonTypeConvert());
    }
}

InfoBox* InfoBox::Clone()
{
    InfoBox* pObj(RTTICast<InfoBox>(ContentControl::Clone()));
    return pObj;
}

String InfoBox::ToString()
{
    return String();
}

InfoBox::Result InfoBox::Show(const String& info, const String& caption)
{
    return InfoBox::Show(HwndHelper::GetActiveWnd(), new OString(info), caption, 
        ButtonType::IB_OK, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(const String& info, const String& caption, ButtonType bt)
{
    return InfoBox::Show(HwndHelper::GetActiveWnd(), new OString(info), caption, 
        bt, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(const String& info, const String& caption, ButtonType bt, Uint32 options)
{
    return InfoBox::Show(HwndHelper::GetActiveWnd(), new OString(info), caption, bt, options);
}

InfoBox::Result InfoBox::Show(Window* main, const String& info, const String& caption)
{
    return InfoBox::Show(main, new OString(info), caption, ButtonType::IB_OK, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Window* main, const String& info, const String& caption, ButtonType bt)
{
    return InfoBox::Show(main, new OString(info), caption, bt, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Window* main, const String& info, const String& caption, ButtonType bt, Uint32 options)
{
    return InfoBox::Show(main, new OString(info), caption, bt, options);
}

InfoBox::Result InfoBox::Show(Object* info, const String& caption)
{
    return InfoBox::Show(HwndHelper::GetActiveWnd(), info, caption, ButtonType::IB_OK, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Object* info, const String& caption, ButtonType bt)
{
    return InfoBox::Show(HwndHelper::GetActiveWnd(), info, caption, ButtonType::IB_OK, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Object* info, const String& caption, ButtonType bt, Uint32 options)
{
    return InfoBox::Show(HwndHelper::GetActiveWnd(), info, caption, ButtonType::IB_OK, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Window* main, Object* info, const String& caption)
{
    return InfoBox::Show(main, info, caption, ButtonType::IB_OK, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Window* main, Object* info, const String& caption, ButtonType bt)
{
    return InfoBox::Show(main, info, caption, bt, InfoBoxOptions::Information);
}

InfoBox::Result InfoBox::Show(Window* main, Object* info, const String& caption, ButtonType bt, Uint32 options)
{
    return InfoBox::ShowInfo(main, info, caption, bt, options);
}

InfoBox::Result InfoBox::ShowInfo(Window* main, Object* info, const String& caption, ButtonType bt, Uint32 options)
{
    InfoBox infoBox;
    Handle mainhwnd = NULL;
    InfoBox::Result ret = InfoBox::Result::OK;

    StaticInit();

    info->ref();

    infoBox.SetValue(ButtonTypeProperty, Integer::GetPosInt(bt));
    infoBox.setAutoDelete(false);

    infoBox.SetTitle(caption);
    infoBox.SetContent(info);
    infoBox.SetMaxWidth(Environment::GetScreenBound().Width() * 0.66);

    infoBox._options = options;

    if (NULL != main)
    {
        mainhwnd = HwndHelper::GetHostHwnd(main);
    }

    infoBox.SetCaptionHeight(0);
    infoBox.ShowInTaskBar(false);
    infoBox.ShowDialogInternal(mainhwnd);

    ret = (InfoBox::Result)infoBox.GetDialogResult();
    info->unref();

    return ret;
}

void InfoBox::OnLoaded(LoadedEventArg* e)
{
    ContentControl::OnLoaded(e);

    ButtonType btnType = (ButtonType)GetValue(ButtonTypeProperty)->ToInt();

    InitializeButtons(btnType);

    ApplyTemplate();

    if (btnType == ButtonType::IB_OK)
    {
        Element* okElem = GetTemplateChild(_U("PART_OK"));
        if (okElem != NULL)
        {
            okElem->Focus();
        }
    }
    else if (btnType == ButtonType::IB_YesNo)
    {
        Element* yesElem = GetTemplateChild(_U("PART_Yes"));
        Element* noElem = GetTemplateChild(_U("PART_No"));

        if (noElem != NULL && NULL != yesElem)
        {
            if (_options & InfoBoxOptions::Def_Button2)
            {
                noElem->Focus();
            }
            else
            {
                yesElem->Focus();
            }
        }
    }
    else if (btnType == ButtonType::IB_OKCancel)
    {
        Element* okElem = GetTemplateChild(_U("PART_OK"));
        Element* cancelElem = GetTemplateChild(_U("PART_Cancel"));

        if (okElem != NULL && NULL != cancelElem)
        {
            if (_options & InfoBoxOptions::Def_Button2)
            {
                cancelElem->Focus();
            }
            else
            {
                okElem->Focus();
            }
        }
    }
    else if (btnType == ButtonType::IB_YesNoCancel)
    {
        Element* yesElem = GetTemplateChild(_U("PART_Yes"));
        Element* noElem = GetTemplateChild(_U("PART_No"));
        Element* cancelElem = GetTemplateChild(_U("PART_Cancel"));

        if (yesElem != NULL && NULL != noElem && NULL != cancelElem)
        {
            if (_options & InfoBoxOptions::Def_Button3)
            {
                cancelElem->Focus();
            }
            else if (_options & InfoBoxOptions::Def_Button2)
            {
                noElem->Focus();
            }
            else
            {
                yesElem->Focus();
            }
        }
    }
}

void InfoBox::SetButttonVisibilty(Element* elem, bool val)
{
    if (elem)
    {
        if (val)
        {
            elem->SetVisibility(Visibility::Visible);
        }
        else
        {
            elem->SetVisibility(Visibility::Collapsed);
        }
    }
}

void InfoBox::InitializeButtons(ButtonType bt)
{
    AddHandler(ButtonBase::ClickEvent, new ClickEventHandler(this, &InfoBox::OnButtonClick));
}

void InfoBox::OnButtonClick(Element* sender, RoutedEventArg* e)
{
    Element* elem = RTTICast<Element>(e->GetOriginalSource());
    if (NULL != elem)
    {
        String strName = elem->GetName();

        if (strName.CompareI(_U("PART_OK")) == 0)
        {
            _dialogResult = InfoBox::Result::OK;
        }
        else if (strName.CompareI(_U("PART_Cancel")) == 0)
        {
            _dialogResult = InfoBox::Result::Cancel;
        }
        else if (strName.CompareI(_U("PART_Yes")) == 0)
        {
            _dialogResult = InfoBox::Result::Yes;
        }
        else if (strName.CompareI(_U("PART_No")) == 0)
        {
            _dialogResult = InfoBox::Result::No;
        }
        else
        {
            return;
        }

        HwndHelper::CloseWindow(this, true);
    }
}

Array<Toast*, false> Toast::_showToasts;

void Toast::Show(Object* info, suic::Uint32 iDelayClose)
{
    suic::Uint32 SHORTTIME = 2;
    suic::Uint32 LONGTIME = 5;

    Toast* toastBox = new Toast();
    Handle mainhwnd = HwndHelper::GetActiveHandle();
    info->ref();
    toastBox->ref();

    toastBox->setAutoDelete(false);
    toastBox->SetValue(SizeToContentProperty, Boolean::True);
    toastBox->SetContent(info);
    toastBox->SetCaptionHeight(0);
    toastBox->ShowInTaskBar(false);
    toastBox->SetCanMoving(false);
    _showToasts.Add(toastBox);

    if (iDelayClose == DelayClose::ShortTime)
    {
        iDelayClose = SHORTTIME;
    }
    else if (iDelayClose == DelayClose::LongTime)
    {
        iDelayClose = LONGTIME;
    }
    else if (iDelayClose < SHORTTIME)
    {
        iDelayClose = SHORTTIME;
    }

    toastBox->_timer->SetInterval(iDelayClose * 1000);
    toastBox->_timer->SetTick(EventHandler(toastBox, &Toast::OnTimer));
    toastBox->_timer->Start();

    toastBox->SetOwnerHandle(mainhwnd);
    toastBox->SetTopMost(true);
    toastBox->SetAllowsTransparency(true);
    toastBox->ShowInternal(mainhwnd, 0);

    info->unref();
}

void Toast::Show(const String& info, suic::Uint32 iDelayClose)
{
    Toast::Show(new OString(info), iDelayClose);
}

Toast::Toast()
{
    _timer = new AssignerTimer();
    _timer->ref();
}

Toast::~Toast()
{
    _timer->unref();
}

void Toast::Remove()
{
    for (int i = 0; i < _showToasts.GetCount(); ++i)
    {
        if (_showToasts[i] == this)
        {
            _showToasts.RemoveAt(i);
            unref();
            break;
        }
    }
}

void Toast::OnLoaded(LoadedEventArg* e)
{
    ContentControl::OnLoaded(e);
    CenterWindow(HwndHelper::GetActiveHandle());
    Duration duration(300);
    suic::Float fOpacity = GetOpacity();
    DoubleAnimation* pAni = new DoubleAnimation(0.2f, fOpacity, duration, FillBehavior::fbHoldEnd);
    BeginAnimation(OpacityProperty, pAni);
}

void Toast::CloseCompleted(Object* sender, EventArg* e)
{
    AsyncClose();
    Remove();
}

void Toast::OnTimer(Object* sender, EventArg* e)
{
    _timer->SetEnabled(false);

    Duration duration(600);
    suic::Float fOpacity = GetOpacity();
    DoubleAnimation* pAni = new DoubleAnimation(fOpacity, 0.0f, duration, FillBehavior::fbHoldEnd);
    pAni->Completed += EventHandler(this, &Toast::CloseCompleted);
    BeginAnimation(OpacityProperty, pAni);
}

}

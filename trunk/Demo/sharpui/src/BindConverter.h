
#ifndef _BINDCONVERTER_H_
#define _BINDCONVERTER_H_

class OrientationConverter : public suic::IBindConverter
{
public:

    virtual ~OrientationConverter() {}

    virtual AutoObj Convert(suic::Object* inVal, suic::String param)
    {
        suic::String strVal = inVal->ToString();
        if (strVal.Equals(_U("Horizontal")))
        {
            return suic::OrientationBox::HorizontalBox;
        }
        else
        {
            return suic::OrientationBox::VerticalBox;
        }
    }

    virtual AutoObj ConvertBack(suic::Object* inVal, suic::String param)
    {
        return NULL;
    }
};

#endif
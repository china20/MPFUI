
#ifndef _UIIMAGEPARSE_H_
#define _UIIMAGEPARSE_H_

#include <System/Windows/DpObject.h>
#include <System/Tools/Math.h>

namespace suic
{

class SUICORE_API ImageParse : public suic::Object
{
public:

    struct Info
    {
        int wid;
        int hei;
    };

    virtual ~ImageParse() {}

    virtual void GetInfo(Info& info) = 0;
    virtual int MoveNext() = 0;
    virtual int GetDelay() = 0;
    virtual bool GetImage(Bitmap& bmp) = 0;
};

}

#endif

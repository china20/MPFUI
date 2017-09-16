
#ifndef _UIGIFPARSER_H_
#define _UIGIFPARSER_H_

#include <System/Graphics/ImageParse.h>

namespace suic
{

class SUICORE_API GIFParser : public ImageParse
{
public:
    
    GIFParser();
    virtual ~GIFParser();

    bool IsLoaded() const;

    bool LoadUri(const ResourceUri* uri);
    bool LoadMemory(const Byte* buff, int size);

    virtual void GetInfo(ImageParse::Info& info);
    virtual int MoveNext();
    virtual int GetDelay();
    virtual bool GetImage(Bitmap& bmp);

private:

    bool _isLoaded;
    ImageParse* _parser;
};

inline bool GIFParser::IsLoaded() const
{
    return _isLoaded;
}

}

#endif

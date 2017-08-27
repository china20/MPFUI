
#include <Skia/images/SkGifReader.h>

#include <System/Tools/ThemeOp.h>
#include <System/Graphics/GIFParser.h>

#include <System/Interop/System.h>

namespace suic
{

class GifParseImpl : public ImageParse
{
public:

    GifParseImpl()
    {
    }

    ~GifParseImpl()
    {
    }

    bool Load(const Byte* buff, int size)
    {
        return _gifReader.ReadMemory(buff, size) != 0;
    }

    bool Load(const ResourceUri* uri)
    {
        ByteStream data;
        int iSize = 0;

        iSize = ThemeOp::ReadRes(uri, &data);
        return Load((const Byte*)data.GetBuffer(), iSize);
    }

    void GetInfo(ImageParse::Info& info)
    {
        info.wid = _gifReader.width();
        info.hei = _gifReader.height();
    }

    bool MoveNext()
    {
        return _gifReader.MoveNext() == 0 ? false : true;
    }

    int GetDelay()
    {
        return _gifReader.GetDelay();
    }

    bool GetImage(Bitmap& bmp)
    {
        SkBitmap* skBmp = &(bmp.GetBitmapInfo()->bmp);
        return _gifReader.GetImage(skBmp) == 0 ? false : true;
    }

private:

    SkGifReader _gifReader;
};

GIFParser::GIFParser()
{
    _parser = new GifParseImpl();
    _parser->ref();
}

GIFParser::~GIFParser()
{
    _parser->unref();
}

bool GIFParser::LoadMemory(const Byte* buff, int size)
{
    GifParseImpl* gifMovie = (GifParseImpl*)_parser;
    return gifMovie->Load(buff, size);
}

bool GIFParser::LoadUri(const ResourceUri* uri)
{
    GifParseImpl* gifMovie = (GifParseImpl*)_parser;
    return gifMovie->Load(uri);
}

void GIFParser::GetInfo(ImageParse::Info& info)
{
    _parser->GetInfo(info);
}

bool GIFParser::MoveNext()
{
    return _parser->MoveNext();
}

int GIFParser::GetDelay()
{
    return _parser->GetDelay();
}

bool GIFParser::GetImage(Bitmap& bmp)
{
    return _parser->GetImage(bmp);
}

}

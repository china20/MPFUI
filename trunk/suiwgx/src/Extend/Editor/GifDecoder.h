
#include <wincodec.h>

using namespace suic;

struct GifFrame
{
    int offsetX;
    int offsetY;
    int width;
    int height;
    int delay;
    int size;
    Byte flag;
    byte* data;
};

class GifDecoder
{
public:

    GifDecoder();
    ~GifDecoder();

    bool LoadFromRes(String strRes);
    bool LoadFromFile(String strPath);
    bool LoadFromStream(IStream* strm);

    int GetFrameCount() const;
    int GetGifFrame(long index, GifFrame& frame);

    static void TestGif(String strPath);

private:

    bool ReadGidMeta();

private:

    IWICImagingFactory *_wicFactory;
    IWICBitmapDecoder *_decoder;
};

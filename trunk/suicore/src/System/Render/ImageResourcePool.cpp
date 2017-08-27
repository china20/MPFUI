/////////////////////////////////////////////////////////////////////////
// ImageResourcePool.cpp

#include "ImageResourcePool.h"

#include <System/Graphics/Bitmap.h>
#include <System/Tools/Collection.h>

namespace suic
{

static StringAutoDic<ImageSource> gs_imagepool;

ImageSource* ImageSourcePool::Lookup(const String& uri)
{
    ImageSource* bmp = NULL;
    gs_imagepool.TryGetValue(uri, bmp);
    return bmp;
}

void ImageSourcePool::AddImage(ImageSource* img)
{
    gs_imagepool.Add(img->GetUri().GenerateKey(), img);
}

void ImageSourcePool::Close()
{
    gs_imagepool.Clear();
}

void ImageSourcePool::RemoveImage(const String& uri)
{
    FreeImage(uri);
}

void ImageSourcePool::FreeImage(const String& uri)
{
    gs_imagepool.Remove(uri);
}

}

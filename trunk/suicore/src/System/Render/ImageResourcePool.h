// 华勤科技版权所有 2008-2022
// 
// 文件名：ImageResourcePool.h
// 功  能：实现界面资源文件的解析。
// 
// 作  者：MPF开发组
// 时  间：2010-07-08
// 
// ============================================================================

#ifndef _IMAGERESOURCEPOOL_H_
#define _IMAGERESOURCEPOOL_H_

#include <System/Windows/ResourceUri.h>
#include <System/Graphics/Bitmap.h>
#include <System/Types/Structure.h>

namespace suic
{

class ImageSourcePool
{
public:

    static ImageSource* Lookup(const String& uri);
    static void AddImage(ImageSource* img);
    static void RemoveImage(const String& uri);
    static void FreeImage(const String& uri);
    static void Close();

protected:

    ImageSourcePool() {};
};

}

#endif

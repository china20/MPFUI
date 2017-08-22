// 华勤科技版权所有 2010-2011
// 
// 文件名：Cursor.h
// 功  能：定义刷子基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UICURSOR_H_
#define _UICURSOR_H_

#include <System/Tools/Collection.h>
#include <System/Graphics/Resource.h>
#include <System/Windows/ResourceUri.h>

namespace suic
{

class SUICORE_API OCursor : public Resource
{
public:

    RTTIOfClass(OCursor)

    OCursor();
    OCursor(int val);

    static OCursor* Arrow;
    static OCursor* Hand;
    static OCursor* Wait;
    static OCursor* Cross;
    static OCursor* Resize;
    static OCursor* Splith;
    static OCursor* Splitv;

    static OCursor* Ibeam;
    static OCursor* SizeNwse;
    static OCursor* SizeNesw;
    static OCursor* SizeWe;
    static OCursor* SizeNs;
    static OCursor* SizeAll;

    static OCursor* GetCusor(const String& name);

    ~OCursor();

    bool Load(const String& source);
    bool LoadFromRes(Char* resId);
    bool LoadFromFile(const String& path);
    bool LoadFromMemory(ByteStream* strm);

    Handle GetHandle() const;
    String ToString();
    void SetCursor();

    void SetSealed(bool val);
    bool IsSealed() const;
    bool IsSystem() const;

    void Clear();

private:

    void LoadSystemCursor(int source);

protected:

    Handle _handle;
    ResourceUri _source;
    bool _isSealed;
    bool _isSystem;
};

inline bool OCursor::IsSealed() const
{
    return _isSealed;
}

inline bool OCursor::IsSystem() const
{
    return _isSystem;
}

}

#endif

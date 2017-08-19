// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SuiTypeface.h
// ��  �ܣ�������Ŀ���������ԡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISUITYPEFACE_H_
#define _UISUITYPEFACE_H_

#include <System/Types/Types.h>
#include <System/Graphics/Resource.h>

namespace suic
{

struct SUICORE_API SuiTypeface
{
public:

    enum Style 
    {
        fNormal = 0,
        fBold   = 0x01,
        fItalic = 0x02,
        fBoldItalic = 0x03
    };

    SuiTypeface();

    static bool CreateFromName(SuiTypeface* tf, const char familyName[], Style style);
    static bool CreateFromByte(SuiTypeface* tf, const Byte* data, Uint32 size, Style style);
    static bool CreateFromFile(SuiTypeface* tf, const char filePath[]);

    bool CreateFromName(const char familyName[], Style style);
    bool CreateFromByte(const Byte* data, Uint32 size, Style style);
    bool CreateFromFile(const char filePath[]);

    Style GetStyle() const;
    void* GetData() const;

    bool IsBold() const;
    bool IsItalic() const;
    bool IsFixedWidth() const;

    Uint32 GetUniqueId() const;

private:

    Uint32 _uniqueId;
    Style _style;
    bool _isFixedWidth;
    void* _data;
};

inline SuiTypeface::Style SuiTypeface::GetStyle() const
{
    return _style;
}

inline void* SuiTypeface::GetData() const
{
    return _data;
}


inline bool SuiTypeface::IsBold() const
{
    return ((SuiTypeface::Style::fBold & _style) != 0);
}

inline bool SuiTypeface::IsItalic() const
{
    return ((SuiTypeface::Style::fItalic & _style) != 0);
}

inline bool SuiTypeface::IsFixedWidth() const
{
    return _isFixedWidth;
}

inline Uint32 SuiTypeface::GetUniqueId() const
{
    return _uniqueId;
}

}

#endif

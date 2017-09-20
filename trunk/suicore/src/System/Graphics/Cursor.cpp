
// ============================================================================
// Cursor.cpp

#include <System/Types/String.h>
#include <System/Types/StreamDef.h>
#include <System/Windows/CoreTool.h>
#include <System/Graphics/Cursor.h>
#include <System/Tools/CoreHelper.h>
#include <System/Tools/ThemeOp.h>

#include <System/Render/ImageResourcePool.h>

extern HMODULE s_g_handle;

namespace suic
{

enum { ArrowTag, IBEAM, WAIT, CROSS, HAND, ICUPARROW, ICSIZENS, 
       ICSIZE, ICICON, ICSIZENWSE, ICSIZENESW, ICSIZEWE, ICSIZEALL};

static StringAutoDic<Object> G_S_Cursors;

OCursor* OCursor::Arrow;
OCursor* OCursor::Hand;
OCursor* OCursor::Wait;
OCursor* OCursor::Cross;
OCursor* OCursor::Resize;
OCursor* OCursor::Splith;
OCursor* OCursor::Splitv;

OCursor* OCursor::Ibeam;
OCursor* OCursor::SizeNwse;
OCursor* OCursor::SizeNesw;
OCursor* OCursor::SizeWe;
OCursor* OCursor::SizeNs;
OCursor* OCursor::SizeAll;

ImplementRTTIOfClassName(OCursor, Resource, Cursor)

OCursor::OCursor()
    : _handle(0)
    , _isSealed(false)
    , _isSystem(false)
{
}

OCursor::OCursor(int val)
    : _handle(0)
    , _isSealed(false)
{
    LoadSystemCursor(val);
}

OCursor::~OCursor()
{
    Clear();
}

void OCursor::SetSealed(bool val)
{
    if (!val || GetHandle())
    {
        _isSealed = val;
    }
}

typedef struct _cursordir
{
    WORD cdreserved;
    WORD cdtype;
    WORD cdcount;
}CURSORDIR;

typedef struct _cursordirentry 
{
    BYTE bwidth;
    BYTE bheight;
    BYTE bcolorcount;
    BYTE breserved;
    WORD wxhotspot;
    WORD wyhotspot;
    DWORD lbytesinres;
    DWORD dwimageoffset;

} CURSORDIRENTRY;

static OCursor* LoadDefCursor(String name, int id)
{
    OCursor* cursor = new OCursor();
    if (cursor->LoadFromRes(MAKEINTRESOURCE(id)))
    {
        G_S_Cursors.Add(name, cursor);
    }
    else
    {
        delete cursor;
        cursor = NULL;
    }
    return cursor;
}

bool StaticInitCursor()
{
    if (G_S_Cursors.GetCount() == 0)
    {
        OCursor::Arrow = new OCursor(ArrowTag);
        OCursor::Hand = new OCursor(HAND);
        OCursor::Wait = new OCursor(WAIT);
        OCursor::Cross = new OCursor(CROSS);

        OCursor::Ibeam = new OCursor(IBEAM);
        OCursor::SizeNwse = new OCursor(ICSIZENWSE);
        OCursor::SizeNesw = new OCursor(ICSIZENESW);
        OCursor::SizeWe = new OCursor(ICSIZEWE);
        OCursor::SizeNs = new OCursor(ICSIZENS);
        OCursor::SizeAll = new OCursor(ICSIZEALL);

        OCursor::Resize = LoadDefCursor(_U("Resize"), 101);
        OCursor::Splith = LoadDefCursor(_U("Splith"), 102);
        OCursor::Splitv = LoadDefCursor(_U("Splitv"), 103);

        G_S_Cursors.Add(_U("Arrow"), OCursor::Arrow);
        G_S_Cursors.Add(_U("Hand"), OCursor::Hand);
        G_S_Cursors.Add(_U("Wait"), OCursor::Wait);
        G_S_Cursors.Add(_U("Cross"), OCursor::Cross);
        
        G_S_Cursors.Add(_U("Ibeam"), OCursor::Ibeam);
        G_S_Cursors.Add(_U("SizeNwse"), OCursor::SizeNwse);
        G_S_Cursors.Add(_U("SizeNesw"), OCursor::SizeNesw);
        G_S_Cursors.Add(_U("SizeWe"), OCursor::SizeWe);
        G_S_Cursors.Add(_U("SizeNs"), OCursor::SizeNs);
        G_S_Cursors.Add(_U("Move"), OCursor::SizeAll);
    }

    return true;
}

OCursor* OCursor::GetCusor(const String& name)
{
    StaticInitCursor();

    Object* cursor = NULL;
    G_S_Cursors.TryGetValue(name, cursor);
    return (OCursor*)cursor;
}

void OCursor::LoadSystemCursor(int source)
{
    if (!IsSealed())
    {

        Clear();

        _isSealed = true;

        if (ArrowTag == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_ARROW);
        }
        else if (IBEAM == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_IBEAM);
        }
        else if (WAIT == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_WAIT);
        }
        else if (CROSS == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_CROSS);
        }
        else if (HAND == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_HAND);
        }
        else if (ICUPARROW == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_UPARROW);
        }
        else if (ICSIZE == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_SIZE);
        }
        else if (ICICON == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_ICON);
        }
        else if (ICSIZENWSE == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_SIZENWSE);
        }
        else if (ICSIZENESW == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_SIZENESW);
        }
        else if (ICSIZEWE == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_SIZEWE);
        }
        else if (ICSIZENS == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_SIZENS);
        }
        else if (ICSIZEALL == source)
        {
            _handle = (Handle)(DWORD_PTR)LoadCursor(NULL, IDC_SIZEALL);
        }
        else
        {
            _isSealed = false;
        }
    }
}

bool OCursor::LoadFromRes(Char* resId)
{
    HCURSOR hCur = ::LoadCursor(s_g_handle, resId);
    if (NULL != hCur)
    {
        _handle = (Handle)(DWORD_PTR)hCur;
        return true;
    }
    else
    {
        return false;
    }
}

bool OCursor::Load(const String& source)
{
    if (IsSealed())
    {
        return false;
    }

    Clear();

    _isSealed = true;
    _source = source;

    ByteStream data;
    int iSize = 0;

    iSize = ThemeOp::ReadRes(source, &data);

    if (iSize <= 0)
    {
        return false;
    }
    else
    {
        return LoadFromMemory(&data);
    }

    /*BYTE* pData = (BYTE*)data.c_str();
    CURSORDIRENTRY* pEntry = NULL;
    CURSORDIR* pDir = NULL;
    BYTE* pDib = NULL;
    
    // 读取光标文件头信息
    pDir = (CURSORDIR*)pData;

    // 检测是否为可用的光标
    if (pDir->cdreserved == 0 && (pDir->cdtype == 1 || pDir->cdtype == 2))
    {
        // 定位到光标头
        pEntry = (CURSORDIRENTRY*)(pData + sizeof(CURSORDIR));

        // dib位置
        pDib = pData + pEntry->dwimageoffset;

        BITMAPINFOHEADER* pHead = (BITMAPINFOHEADER*)(pDib);

        // 定位到调色板位置
        pDib += sizeof(BITMAPINFOHEADER);

        // 跳过调色板
        if (pHead->biClrUsed > 0)
        {
            pDib += pHead->biClrUsed * 4;
        }
        else
        {
            switch (pHead->biBitCount)
            {
            case 1:
                pDib += 8;
                break;

            case 4:
                pDib += 64;
                break;

            case 8:
                pDib += 256;
                break;
            }
        }

        try
        {
            // 图像数据每行字节数
            int linebytes = ((pEntry->bwidth * pHead->biBitCount + 31) / 32) * 4;

            // 这里不能直接用pHead->biSizeImage
            // 可能会不准确
            pHead->biSizeImage = linebytes * pEntry->bheight;
            // 掩码每行数据长度(掩码都是1位)
            int maskwdt = ((pEntry->bwidth + 31) / 32) * 4;
            int masksize = pEntry->bheight * maskwdt;

            BYTE* xOr = pDib;
            BYTE* xAnd = pDib + pHead->biSizeImage;

            HCURSOR hCur = NULL;
            hCur = CreateCursor(NULL, pEntry->wxhotspot
                , pEntry->bheight - pEntry->wyhotspot
                , pEntry->bwidth, pEntry->bheight, xAnd, xOr);
            _handle = (Handle)(DWORD_PTR)hCur;
        }
        catch (...)
        {

        }

        return true;
    }
    else
    {
        return false;
    }*/
}

bool OCursor::LoadFromFile(const String& path)
{
    HANDLE hcur = NULL;

    if (NULL != _handle)
    {
        Clear();
    }

    hcur = ::LoadImage(NULL, path.c_str(), 2, 0, 0, 0x10);
    if (NULL != hcur)
    {
        _handle = (Handle)(DWORD_PTR)hcur;
    }
    return (NULL != hcur);
}

bool OCursor::LoadFromMemory(ByteStream* strm)
{
    String tempFileName = FileDir::GetTempFileName();
    FileStream fStrm;
    bool bSucc = false;

    if (fStrm.Open(tempFileName, true))
    {
        fStrm.Write((Byte*)strm->GetBuffer(), strm->GetSize());
        fStrm.Close();
        bSucc = LoadFromFile(tempFileName);

        FileDir::RemoveFile(tempFileName);
    }

    return bSucc;
}

void OCursor::Clear()
{
    HCURSOR hcur = (HCURSOR)(DWORD_PTR)_handle;

    if (hcur)
    {
        ::DestroyCursor(hcur);
        _handle = 0;
    }
    _isSealed = false;
}

Handle OCursor::GetHandle() const
{
    return _handle;
}

void OCursor::SetCursor()
{
    if (GetHandle() != NULL)
    {
        HCURSOR hcur = (HCURSOR)(DWORD_PTR)GetHandle();
        ::SetCursor(hcur);
    }
}

String OCursor::ToString()
{
    String strXml;

    strXml = _T("<Cursor ");

    strXml += _T("Key=\"");
    strXml += _key;
    strXml += _T("\" ");

    strXml += _T("Source=\"");
    strXml += _source.ToString();
    strXml += _T("\" ");

    strXml = _T("/>");

    return strXml;
}

}

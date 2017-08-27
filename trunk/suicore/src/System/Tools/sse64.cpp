
#include <Windows.h>

extern "C" {

void __DoXorSSE(DWORD dwKey, char * pBuff, int nSSELen)
{
}

void __MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSSESize)
{
}

void __MemSetDWordSSE(LPVOID pBuff, DWORD dwVal, int nSSESize)
{	
}

void __DoGraySSE(LPVOID pBuff, int nSSESize)
{
    DWORD dwMask = 0xff;
    DWORD dwDiv3 = 85;
}


void __OpenAlphaSSE(LPVOID pBuff, int nSizeSSE)
{
    DWORD dwMask = 0xff000000;
}

void __SetColorKeySSE(LPVOID pBuff, int nSSESize, DWORD dwColor)
{
    DWORD dwMask = 0xffffff;
}


void __DoOrSSE(DWORD dwKey, LPVOID pBuff, int nSSELen)
{
}

void __DoAndSSE(DWORD dwKey, char * pBuff, int nSSELen)
{
}

}

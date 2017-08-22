
#include "stdafx.h"

#include "SSE.h"
#include <intrin.h>

BOOL CSSE::IsSupportSSE()
{
    #define EDX_SSE_bit 0x04000000

    static int is_support_sse = -1;

    if (-1 == is_support_sse)
    {
        int cpuInfo[4];
        
        is_support_sse = 1;
        __cpuid(cpuInfo, 0);

        if (cpuInfo[0] > 0)
        {
            __cpuid(cpuInfo, 1);

            if (cpuInfo[3] & EDX_SSE_bit)
            {
                is_support_sse = 0;
            }
        }
    }

    return (0 == is_support_sse);
}

extern "C" void ___DoXorSSE(DWORD dwKey, char * pBuff, int nLen);

void CSSE::DoXorSSE(DWORD dwKey, char * pBuff, int nLen)
{
	int nSSELen = nLen >> 4;

	if (nSSELen)
	{
#ifdef _WIN64
        ___DoXorSSE(dwKey, pBuff, nSSELen);
#endif
	}
	DoXorNormal(dwKey, pBuff + (nSSELen << 4), nLen - (nSSELen << 4));
}

void CSSE::DoXorNormal(DWORD dwKey, char * pBuff, int nLen)
{
	if (nLen <= 0)
	{
		return;
	}

	char szKey[4];
	memcpy(szKey, &dwKey, sizeof(szKey));
	int j = 0;

	for(int i = 0; i < nLen; i++)
	{
		pBuff[i] ^= szKey[j];
		j = (j + 1) % sizeof(szKey);
	}
}

void CSSE::DoXor(DWORD dwKey, char * pBuff, int nLen)
{
	if (IsSupportSSE())
	{
		DoXorSSE(dwKey, pBuff, nLen);
	}
	else
	{
		DoXorNormal(dwKey, pBuff, nLen);
	}
}

void CSSE::MemSetDWord(DWORD dwVal, LPVOID pBuff, int nSize)
{
	if (IsSupportSSE())
	{
		return MemSetDWordSSE(pBuff, dwVal, nSize);
	}
	else
	{
		return MemSetDWordNormal(pBuff, dwVal, nSize);
	}
}

void CSSE::MemCopy(LPVOID pDest, LPVOID pSrc, int nSize)
{
	if (IsSupportSSE())
	{
		MemCopySSE(pDest, pSrc, nSize);
	}
	else
	{
		memcpy(pDest, pSrc, nSize);
	}
}

extern "C" void ___MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize);
void CSSE::MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize)
{
	int nSSESize = nSize >> 4;
	int nTemp = nSSESize << 4;
	nSize -= nTemp;

	if (nSSESize)
	{
#ifdef _WIN64
        ___MemCopySSE(pDest, pSrc, nSSESize);
#else
        __asm
        {
            push ecx
            push edx
            push eax
            mov	eax, [pSrc]
            mov edx, [pDest]
            mov ecx, [nSSESize]		
memcpy_process:
            movups xmm0, [eax]
            add eax, 10h
            movups [edx], xmm0
            add edx, 10h
            dec ecx
            jnz memcpy_process
            pop eax
            pop edx
            pop ecx
            emms
        }
#endif
	}

	if(nSize)
	{
		memcpy((BYTE *)pDest + nTemp, (BYTE *)pSrc + nTemp, nSize);
	}
}

void CSSE::MemSetDWordNormal(LPVOID pBuff, DWORD dwVal, int nSize)
{
	if (nSize <= 0 || nSize % 4)
	{
		return;
	}

	for(DWORD * p = (DWORD *)pBuff; nSize > 0; nSize -= 4, p++)
	{
		*p = dwVal;
	}
}

extern "C" void ___MemSetDWordSSE(LPVOID pBuff, DWORD dwVal, int nSize);

void CSSE::MemSetDWordSSE(LPVOID pBuff, DWORD dwVal, int nSize)
{
	int nSSESize = nSize >> 4;

	if (nSSESize)
	{
#ifdef _WIN64
        ___MemSetDWordSSE(pBuff, dwVal, nSSESize);
#else
        __asm
        {
            push ecx
            push eax
            movss xmm1, [dwVal]
            shufps xmm1, xmm1, 0
            mov	eax, [pBuff]
            mov ecx, [nSSESize]		
xor_process:
            movups [eax], xmm1
            add eax, 10h
            dec ecx
            jnz xor_process
            pop eax
            pop ecx
            emms
        }
#endif
	}

	MemSetDWordNormal((BYTE *)pBuff + (nSSESize << 4), dwVal, nSize - (nSSESize << 4));
}

void CSSE::DoGray(LPVOID pBuff, int nSize)
{
	if(IsSupportSSE())
	{
		DoGraySSE(pBuff, nSize);
	}
	else
	{
		DoGrayNormal(pBuff, nSize);
	}
}

void CSSE::DoGrayNormal(LPVOID pBuff, int nSize)
{
	if (nSize <= 0 || nSize % 4)
	{
		return;
	}

	for (DWORD * p = (DWORD *)pBuff; nSize > 0; nSize -= 4, p++)
	{
		BYTE c = (GetRValue(*p) + GetGValue(*p) + (int)GetBValue(*p)) / 3;
		BYTE * pSrc = (BYTE *)p;
		*pSrc++ = c;
		*pSrc++ = c;
		*pSrc++ = c;
	}
}

extern "C" void ___DoGraySSE(LPVOID pBuff, int nSSESize);

void CSSE::DoGraySSE(LPVOID pBuff, int nSize)
{
	DWORD dwMask = 0xff;
	DWORD dwDiv3 = 85;
	int nSSESize = nSize >> 4;

	if (nSSESize)
	{
#ifdef _WIN64
        ___DoGraySSE(pBuff, nSSESize);
#else
        __asm
        {
            push ecx
            push eax
            mov ecx, [nSSESize]
            movss xmm1, [dwMask]
            shufps xmm1, xmm1, 0
            movss xmm2, [dwDiv3]
            shufps xmm2, xmm2, 0
            movaps xmm3, xmm1
            pslld xmm3, 24
            mov	eax, [pBuff]			
gray_process:
            movups xmm0, [eax]
            movaps xmm4, xmm0
            movaps xmm5, xmm0
            movaps xmm6, xmm0
            andps xmm4, xmm1
            psrld xmm5, 8
            andps xmm5, xmm1
            psrld xmm6, 16
            andps xmm6, xmm1
            paddd xmm4, xmm5
            paddd xmm4, xmm6
            pmullw xmm4, xmm2
            psrld xmm4, 8
            movaps xmm5, xmm4
            pslld xmm5, 8
            orps xmm4, xmm5
            pslld xmm5, 8
            orps xmm4, xmm5
            andps xmm0, xmm3
            orps xmm0, xmm4
            movups [eax], xmm0
            add eax, 10h
            dec ecx
            jnz gray_process
            pop eax
            pop ecx
            emms                
        }
#endif
	}

	DoGrayNormal((BYTE *)pBuff + (nSSESize << 4), nSize - (nSSESize << 4));
}

void CSSE::OpenAlpha(LPVOID pBuff, int nSize)
{
	if(IsSupportSSE())
	{
		OpenAlphaSSE(pBuff, nSize);
	}
	else
	{
		OpenAlphaNormal(pBuff, nSize);
	}
}

extern "C" void ___OpenAlphaSSE(LPVOID pBuff, int nSize);
void CSSE::OpenAlphaSSE(LPVOID pBuff, int nSize)
{
	int nSizeSSE = nSize >> 4;
	DWORD dwMask = 0xff000000;

	if (nSizeSSE)
	{
#ifdef _WIN64
        ___OpenAlphaSSE(pBuff, nSizeSSE);	
#else
        __asm
        {
            push eax
            push ecx
            movss xmm1, [dwMask]
            shufps xmm1, xmm1, 0	// 0xff00000000¼Ä´æ
            xorps xmm3, xmm3		// 0¼Ä´æ
            mov eax, [pBuff]
            mov ecx, [nSizeSSE]
open_alpha:
            movups xmm0, [eax]
            movaps xmm2, xmm0
            andps xmm2, xmm1
            pcmpeqd xmm2, xmm3
            andps xmm0, xmm2
            andps xmm2, xmm1
            orps xmm0, xmm2
            movups [eax], xmm0
            add eax, 10h
            dec ecx
            jnz open_alpha
            pop ecx
            pop eax
            emms
        }
#endif
	}

	OpenAlphaNormal((BYTE *)pBuff + (nSizeSSE << 4), nSize - (nSizeSSE << 4));
}

void CSSE::OpenAlphaNormal(LPVOID pBuff, int nSize)
{
	if (nSize <= 0 || nSize % 4)
	{
		return;
	}

	DWORD * pSrc = (DWORD *)pBuff;

	for (int i = 0; i < nSize; i += 4)
	{
		if (*pSrc & 0xff000000)
		{
			*pSrc = 0;
		}
		else
		{
			*pSrc |= 0xff000000;
		}

		pSrc++;
	}
}

void CSSE::SetColorKey(LPVOID pBuff, int nSize, DWORD dwColor)
{
	dwColor &= 0x00ffffff;

	if (IsSupportSSE())
	{
		CSSE::SetColorKeySSE(pBuff, nSize, dwColor);
	}
	else
	{
		CSSE::SetColorKeyNormal(pBuff, nSize, dwColor);
	}
}

void CSSE::SetColorKeyNormal(LPVOID pBuff, int nSize, DWORD dwColor)
{
	if (nSize <= 0 || nSize % 4)
	{
		return;
	}

	for(DWORD * pSrc = (DWORD *)pBuff; nSize > 0; nSize -= 4, pSrc++)
	{
		if((*pSrc & 0xffffff) == (dwColor & 0xffffff))
		{
			*pSrc = 0;
		}
	}
}

extern "C" void ___SetColorKeySSE(LPVOID pBuff, int nSize, DWORD dwColor);

void CSSE::SetColorKeySSE(LPVOID pBuff, int nSize, DWORD dwColor)
{
	int nSSESize = nSize >> 4;
	DWORD dwMask = 0xffffff;

	if (nSSESize)
	{
#ifdef _WIN64
        ___SetColorKeySSE(pBuff, nSSESize, dwColor);
#else
        __asm
        {
            push eax
            push ecx
            movss xmm1, [dwColor]
            shufps xmm1, xmm1, 0
            movss xmm3, [dwMask]
            shufps xmm3, xmm3, 0
            mov eax, [pBuff]
            mov ecx, [nSSESize]
color_key:
            movups xmm0, [eax]
            movaps xmm2, xmm0
            andps xmm2, xmm3
            pcmpeqd xmm2, xmm1
            pandn xmm2, xmm0
            movups [eax], xmm2
            add eax, 10h
            dec ecx
            jnz color_key
            pop ecx
            pop eax
            emms
        }
#endif
	}

	SetColorKeyNormal((BYTE *)pBuff + (nSSESize << 4), nSize - (nSSESize << 4), dwColor);
}

void CSSE::DoOr(DWORD dwKey, LPVOID pBuff, int nLen)
{
	if (IsSupportSSE())
	{
		CSSE::DoOrSSE(dwKey, pBuff, nLen);
	}
	else
	{
		CSSE::DoOrNormal(dwKey, pBuff, nLen);
	}
}

extern "C" void ___DoOrSSE(DWORD dwKey, LPVOID pBuff, int nLen);
void CSSE::DoOrSSE(DWORD dwKey, LPVOID pBuff, int nLen)
{
	int nSSELen = nLen >> 4;

	if (nSSELen)
	{
#ifdef _WIN64
        ___DoOrSSE(dwKey, pBuff, nSSELen);
#else
        __asm
        {
            push ecx
            push eax
            movss xmm1, [dwKey]
            shufps xmm1, xmm1, 0
            mov	eax, [pBuff]
            mov ecx, [nSSELen]		
xor_process:
            movups xmm0, [eax]
            orps xmm0, xmm1
            movups [eax], xmm0
            add eax, 10h
            dec ecx
            jnz xor_process
            pop eax
            pop ecx
            emms
        }
#endif
	}

	CSSE::DoOrNormal(dwKey, (BYTE *)pBuff + (nSSELen << 4), nLen - (nSSELen << 4));
}

void CSSE::DoOrNormal(DWORD dwKey, LPVOID pBuff, int nLen)
{
	if (nLen <= 0 || nLen % 4)
	{
		return;
	}

	for (DWORD * pSrc = (DWORD *)pBuff; nLen > 0; nLen -= 4, pSrc++)
	{
		*pSrc |= dwKey;
	}
}

void CSSE::DoOr(DWORD dwKey, LPVOID pBuff, int nLen, int iWid, int x, int y, int cx, int cy)
{
    LPBYTE pData = (LPBYTE)pBuff + (x * 4);

    pData += y * 4 * iWid;

    for (int i = 0; i < cy; ++i)
    {
        DoOr(dwKey, pData, cx * 4);

        pData += iWid * 4;
    }
}

void CSSE::MemSetDWord(LPVOID pBuff, DWORD dwColor, int iWid, int x, int y, int cx, int cy)
{
    x = x << 2;
    cx = cx << 2;

    int iRow = iWid << 2;
    LPBYTE pData = (LPBYTE)pBuff + x;
    pData += y * iRow;

    for (int i = 0; i < cy; ++i)
    {
        MemSetDWord(dwColor, pData, cx);
        pData += iRow;
    }
}

void CSSE::MemCopy(LPVOID pBuff, int iRBd, int xd, int yd, LPVOID pSrc, int iRBc, int xc, int yc, int cx, int cy)
{
    xd = xd << 2;
    xc = xc << 2;

    cx = cx << 2;

    LPBYTE pData = (LPBYTE)pBuff + xd;
    LPBYTE pSrcData = (LPBYTE)pSrc + xc;

    pData += yd * iRBd;
    pSrcData += yc * iRBc;

    for (int i = 0; i < cy; ++i)
    {
        MemCopy(pData, pSrcData, cx);
        pData += iRBd;
        pSrcData += iRBc;
    }
}

void CSSE::MemCopy(LPVOID pBuff, LPVOID pSrc, int iWid, int iHei, int x, int y, int cx, int cy)
{
    x = x << 2;
    cx = cx << 2;

    int iRow = iWid << 2;

    LPBYTE pData = (LPBYTE)pBuff + x;
    LPBYTE pSrcData = (LPBYTE)pSrc + x;

    pData += y * iRow;
    pSrcData += y * iRow;

    for (int i = 0; i < cy; ++i)
    {
        MemCopy(pData, pSrcData, cx);
        pData += iRow;
        pSrcData += iRow;
    }
}

DWORD CSSE::HSLToRGB(DWORD dwHsl)
{
	int r, g, b;
	int h = GetH(dwHsl);
	int s = GetS(dwHsl);
	int l = GetL(dwHsl);
	if(s == 0)
	{
		r = g = b = l;
	}
	else
	{
		int q = (l < 128) ? (l  + ((l * s) >> 8)) : (l + s - ((l * s) >> 8));
		int p = (l << 1)  - q;
		int T[3];
		T[0] = h + 120;
		T[1] = h;
		T[2] = h - 120;
		for(int i = 0; i < 3; i++)
		{
			if(T[i] < 0)
			{
				T[i] += 360;
			}
			if(T[i] > 360)
			{
				T[i] -= 360;
			}
			if(T[i] < 60)
			{
				T[i] = p + (q - p) * 6 * T[i] / 360;
			}
			else if(T[i] < 180)
			{
				T[i] = q;
			}
			else if(T[i] < 240)
			{
				T[i] = p + (q - p) * (240 - T[i]) * 6 / 360;
			}
			else 
			{
				T[i] = p;
			}
		}
		r = T[0] > 255 ? 255 : T[0];
		g = T[1] > 255 ? 255 : T[1];
		b = T[2] > 255 ? 255 : T[2];
	}
	return RGB(r, g, b);
}

DWORD CSSE::RGBToHSL(DWORD dwColor)
{
	int r = GetRValue(dwColor);
	int g = GetGValue(dwColor);
	int b = GetBValue(dwColor);
	int nMax = max(max(r, g), b);
	int nMin = min(min(r, g), b);

	int h, s, l;
	// luminance
	l = (nMax + nMin) >> 1;

	// saturation
	if(l == 0 || nMin == nMax)
	{
		s = 0;
	}
	else if(l > 0 && l <= 128)
	{
		s = ((nMax - nMin) << 8) / (nMin + nMax);
	}
	else if(l > 128)
	{
		s = ((nMax - nMin) << 8) / (512 - (nMin + nMax));
	}
	s = s > 255 ? 255 : s;

	// hue
	if(nMax == nMin)
	{
		h = 0;
	}
	else if(nMax == r)
	{
		h = 60 * (g - b) / (nMax - nMin);
	}
	else if(nMax == g)
	{
		h = 60 * (b - r) / (nMax - nMin) + 120;
	}
	else if(nMax == b)
	{
		h = 60 * (r - g) / (nMax - nMin) + 240;
	}
	if(h < 0)
	{
		h += 360;
	}
	else if(h > 360)
	{
		h = 360;
	}
	return (DWORD)MAKEWPARAM(MAKEWORD(l, s), h);
}

void CSSE::SetHSL(LPVOID pBuff, int nSize, int h, int s, int l)
{
	if(IsSupportSSE())
	{
		CSSE::SetHSLNormal(pBuff, nSize, h, s, l);
	}
	else
	{
		CSSE::SetHSLNormal(pBuff, nSize, h, s, l);
	}
}

void CSSE::SetHSLSSE(LPVOID pBuff, int nSize, int h, int s, int l)
{
}

void CSSE::SetHSLNormal(LPVOID pBuff, int nSize, int h, int s, int l)
{
	if(nSize <= 0 || nSize % 4)
	{
		return;
	}
	if(h > 0)
	{
		h = abs(h) % 361;
	}

	for(DWORD * pSrc = (DWORD *)pBuff; nSize > 0; nSize -= 4, pSrc++)
	{
		DWORD hsl = RGBToHSL(*pSrc);
		int oh = GetH(hsl);
		int os = GetS(hsl);
		int ol = GetL(hsl);
		oh = h >= 0 ? h : oh;
		os = s >= 0 ? ((os * s) >> 8) : os;
		ol = l >= 0 ? ((ol * l) >> 8) : ol;
		os = os > 255 ? 255 : os;
		ol = ol > 255 ? 255 : ol;
		*pSrc = (*pSrc & 0xff000000) | HSLToRGB(MAKELONG(MAKEWORD(ol, os), oh));
	}
}

void CSSE::MirrorX(LPVOID pBuff, int nWidth, int nHeight)
{
	for(int i = 0; i < nHeight; i++)
	{
		DWORD * pLeft = (DWORD *)pBuff + i * nWidth;
		DWORD * pRight = (DWORD *)pBuff + (i + 1) * nWidth - 1;

		while (pLeft < pRight)
		{
			DWORD dwTemp = *pLeft;
			*pLeft = *pRight;
			*pRight = dwTemp;
			pLeft++;
			pRight--;
		}
	}
}

void CSSE::MirrorY(LPVOID pBuff, int nWidth, int nHeight)
{
	DWORD * pTop = (DWORD *)pBuff;
	DWORD * pBottom = (DWORD *)pBuff + nWidth * (nHeight - 1);

	while (pTop < pBottom)
	{
		DWORD * p1 = pTop;
		DWORD * p2 = pBottom;

		for (int i = 0; i < nWidth; i++)
		{
			DWORD dwTemp = *p1;
			*p1 = *p2;
			*p2 = dwTemp;
			p1++;
			p2++;
		}

		pTop += nWidth;
		pBottom -= nWidth;
	}
}

void CSSE::AdjustAlpha(DWORD &dwColor)
{
	BYTE * pSrc = (BYTE *)&dwColor;
	BYTE bAlpha = *(pSrc + 3);

	if (bAlpha == 0)
	{
		*(DWORD *)pSrc = 0;
	}

	if (bAlpha > 0 && bAlpha < 255)
	{
		*pSrc = *pSrc * bAlpha >> 8;
		*(pSrc + 1) = *(pSrc + 1) * bAlpha >> 8;
		*(pSrc + 2) = *(pSrc + 2) * bAlpha >> 8;
	}
}

void CSSE::Stretch(LPVOID pBuff, int nWidth, int nHeight, LPVOID pDest, int cx, int cy)
{
	DWORD * p = (DWORD *)pDest;
	int nFx = (nWidth << 10) / cx;
	int nFy = (nHeight << 10) / cy;

	for (int i = 0; i < cy; i++)
	{
		for (int j = 0;j < cx; j++)
		{
			*p++ = *((DWORD *)pBuff + ((i * nFy) >> 10) * nWidth + ((j * nFx) >> 10));
		}
	}
}

void CSSE::DoAnd(DWORD dwKey, char * pBuff, int nLen)
{
	if(IsSupportSSE())
	{
		DoAndSSE(dwKey, pBuff, nLen);
	}
	else
	{
		DoAndNormal(dwKey, pBuff, nLen);
	}
}

void CSSE::DoAndNormal(DWORD dwKey, char * pBuff, int nLen)
{
	if(nLen <= 0 || nLen % 4)
	{
		return;
	}
	for(DWORD * pSrc = (DWORD *)pBuff; nLen > 0; nLen -= 4, pSrc++)
	{
		*pSrc &= dwKey;
	}
}

extern "C" void ___DoAndSSE(DWORD dwKey, char * pBuff, int nLen);
void CSSE::DoAndSSE(DWORD dwKey, char * pBuff, int nLen)
{
	int nSSELen = nLen >> 4;

	if (nSSELen)
	{
#ifdef _WIN64
        ___DoAndSSE(dwKey, pBuff, nSSELen);
#else
        __asm
        {
            push ecx
            push eax
            movss xmm1, [dwKey]
            shufps xmm1, xmm1, 0
            mov	eax, [pBuff]
            mov ecx, [nSSELen]		
and_process:
            movups xmm0, [eax]
            andps xmm0, xmm1
            movups [eax], xmm0
            add eax, 10h
            dec ecx
            jnz and_process
            pop eax
            pop ecx
            emms
        }
#endif
	}

	CSSE::DoAndNormal(dwKey, (char *)pBuff + (nSSELen << 4), nLen - (nSSELen << 4));
}
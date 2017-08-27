
#include <tchar.h>
#include "SkTextOp.h"

#include <System/Tools/CoreHelper.h>

#define CoereYCoor(y, fm) (y - fm.fTop)

int SkTextOp::ComputeTextLineCount(SkPaint& paint, const wchar_t* text, int len)
{
	int iLineCount = 0;
	int iStartPos = 0;
	int i = 0;

	for (i = 0; i < len; ++i)
	{
		// 碰到换行符，直接断行
		if (text[i] == '\n')
		{
			iStartPos = i;
			++iLineCount;
		}
		else if (text[i] == '\r')
		{
			if (i < len - 1 && text[i + 1] == '\n')
			{
				++i;
			}

			iStartPos = i;
			++iLineCount;
		}
	}

	if (iStartPos < len)
	{
		++iLineCount;
	}

	return iLineCount;
}

int SkTextOp::ComputeWrapTextLineCount(SkPaint& paint, SkScalar& outLen, SkScalar w, const wchar_t* text, int len)
{
	// 记录上一个字符位置索引
	int iStartPos = 0;
	int iLineCount = 0;
	int i = 0;

	for (i = 0; i < len; ++i)
	{
		// 碰到换行符，直接断行
		if (text[i] == '\n')
		{
			iLineCount += ComputeWrapTextLines(paint, outLen, w, text + iStartPos, i - iStartPos) + 1;
			iStartPos = i + 1;
		}
		else if (text[i] == '\r')
		{
			iLineCount += ComputeWrapTextLines(paint, outLen, w, text + iStartPos, i - iStartPos) + 1;

			if (i < len - 1 && text[i + 1] == '\n')
			{
				++i;
			}

			iStartPos = i + 1;
		}
	}

	if (iStartPos < len)
	{
		iLineCount += ComputeWrapTextLines(paint, outLen, w, text + iStartPos, len - iStartPos) + 1;
	}

	return iLineCount;
}

int SkTextOp::ComputeTextLines(SkPaint& paint, SkScalar& outLen, SkScalar w, const wchar_t* text, int len)
{
    int iCharCount = 0;
    len *= 2;

    if (len > 0)
    {
        iCharCount = paint.breakText(text, len, w, &outLen);
        iCharCount /= 2;
    }

    return iCharCount;
}

int SkTextOp::ComputeWrapTextLines(SkPaint& paint, SkScalar& outLen, SkScalar w, const wchar_t* text, int len)
{
	int iLineCount = 0;
	len *= 2;

	if (len > 0)
	{
        SkScalar measureLen = 0;
		int iCurrTextPos = 0;
		int iTextPos = paint.breakText(text, len, w, &measureLen);

		iTextPos = iTextPos  /  2.0f;

        if (measureLen > outLen)
        {
            outLen = measureLen;
        }

		while (iTextPos > 0)
		{
			iCurrTextPos += iTextPos ;
			iTextPos = paint.breakText(text + iCurrTextPos, len - iCurrTextPos * 2, w, &measureLen);

            if (measureLen > outLen)
            {
                outLen = measureLen;
            }

			if (iTextPos % 2 != 0)
			{
				iTextPos -= 1;
			}

			iTextPos = iTextPos  /  2.0f;

			if (iTextPos > 0)
			{
				++iLineCount;
			}
		}
	}

	return iLineCount;
}

//=================================================================

void SkTextOp::DrawLimitText(SkCanvas* canvas, SkPaint& paint, SkScalar x, SkScalar w, const wchar_t* text, int len, SkScalar lineSpace, SkPoint& offset)
{
	len *= 2;
	offset.fX = 0;

	if (len > 0)
	{
		int iCurrTextPos = 0;
		int iTextPos = paint.breakText(text, len, w, &(offset.fX));

		if (iTextPos % 2 != 0)
		{
			iTextPos -= 1;
		}

		iTextPos = iTextPos  /  2.0f;

		while (iTextPos > 0)
		{
			canvas->drawText(text + iCurrTextPos,  iTextPos * 2, x,  offset.fY,  paint);

			iCurrTextPos += iTextPos ;
			iTextPos = paint.breakText(text + iCurrTextPos, len - iCurrTextPos * 2, w, &(offset.fX));

			if (iTextPos % 2 != 0)
			{
				iTextPos -= 1;
			}

			iTextPos = iTextPos  /  2.0f;

			if (iTextPos > 0)
			{
				offset.fY += lineSpace;
			}
		}
	}

	offset.fX += x;
}

void SkTextOp::CoereYCor(SkPaint& paint, SkScalar& y)
{
	SkPaint::FontMetrics fm;
	paint.getFontMetrics(&fm);
    y = CoereYCoor(y, fm);
}

SkScalar SkTextOp::DrawSingle(SkCanvas* canvas, SkPaint& paint, SkScalar x, SkScalar y, SkScalar w, SkScalar h, const wchar_t* text, int len)
{
	SkScalar measureSize = 0;
	len *= 2;

	if (len > 0)
	{
		int iTextPos = paint.breakText(text, len, w, &measureSize);
		canvas->drawText(text, iTextPos,  x,  y,  paint);
	}

	return (x + measureSize);
}

SkScalar SkTextOp::DrawSingleText(SkCanvas* canvas, SkPaint& paint,const SkRect& rect, const wchar_t* text, int len)
{
    SkPaint::FontMetrics fm;
    SkScalar lineSpace = paint.getFontMetrics(&fm);
	SkScalar pos = 0;
    SkScalar top = CoereYCoor(rect.top(), fm);

	pos = DrawSingle(canvas, paint, rect.fLeft, top, rect.width(), rect.height(), text, len);

	return pos;
}

SkPoint SkTextOp::DrawLinesText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len)
{
	SkPaint::FontMetrics fm;
	paint.getFontMetrics(&fm);

	// 记录上一个字符位置索引
	int iStartPos = 0;
	int i = 0;
	SkPoint offset = SkPoint::Make(rect.fLeft, rect.fTop);

	SkScalar w = rect.width();
	SkScalar h = rect.height();
	SkScalar y = CoereYCoor(rect.top(), fm);//rect.fTop/* + fm.fDescent*/ - fm.fAscent;
	SkScalar iStartY = rect.fTop;

    SkScalar lineSpace = CalcLineSpace(fm);

	canvas->save();
	canvas->clipRect(rect);

	for (i = 0; i < len; ++i)
	{
		// 碰到换行符，直接断行
		if (text[i] == '\n')
		{
			offset.fX = DrawSingle(canvas, paint, rect.fLeft, y, w, h, text + iStartPos, i - iStartPos);
			y += lineSpace;
			iStartPos = i + 1;
		}
		else if (text[i] == '\r')
		{
			offset.fX = DrawSingle(canvas, paint, rect.fLeft, y, w, h, text + iStartPos, i - iStartPos);
			y += lineSpace;

			if (i < len - 1 && text[i + 1] == '\n')
			{
				++i;
			}

			iStartPos = i + 1;
		}

		//
		// 已经大于可见区域，直接退出
		//
		if (y - iStartY > h)
		{
			iStartPos = len;
			break;
		}
	}

	if (iStartPos < len)
	{
		offset.fX = DrawSingle(canvas, paint, rect.fLeft, y, w, h, text + iStartPos, len - iStartPos);
	}

    offset.fY = CoereYCoor(y, fm);
	//offset.fY = y - (/*fm.fDescent*/0 - fm.fAscent);

	canvas->restore();

	return offset;
}

SkPoint SkTextOp::DrawWrapText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len)
{
	SkPaint::FontMetrics fm;
	paint.getFontMetrics(&fm);

	// 记录上一个字符位置索引
	int iStartPos = 0;
	int i = 0;
	SkPoint offset = SkPoint::Make(rect.fLeft, CoereYCoor(rect.top(), fm));

	SkScalar w = rect.width();
	SkScalar h = rect.height();
	SkScalar iStartY = rect.fLeft;

    SkScalar lineSpace = CalcLineSpace(fm);

	for (i = 0; i < len; ++i)
	{
		// 碰到换行符，直接断行
		if (text[i] == '\n')
		{
			DrawLimitText(canvas, paint, rect.fLeft, w, text + iStartPos, i - iStartPos, lineSpace, offset);
			offset.fY += lineSpace;
			iStartPos = i + 1;
		}
		else if (text[i] == '\r')
		{
			DrawLimitText(canvas, paint, rect.fLeft, w, text + iStartPos, i - iStartPos, lineSpace, offset);
			offset.fY += lineSpace;

			if (i < len - 1 && text[i + 1] == '\n')
			{
				++i;
			}

			iStartPos = i + 1;
		}

		//
		// 已经大于可见区域，直接退出
		//
		if (offset.fY - iStartY > h)
		{
			iStartPos = len;
			break;
		}
	}

	if (iStartPos < len)
	{
		DrawLimitText(canvas, paint, rect.fLeft, w, text + iStartPos, len - iStartPos, lineSpace, offset);
	}

    offset.fY -= CoereYCoor(0, fm);
	//offset.fY -= /*fm.fDescent*/0 - fm.fAscent;

	return offset;
}

void SkTextOp::DrawText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len, int fmt)
{
    if ((fmt & (int)SkTextOp::TextFormat::tSingleLine) == 0)
    {
        if ((fmt & (int)SkTextOp::TextFormat::tWrapText) != 0)
        {
            DrawWrapText(canvas, paint, rect, text, len);
        }
        else
        {
            DrawLinesText(canvas, paint, rect, text, len);
        }
    }
    else
    {
        if ((fmt & (int)SkTextOp::TextFormat::tEllipsisText) != 0)
        {
            DrawEllipsisText(canvas, paint, rect, text, len, fmt);
        }
        else if ((fmt & SkTextOp::TextFormat::tPathEllipsisText) != 0)
        {
            DrawPathEllipsisText(canvas, paint, rect, text, len, fmt);
        }
        else
        {
            DrawSingleText(canvas, paint, rect, text, len);
        }
    }
}

//====================================================================

SkScalar SkTextOp::DrawEllipsisText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len, int fmt)
{
	SkPaint::FontMetrics fm;
	paint.getFontMetrics(&fm);
    SkScalar y = rect.top();
    SkScalar lineSpace = CalcLineSpace(fm);

    if ((fmt & SkTextOp::TextFormat::tTextVCenter) != 0)
    {
        y += (rect.height() - lineSpace) / 2.0f;
    }

	return InnerDrawEllipsisText(canvas, paint, rect.left(), CoereYCoor(y, fm), rect.width(), text, len);
}

SkScalar SkTextOp::InnerDrawEllipsisText(SkCanvas* canvas, SkPaint& paint, SkScalar x, SkScalar y, SkScalar w, const wchar_t* text, int len)
{
	SkScalar iDrawSize = 0;
	const wchar_t* dotText = L"...";
	SkScalar dotSize = paint.measureText(dotText, 6);
	SkScalar realWid = w - dotSize;

	len = len * 2;

	if (realWid > 0)
	{
		SkScalar realMeasure = 0;
		int iTextPos = paint.breakText(text, len, w, &realMeasure);

		if (iTextPos % 2 != 0)
		{
			iTextPos -= 1;
		}

		//
		// 可视区域完全可以显示文本
		//
		if (iTextPos >= len)
		{
			iDrawSize = realMeasure;
			canvas->drawText(text, len, x, y, paint);
		}
		else if (realMeasure <= dotSize)
		{
			iDrawSize = DoDotTextOnly(canvas, paint, dotText, x, y, w, dotSize);
		}
		//
		// 文本实际长度大于给出的可视宽度
		//
		else
		{
			const int CharNum = 3;
			int i = 0;
			// 计算省略号所在位置
			SkScalar charWid[CharNum] = {0};
			SkScalar charSum = w - realMeasure;
			int charPos = iTextPos / 2.0f - CharNum;
			paint.getTextWidths(text + charPos, CharNum * 2, charWid);

			for (i = 1; i <= CharNum; ++i)
			{
				charSum += charWid[CharNum - i];
				if ((int)charSum >= dotSize)
				{
					break;
				}
			}

			iDrawSize = w - charSum;

			// 加上省略号
			canvas->drawText(text, iTextPos - i * 2, x, y, paint);
			canvas->drawText(dotText, 6, x + iDrawSize, y, paint);

			iDrawSize += dotSize;
		}
	}
	else
	{
		SkScalar txtSize = paint.measureText(text, len);
		if (txtSize <= w)
		{
			canvas->drawText(text, len, x, y, paint);
			iDrawSize = txtSize;
		}
		else
		{
			iDrawSize = DoDotTextOnly(canvas, paint, dotText, x, y, w, dotSize);
		}
	}

	return iDrawSize;
}

SkScalar SkTextOp::DrawPathEllipsisText(SkCanvas* canvas, SkPaint& paint, const SkRect& rect, const wchar_t* text, int len, int fmt)
{
	SkPaint::FontMetrics fm;
	paint.getFontMetrics(&fm);

	SkScalar iDrawSize = 0;
	const wchar_t* dotText = L"...";
	SkScalar dotSize = paint.measureText(dotText, 6);
    SkScalar lineSpace = CalcLineSpace(fm);

	SkScalar realMeasure = 0;
    SkScalar y = rect.top();
    SkScalar w = rect.width();

	realMeasure = paint.measureText(text, len * 2);

    if ((fmt & (int)SkTextOp::TextFormat::tTextVCenter) != 0)
    {
        y += (rect.height() - lineSpace) / 2.0f;
    }

    y += CoereYCoor(0, fm);
	//y += /*fm.fDescent*/0 - fm.fAscent;

	//
	// 文本实际长度小于给出的可视宽度
	// 
	if ((int)realMeasure <= w)
	{
		canvas->drawText(text, len * 2, rect.left(), y, paint);
	}
	else
	{
		int i = 0;
		int iLastCount = 0;
		int iLastSize = 0;
		int iFirstPos = -1;

		for (; i < len; ++i)
		{
			if (text[i] == _T('/') || text[i] == _T('\\'))
			{
				iFirstPos = i;
				break;
			}
		}

		// 不是路径格式字符串
		if (iFirstPos == -1)
		{
			return InnerDrawEllipsisText(canvas, paint, rect.left(), y, w, text, len);
		}

		for (i = len - 1; i >= 0; --i)
		{
			if (text[i] == _T('/') || text[i] == _T('\\'))
			{
				iLastCount = len - i;
				if (iLastCount > 0)
				{
					iLastSize = (int)paint.measureText(text + i + 1, iLastCount * 2);
				}
				--i;
				break;
			}
		}

		int iOldPos = i;
		int curSize = (int)realMeasure;

		for (; i >= 0; --i)
		{
			if (text[i] == _T('/') || text[i] == _T('\\'))
			{
				if (iFirstPos == i)
				{
					i = 0;
				}

				int iStart = i;
				int iSize = iOldPos - i;
				int iMeasureSize = 0;

				iMeasureSize = (int)paint.measureText(text + iStart, iSize * 2);

				if (curSize - iMeasureSize < w)
				{
					SkScalar endPos = InnerDrawEllipsisText(canvas, paint, rect.left(), y, w - iLastSize, text, iOldPos + 1);
					if (iLastCount > 0)
					{
						canvas->drawText(text + len - iLastCount, iLastCount * 2, endPos, y, paint);
					}
					return endPos + iLastSize;
				}

				curSize -= iMeasureSize;

				--i;
				iOldPos = i;
			}
		}

		if (w >= dotSize)
		{
			canvas->drawText(dotText, 6, rect.left(), y, paint);
			const wchar_t* strFile = text + len - iLastCount;
			int iTextPos = paint.breakText(strFile, iLastCount * 2, w - dotSize, &realMeasure);
			if (iTextPos > 0)
			{
				canvas->drawText(strFile, iTextPos, rect.left() + dotSize, y, paint);
			}

			realMeasure += dotSize;
		}
		else
		{
			realMeasure = DoDotTextOnly(canvas, paint, dotText, rect.left(), y, w, dotSize);
		}
	}

	return realMeasure;
}

SkScalar SkTextOp::DoDotTextOnly(SkCanvas* canvas, SkPaint& paint, const wchar_t* dotText, SkScalar x, SkScalar y, SkScalar w, SkScalar dotSize)
{
	SkScalar oneDot = dotSize / 3.0f;
	SkScalar fPos = oneDot * 2.0f;

	if (w >= fPos)
	{
		canvas->drawText(dotText, 4, x, y, paint);
	}
	else if (w >= oneDot)
	{
		fPos = oneDot;
		canvas->drawText(dotText, 2, x, y, paint);
	}
	else
	{
		fPos = 0;
	}

	return fPos;
}

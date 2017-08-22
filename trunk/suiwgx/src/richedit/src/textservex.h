//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft shared
// source or premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the SOURCE.RTF on your install media or the root of your tools installation.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
/*	@doc EXTERNAL
 *
 *	@module TEXTSRV.H  Text Service Interface |
 *	
 *	Define interfaces between the Text Services component and the host
 *
 *	Original Author: <nl>
 *		Christian Fortini
 *
 *	History: <nl>
 *		8/1/95	ricksa	Revised interface definition
 */

#ifndef _TEXTSERVEX_H
#define _TEXTSERVEX_H

#include "textserv.h"

class ITextServicesEx : public ITextServices
{
public:

    virtual HRESULT TxRecalcView(BOOL bForce) = 0;
    virtual HRESULT TxFindOleObject(DWORD cp, REOBJECT* reobj) = 0;
    virtual HRESULT TxFindFirstVisibleOleObject() = 0;

    virtual HRESULT TxGetPointFromCp(DWORD cp, BOOL leftTop, POINT* point) = 0;
    virtual HRESULT TxSetLineLeftOffset(DWORD index, int xLeft) = 0;
    virtual HRESULT TxGetLineDetail(DWORD index, int& xCount, int& iInPara, int& xWhite, int& xLeft, int& xWidth, int& xHeight, int* iYPos) = 0;
};

class ITextHostEx : public ITextHost2
{
public:

    virtual ~ITextHostEx() {}

    virtual HRESULT TxInsertOleObject(LPDATAOBJECT pdo, STGMEDIUM* pStdMedium, FORMATETC* fetc, REOBJECT* reobj) = 0;
    virtual HRESULT TxObjectWrapping(BOOL& val) = 0;
};

//+-----------------------------------------------------------------------
// 	Factories
//------------------------------------------------------------------------

// Text Services factory
STDAPI CreateTextServices(
                          IUnknown *punkOuter,
                          ITextHostEx *pITextHost, 
                          IUnknown **ppUnk);

typedef HRESULT (STDAPICALLTYPE * PCreateTextServices)(
    IUnknown *punkOuter,
    ITextHostEx *pITextHost, 
    IUnknown **ppUnk);

#endif // _TEXTSERVEX_H


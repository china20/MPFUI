// ======================================================================
//
// Copyright (c) 2008-2012 汪荣, Inc. All rights reserved.
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

//////////////////////////////////////////////////////////////////////////////
// TextEditor.cpp

#include <Extend/Richedit/TextEditor.h>
#include "TextContainer.h"

namespace suic
{

TextEditor::TextEditor()
{
    _textContainer = new TextContainer();
    _textContainer->ref();
}

TextEditor::~TextEditor()
{
    _textContainer->unref();
}

}

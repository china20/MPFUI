#ifndef _UITEXTDOC_H_
#define _UITEXTDOC_H_

#include <Richedit.h>
#include <RichOle.h>
#include <textserv.h>
#include <TOM.h>

#include <System/Windows/Object.h>
#include <Extend/Editor/RichTextDef.h>

namespace suic
{

class TextDoc : public ITextDoc
{
public:

    TextDoc(ITextDocument* pDoc)
    {
    }

    virtual ~TextDoc()
    {
    }

    ITextDocument* GetTextDoc() const
    {
        return _textDoc;
    }

private:

    ITextDocument* _textDoc;
};

}

#endif

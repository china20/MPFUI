// 华勤科技版权所有 2010-2011
// 
// 文件名：TextSimple.h
// 功  能：实现标准的win32编辑控件。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITEXTSIMPLE_H_
#define _UITEXTSIMPLE_H_

#include <System/Tools/Array.h>
#include <System/Windows/Effect.h>
#include <System/Windows/TextInput.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API TextDecoration : public suic::DpObject
{
public:

    static suic::DpProperty* PenProperty;
    static suic::DpProperty* LocationProperty;
    static suic::DpProperty* PenOffsetProperty;

    static bool StaticInit();

    RTTIOfClass(TextDecoration)

    TextDecoration();
    TextDecoration(TextDecorationLocation textLoc);

    Brush* GetPen();
    void SetPen(Brush* val);

    TextDecorationLocation GetLocation();
    void SetLocation(TextDecorationLocation val);

    Float GetPenOffset();
    void SetPenOffset(Float val);
};

class SUICORE_API TextDecorationCollection : public suic::DpObject
                                           , public suic::IAddChild
{
public:

    static TextDecorationCollection* s_empty;

    static TextDecorationCollection* GetEmpty();

    TextDecorationCollection();
    ~TextDecorationCollection();

    RTTIOfClass(TextDecorationCollection);

    int GetCount() const;
    TextDecoration* GetTextDecoration(int index) const;

    void AddTextDecoration(TextDecoration* val);
    void RemoveChild(Object* child);
    void Remove(TextDecoration* val);

    void Clear();

    virtual void AddChild(Object* obj);
    virtual void AddText(String val);

private:

    suic::Array<TextDecoration*> _textDecorations;
};

class SUICORE_API TextDecorations
{
public:

    static TextDecorationCollection* none;
    static TextDecorationCollection* baseline;
    static TextDecorationCollection* overline;
    static TextDecorationCollection* strikethrough;
    static TextDecorationCollection* underline;
};

class SUICORE_API TextEffectCollection : public suic::DpObject
                                       , public suic::IAddChild
{
public:

    static TextEffectCollection* s_empty;

    static TextEffectCollection* GetEmpty();

    TextEffectCollection();
    ~TextEffectCollection();

    RTTIOfClass(TextEffectCollection);

    void Add(TextEffect* val);
    void RemoveChild(Object* child);
    void Remove(TextEffect* val);
    void Clear();

    virtual void AddChild(Object* obj);
    virtual void AddText(String val);

    int GetCount();
    TextEffect* GetTextEffect(int index);

private:

    Array<TextEffect*> _effects;
};

class SUICORE_API TextElement : public FrameworkElement
{
public:

    static DpProperty* ForegroundProperty;
    static DpProperty* BackgroundProperty;

    static DpProperty* FontFamilyProperty;
    static DpProperty* FontSizeProperty;
    static DpProperty* FontWeightProperty;
    static DpProperty* FontStyleProperty;
    static DpProperty* FontStretchProperty;

    static DpProperty* IsSingleLineProperty;
    static DpProperty* TextEffectsProperty;

    static bool StaticInit();

    TextElement();
    ~TextElement();

    RTTIOfAbstractClass(TextElement)

    String GetFontFamily();
    void SetFontFamily(String val);

    int GetFontSize();
    void SetFontSize(int val);

    int GetFontWeight();
    void SetFontWeight(int val);

    int GetFontStyle();
    void SetFontStyle(int val);

    int GetFontStretch();
    void SetFontStretch(int val);

    Brush* GetForeground();
    void SetForeground(Brush* val);

    Brush* GetBackground();
    void SetBackground(Brush* val);

    suic::TextEffectCollection* GetTextEffects();
    void SetTextEffects(suic::TextEffectCollection* val);

public:

    virtual String GetText() = 0;
    virtual void SetText(String text) = 0;

    virtual bool GetSingleLine();
    virtual void SetSingleLine(bool val);

protected:

    Float GetRealFontSize();
};

}

#endif

// 华勤科技版权所有 2008-2022
// 
// 文件名：BindParser.h
// 功  能：解析Binding标签辅助类。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIBINDPARSER_H_
#define _UIBINDPARSER_H_

#include <System/Windows/Binding.h>

namespace suic
{

class BindParser
{
public:

    BindParser();

    Binding* Parse(const String& val);

private:

    int FindCommaPos(const String& str, int iStart);
    int ParseMode(const String& str, int iStart);
    int ParseUpdateSourceTrigger(const String& str, int iStart);
    int ParsePath(const String& str, int iStart);
    int ParseConverter(const String& str, int iStart);
    int ParseSource(const String& str, int iStart);
    int ParseElementName(const String& str, int iStart);
    int ParseConverterParam(const String& str, int iStart);
    int ParseRelativeSource(const String& str, int iStart);
    bool ParseBinding(String str);

    int ParseSingle(const String& str, int iStart, String& val);

private:

    Binding* _binding;
};

}

#endif
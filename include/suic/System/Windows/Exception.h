// 华勤科技版权所有 2010-2011
// 
// 文件名：uiexception.h
// 功  能：定义核心库的异常类信息。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEXCEPTION_H_
#define _UIEXCEPTION_H_

#include <System/Types/Const.h>
#include <System/Windows/Object.h>

#define __FILELINE__ __FILE__, __LINE__

namespace suic 
{

class SUICORE_API Exception
{
public:

    Exception();
    Exception(String errorMsg, char* f, int l);
    Exception(String tag, String errorMsg, char* f, int l);

    String GetErrorMsg() const;
    String GetLineInfo() const;

private:
    
    String _errorMsg;
    String _lineInfo;
};

class SUICORE_API InvalidOperationException : public Exception
{
public:

    InvalidOperationException(String info, char* f, int l);
    InvalidOperationException(String oper, String info, char* f, int l);
};

class SUICORE_API ArgumentException : public Exception
{
public:

    ArgumentException(String arg, char* f, int l);
    ArgumentException(String arg, String info, char* f, int l);
};

class SUICORE_API ArgumentNullException : public Exception
{
public:

    ArgumentNullException(String arg, char* f, int l);
    ArgumentNullException(String arg, String info, char* f, int l);
};

class SUICORE_API InvalidValueException : public Exception
{
public:

    InvalidValueException(String err, char* f, int l);
};

}

#endif

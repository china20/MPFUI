// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uilog.h
// ��  �ܣ�������־��¼��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-01-02
// 
// ============================================================================

#ifndef _UILOG_H_
#define _UILOG_H_

#include <System/Windows/Object.h>
#include <stdio.h>

namespace suic
{

class SUICORE_API Log
{
public:

    enum {eNone, eError, eWarn, eInfo, eTrace};

    Log();
    ~Log();

    void Print(const String& info);
    void Trace(const String title, const String& info);
    void Warn(const String& info);
    void Info(const String& info);
    void Error(const String& info);

    bool IsValid();
    bool Init(const String& path, int level);
    void Close();

    bool Open();

protected:

    int _level;
    FILE* _file;
    String _path;
};

}

#endif

// 华勤科技版权所有 2010-2022
// 
// 文件名：ProcessInfo.h
// 功  能：进程列表信息。
// 
// 作  者：MPF开发组
// 时  间：2017-09-02
// 
// ============================================================================

#ifndef _PROCESSINFO_H_
#define _PROCESSINFO_H_

class ProcessItem : public suic::NotifyPropChanged
{
public:

    BeginMember(ProcessItem, suic::NotifyPropChanged)
        MemberString(Name)
        MemberString(UserName)
        MemberInt(ProcessID)
    EndMember()

    RTTIOfClass(ProcessItem)

    DefineString(Name);
    DefineString(UserName);
    DefineInt(ProcessID);
};

class ProcessInfo
{
public:

    ProcessInfo();
    ~ProcessInfo();

    void RefleshProcessInfo();

    suic::ObservableCollection* GetProcessColl()
    {
        return _processColl;
    }

private:

    suic::ObservableCollection *_processColl;
};

#endif

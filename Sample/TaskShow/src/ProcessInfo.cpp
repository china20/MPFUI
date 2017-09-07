/////////////////////////////////////////////////////
// ProcessInfo.cpp

#include "stdafx.h"
#include "ProcessInfo.h"

#include <Tlhelp32.h>

ImplementRTTIOfClass(ProcessItem, suic::NotifyPropChanged)

ProcessInfo::ProcessInfo()
{
    _processColl = new suic::ObservableCollection();
    _processColl->ref();
}

ProcessInfo::~ProcessInfo()
{
    _processColl->unref();
}

void ProcessInfo::RefleshProcessInfo()
{
    _processColl->Clear();

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    BOOL bRet;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
    bRet = Process32First(hProcessSnap,&pe32);

    while (bRet)
    {
        bRet = Process32Next(hProcessSnap,&pe32);
        ProcessItem* pItem = new ProcessItem();
        _processColl->AddItem(pItem);

        pItem->SetName(pe32.szExeFile);
        pItem->SetProcessID(pe32.th32ProcessID);
    }
}

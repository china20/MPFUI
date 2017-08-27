// 华勤科技版权所有 2010-2011
// 
// 文件名：uidframe.h
// 功  能：设计器框架导出接口定义。
// 
// 作  者：汪荣
// 时  间：2012-07-02
// 
// ============================================================================

#ifndef _UIDFRAME_H_
#define _UIDFRAME_H_

#include <System/Windows/Element.h>

struct CreateVSInfo
{
    bool needOpenSln;
    bool needAddSln;
    bool needOpenDir;

    suic::String createVs;
    suic::String targetVs;
    suic::String prjPath;
    suic::String slnPath;
};

class IUIDFrame : public suic::Object
{
public:

    enum UIDMode { umNormal, umAddin, };

    virtual ~IUIDFrame() {}

    virtual bool LoadRes(const suic::String& path, const suic::String& pwd) = 0;
    virtual bool LoadProject(const suic::String& strDir, const suic::String& strTheme, bool bNoExistCreate) = 0;

    virtual bool RunFrame() = 0;
    
    virtual bool UpdateWindow(const suic::String& strUri) = 0;
    virtual bool StartWindow(const suic::String& strUri) = 0;
    virtual void RemoveWindow(const suic::String& strUri) = 0;

    virtual void SetUIDMode(IUIDFrame::UIDMode mode) = 0;

    virtual void ShowCreateVS(CreateVSInfo& info) = 0;

    virtual void Dispose() = 0;
};

typedef suic::shared<IUIDFrame> UIDFramePtr;

extern "C" void UIDGetFrame(UIDFramePtr& frame);

#endif

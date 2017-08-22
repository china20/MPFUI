// 华勤科技版权所有 2010-2011
// 
// 文件名：EncryResource.h
// 功  能：加密资源文件。
// 
// 作  者：汪荣
// 时  间：2012-02-02
// 
// ============================================================================

#ifndef _UIENCRYRESOURCE_H_
#define _UIENCRYRESOURCE_H_

#include "Encrypt.h"
#include <main/Project.h>

class EncryResource : public suic::Window
{
public:

    EncryResource(Project* pPrj);
    ~EncryResource();

    void SetProject(Project* pPrj)
    {
        _project = pPrj;
    }

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void OnOpenFile(suic::Element* pItem, suic::RoutedEventArg* e);
    void OnEncry(suic::Element* pItem, suic::RoutedEventArg* e);
    void OnCancel(suic::Element* pItem, suic::RoutedEventArg* e);

    void OnEnter(suic::Element* sender, suic::KeyboardEventArg* e);

private:

    Project* _project;
};


#endif

/////////////////////////////////////////////////////
// DesignWindow.cpp

#include "stdafx.h"
#include "DesignWindow.h"

DesignWindow::DesignWindow()
{
}

DesignWindow::~DesignWindow()
{
}

suic::ObjectPtr DesignWindow::OnBuild(suic::IXamlNode* pNode)
{
    if (pNode->FindAttri(_U("Name")).Equals(_U("PropScrollPanel")))
    {
        return NULL;
    }
    else
    {
        return NULL;
    }
}

void DesignWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void DesignWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void DesignWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
}

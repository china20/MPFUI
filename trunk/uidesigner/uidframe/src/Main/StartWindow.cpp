/////////////////////////////////////////////////////
// StartWindow.cpp

#include "stdafx.h"
#include "StartWindow.h"
#include <Core/ResNode.h>
#include <Tools/Utils.h>

StartWindow::StartWindow()
{
    _bCanContinue = false;
    _recentlyPrjs.setAutoDelete(false);
}

StartWindow::~StartWindow()
{
}

void StartWindow::OnOpenProjectDir(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::DirBrowser fb;
    if (fb.Open(this))
    {
        _newPrjPath = fb.GetDir();
        suic::TextBox* txtBox = FindElem<suic::TextBox>(_U("PrjPath"));
        if (NULL != txtBox)
        {
            txtBox->SetText(_newPrjPath);
        }
        //AsyncClose();
    }
}

void StartWindow::OnNewProject(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::TextBox* titleBox = FindElem<suic::TextBox>(_U("PrjName"));
    suic::TextBox* txtBox = FindElem<suic::TextBox>(_U("PrjPath"));

    if (NULL != txtBox && NULL != titleBox)
    {
        _prjName = titleBox->GetText();
        _newPrjPath = txtBox->GetText();
    }

    if (!_prjName.Empty() && !_newPrjPath.Empty())
    {
        _bCanContinue = true;
        _openPrjPath = _U("");
        AsyncClose();
    }
    else
    {
        suic::InfoBox::Show(_T("请输入项目名称或路径!"), _T("提示"));
    }
}

void StartWindow::OnOpenProject(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    suic::FileBrowser fb;
    if (fb.Open(this, _U("All Files(*.uiproj)\0*.uiproj\0\0")))
    {
        _openPrjPath = fb.GetFilePath();
        _bCanContinue = true;
        AsyncClose();
    }
}

void StartWindow::OnCloseStartWindow(suic::Element* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);
    AsyncClose();
}

void StartWindow::OnDbclkOnItem(suic::Element* sender, suic::MouseButtonEventArg* e)
{
    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    Element* elem = RTTICast<Element>(e->GetOriginalSource());
    RecentlyPrj* item = DynamicCast<RecentlyPrj>(elem->GetDataContext());
    if (NULL != item)
    {
        e->SetHandled(true);
        _bCanContinue = true;
        _openPrjPath = item->path;
        AsyncClose();
    }
}

bool StartWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void StartWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void StartWindow::AddRecentlyProject(const String& name, const String& path)
{
    if (!name.Empty() && !path.Empty())
    {
        for (int i = 0; i < _recentlyPrjs.GetCount(); ++i)
        {
            RecentlyPrj* item = DynamicCast<RecentlyPrj>(_recentlyPrjs.GetItem(i));
            String name1 = item->name;
            String name2 = name;

            if (name1.CompareI(name2) == 0)
            {
                return;
            }
        }
        _recentlyPrjs.AddItem(new RecentlyPrj(name, path));
    }
}

void StartWindow::ReadRecentlyProjects()
{
    XmlDoc xmlDoc;
    XmlNode* root = NULL;

    try
    {
        suic::Char appPath[512];
        ::GetModuleFileName(NULL, appPath, 511);
        suic::Mulstr strPath(appPath);

        strPath += ".xml";
        suic::Mulstr buff;

        Utils::ReadResFromFile(Mulstr(strPath.c_str()).c_str(), "rb", buff);
        buff = buff.FromUtf8(buff.c_str(), buff.Length());

        xmlDoc.parse<0>(buff.c_str());
        root = xmlDoc.first_node();

        if (NULL == root)
        {
            return;
        }

        XmlNode* childNode = root->first_node();
        while (childNode != NULL)
        {
            String strName(childNode->name());
            if (strName.Equals(_U("Recently")))
            {
                XmlNode* recNode = childNode->first_node();
                while (recNode != NULL)
                {
                    String name;
                    String path;

                    XmlAttr* attr = recNode->first_attribute("Name");

                    if (NULL != attr)
                    {
                        name = attr->value();
                    }

                    attr = recNode->first_attribute("Path");

                    if (NULL != attr)
                    {
                        path = suic::FileDir::CalculatePath(attr->value());
                    }

                    if (!name.Empty() && !path.Empty() && FileDir::GetFileProp(path) == FileDir::fpFile)
                    {
                        RecentlyPrj* item = new RecentlyPrj(name, path);
                        _recentlyPrjs.AddItem(item);
                    }

                    recNode = recNode->next_sibling();
                }
            }

            childNode = childNode->next_sibling();
        }
    }
    catch (char* err)
    {
        ;
    }
    catch (...)
    {
    }
}

void StartWindow::SaveRecentlyProjects()
{
    suic::String strXml;
    suic::Char appPath[512];
    ::GetModuleFileName(NULL, appPath, 511);
    suic::String strPath(appPath);

    strPath += _U(".xml");

    strXml += _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    strXml += _U("<UIConfig>\n");

    strXml += ResNode::OFFSET1 + _U("<Recently>\n");

    for (int i = 0; i < _recentlyPrjs.GetCount(); ++i)
    {
        RecentlyPrj* item = DynamicCast<RecentlyPrj>(_recentlyPrjs.GetItem(i));

        strXml += ResNode::OFFSET2 + _U("<Item Name=\"");
        strXml += item->name;
        strXml += _U("\" Path=\"");
        strXml += item->path;
        strXml += _U("\"/>\n");
    }
    strXml += ResNode::OFFSET1 + _U("</Recently>\n");

    strXml += _U("</UIConfig>\n");

    FileWriter fs;

    if (fs.Open(strPath))
    {
        fs.Write(strXml);
        fs.Close();
    }
}

void StartWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);
}

void StartWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    ReadRecentlyProjects();

    ListBox* recList = FindElem<suic::ListBox>(_U("RecentlyPrjs"));

    if (NULL != recList)
    {
        recList->SetItemsSource(&_recentlyPrjs);
        recList->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &StartWindow::OnDbclkOnItem));
    }

    suic::Button* pBtn = FindElem<suic::Button>(_U("OpenDir"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &StartWindow::OnOpenProjectDir));
    }

    pBtn = FindElem<suic::Button>(_U("NewPrjOk"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &StartWindow::OnNewProject));
    }

    pBtn = FindElem<suic::Button>(_U("OpenProject"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &StartWindow::OnOpenProject));
    }

    pBtn = FindElem<suic::Button>(_U("CloseBtn"));
    if (NULL != pBtn)
    {
        pBtn->AddClick(new suic::ClickEventHandler(this, &StartWindow::OnCloseStartWindow));
    }
}

// 华勤科技版权所有 2010-2022
// 
// 文件名：MainWindow.h
// 功  能：主界面窗口。
// 
// 作  者：MPF开发组
// 时  间：2017-09-02
// 
// ============================================================================

#ifndef _DRIVERBOX_H_
#define _DRIVERBOX_H_

class DriverBox : public suic::TreeView
{
public:

    DriverBox();
    virtual ~DriverBox();

    void LoadDriver();
};

#endif

///
/// 文件名：IRedoUndo.h
///
/// 功  能：管理和UI对应的属性
///
/// 作  者：汪荣
///

#ifndef _IREDOUNDO_H_
#define _IREDOUNDO_H_

class IRedoUndo
{
public:

    virtual ~IRedoUndo() {}

    virtual bool Redo() = 0;
    virtual void Undo() = 0;
};

#endif


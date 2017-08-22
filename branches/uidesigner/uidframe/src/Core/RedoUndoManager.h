///
/// 文件名：IRedoUndo.h
///
/// 功  能：管理和UI对应的属性
///
/// 作  者：汪荣
///

#ifndef _REDOUNDOMANAGER_H_
#define _REDOUNDOMANAGER_H_

#include <Core/IRedoUndo.h>

class RedoUndoManager
{
public:

    RedoUndoManager();
    ~RedoUndoManager();

    void AddRedoUndo(IRedoUndo* pRedoUndo);
    void Redo();
    void Undo();

    void Clear();

private:

    void DoMaxRedoUndos();

private:

    int _maxRedoUndos;
    // 记录操作
    suic::Array<IRedoUndo*> _redos;
    suic::Array<IRedoUndo*> _undos;
};

#endif

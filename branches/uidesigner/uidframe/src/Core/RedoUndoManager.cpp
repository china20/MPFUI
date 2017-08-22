///
/// 文件名：IRedoUndo.h
///
/// 功  能：管理和UI对应的属性
///
/// 作  者：汪荣
///

#include "stdafx.h"

#include <Core/RedoUndoManager.h>

RedoUndoManager::RedoUndoManager()
{
    _maxRedoUndos = 100;

    _redos.Resize(_maxRedoUndos);
    _undos.Resize(_maxRedoUndos);

    int i = 0;

    for (i = 0; i < _redos.GetCount(); ++i)
    {
        _redos[i] = NULL;
    }

    for (i = 0; i < _undos.GetCount(); ++i)
    {
        _undos[i] = NULL;
    }
}

RedoUndoManager::~RedoUndoManager()
{
    Clear();
}

void RedoUndoManager::Clear()
{
    int i = 0;

    for (i = 0; i < _redos.GetCount(); ++i)
    {
        if (NULL != _redos[i])
        {
            delete _redos[i];
        }
    }

    _redos.Clear();

    for (i = 0; i < _undos.GetCount(); ++i)
    {
        if (NULL != _undos[i])
        {
            delete _undos[i];
        }
    }

    _undos.Clear();
}

void RedoUndoManager::AddRedoUndo(IRedoUndo* pRedoUndo)
{
    DoMaxRedoUndos();
    _undos.Add(pRedoUndo);
}

void RedoUndoManager::DoMaxRedoUndos()
{
    if (_undos.GetCount() >= _maxRedoUndos)
    {
        if (_undos[0] != NULL)
        {
            delete _undos[0];
        }
        _undos.RemoveAt(0);
    }
}

void RedoUndoManager::Redo()
{
    if (_redos.GetCount() > 0)
    {
        int index = _redos.GetCount() - 1;
        IRedoUndo* pRedoUndo = _redos[index];

        _redos.RemoveAt(index);
        _undos.Add(pRedoUndo);

        pRedoUndo->Redo();
    }
}

void RedoUndoManager::Undo()
{
    if (_undos.GetCount() > 0)
    {
        int index = _undos.GetCount() - 1;
        IRedoUndo* pRedoUndo = _undos[index];

        _undos.RemoveAt(index);
        _redos.Add(pRedoUndo);

        pRedoUndo->Undo();
    }
}

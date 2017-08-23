//Created by Wang.xp
//Created Time :2015/09/17
#ifndef _FrameworkCoreSimpleDataGrid_H_
#define _FrameworkCoreSimpleDataGrid_H_

#include "BindingItem.h"
#include "Framework\Controls\GridView.h"

namespace FrameworkCore
{

class SimpleDataGridRow : public FrameworkCore::PropertyDescriptors
{
public:

	SimpleDataGridRow();
	~SimpleDataGridRow();

	void ClearVals();
};

class MyThread : public suic::Thread
{
public:

    MyThread(suic::InvokeProxy* h)
    {
        _handler = h;
    }

    void Run()
    {
        for (;;)
        {
            if (IsStopped()) 
            {
                break;
            }
            _handler->PostInvoker(0, NULL);
            Sleep(10);
        }
    }

protected:

    suic::InvokeProxy* _handler;
};

//SimpleDataGrid
class SimpleDataGrid : public suic::ListView
{
public:

	SimpleDataGrid();
	~SimpleDataGrid();
	//RTTIOfClass(SimpleDataGrid);
	static bool StaticInit();

	void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);	

public:

	void AddColumn(suic::String column);
	void RemoveColumn(suic::String column);
	void RemoveColumn(int index);
	void ClearColumns();
	bool FindColumnByName(suic::String name,int &index);

	void AddRow(SimpleDataGridRow* row);
	void RemoveRow(SimpleDataGridRow* row);
	void RemoveRow(int index);
	void ClearRows();
	bool FindRowByFlag(int flag,int &index);

	void AddInvoker(DelegateFunC f);

protected:

	void OnColumnCountChanged();
	void OnRowCountChanged();
	
	void OnInvoker(suic::Object* sender, suic::InvokerArg* e);

protected:

	suic::StringArray _columns;
	suic::AutoColl<SimpleDataGridRow> _rows;
    suic::Thread* _thread;
	suic::InvokeProxy* _threadHandle;
	//DelegateFunC带一个参数的委托 typedef delegate<void(suic::Object*)> DelegateFunC;
	DelegateFunC _invokeFunC;
};


typedef suic::shared<SimpleDataGrid> SimpleDataGridPtr;

}

#endif

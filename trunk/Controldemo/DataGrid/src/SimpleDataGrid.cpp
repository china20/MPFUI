//Created by Wang.xp
//Created Time :2015/09/17

#include "stdafx.h"
#include "SimpleDataGrid.h"
#include <System\Windows\Element.h>

namespace FrameworkCore
{
/************************************************************************/
/*                                                                      */
/************************************************************************/
SimpleDataGridRow::SimpleDataGridRow()
	:FrameworkCore::PropertyDescriptors()
{

}

SimpleDataGridRow::~SimpleDataGridRow()
{
	_propertys.Clear();
}

void SimpleDataGridRow::ClearVals()
{
	_propertys.Clear();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
//ImplementRTTIOfClass(SimpleDataGrid,suic::ListView);
SimpleDataGrid::SimpleDataGrid()
	:_thread(NULL)
	,_threadHandle(NULL)
{	
	_columns.Clear();
	_rows.Clear();
}

bool SimpleDataGrid::StaticInit()
{
	//OrientationProperty=suic::DpProperty::Register("Orientation",SimpleDataGrid::RTTIType(),suic::Integer::RTTIType(),suic::DpPropMemory::GetPropMeta(suic::Integer::GetPosInt(suic::Orientation::Horizontal),suic::PropMetadataOptions::AffectsArrange));
	//OrientationProperty->SetConvertValueCb(suic::OrientationConvert::Convert);

	return true;
}

SimpleDataGrid::~SimpleDataGrid()
{
}
void SimpleDataGrid::OnInitialized(suic::EventArg* e)
{
	__super::OnInitialized(e);		
}

void SimpleDataGrid::OnLoaded(suic::LoadedEventArg* e)
{
	__super::OnLoaded(e);	
	_threadHandle = new suic::InvokeProxy(this);
	_threadHandle->ref();
	_thread = new MyThread(_threadHandle);
	_thread->ref();
	_threadHandle->Invoker+=suic::InvokerHandler(this, &SimpleDataGrid::OnInvoker);	
	_thread->Start();
}

void SimpleDataGrid::OnInvoker(suic::Object* sender, suic::InvokerArg* e)
{
	MyThread* temThread = dynamic_cast<MyThread*>(e->GetData());
	if (_invokeFunC)
	{
		if (_invokeFunC(this))
        {
            ScrollToEnd();
        }
        else
        {
            //temThread->Stop();
        }
	}
}

void SimpleDataGrid::AddColumn(suic::String column)
{
	int _index=-1;
	if (!FindColumnByName(column, _index))
	{
		_columns.Add(column);
		OnColumnCountChanged();
	}
}

void SimpleDataGrid::RemoveColumn(suic::String column)
{
	int _index=-1;
	if (FindColumnByName(column,_index))
	{
		_columns.RemoveAt(_index);
		OnColumnCountChanged();
	}
}

void SimpleDataGrid::RemoveColumn(int index)
{
	_columns.RemoveAt(index);
	OnColumnCountChanged();
}
void SimpleDataGrid::ClearColumns()
{
	if (_columns.GetCount() > 0)
	{
		_columns.Clear();
		OnColumnCountChanged();
	}
}

void SimpleDataGrid::OnColumnCountChanged()
{
	if (_columns.GetCount() > 0)
	{
		suic::GridView* _detailView=NULL;
		if (this->GetView() != NULL)
        {
            _detailView = suic::DynamicCast<suic::GridView>(this->GetView());
        }

		if (_detailView)
		{
			_detailView->GetColumns()->Clear();
		}
		else
		{
			_detailView=new suic::GridView();
			this->SetView(_detailView);
		}

		for(int i = 0;i < _columns.GetCount(); i++)
		{
			suic::String _colName = _columns.GetItem(i);
			suic::GridViewColumn* _col=new suic::GridViewColumn();
			_col->SetWidth(100);
			_col->SetHeader(new suic::OString(_colName));
			suic::Binding* _binding=new suic::Binding(_colName);
			//suic::DataTemplate* _cellTemplate=DynamicCast<suic::DataTemplate>(suic::Application::Current()->FindRes("ItemWidthIconDataTemplate"));
			_col->SetDisplayMember(_binding);
			_detailView->AddChild(_col);
		}
	}
}
bool SimpleDataGrid::FindColumnByName(suic::String name,int &index)
{
	for (int i = 0;i < _columns.GetCount(); i++)
	{
		if (name.Equals(_columns.GetItem(i)))
		{
			index=i;
			return true;
		}
	}
	return false;
}

void SimpleDataGrid::AddRow(SimpleDataGridRow* row)
{
	int _index = -1;
	int flag = row->GetFlag();
	if (flag == -1)
    {
        flag=_rows.GetCount();
    }

	if (!FindRowByFlag(flag, _index))
	{
		_rows.AddItem(row);
		this->AddChild(row);
	}
}

void SimpleDataGrid::RemoveRow(SimpleDataGridRow* row)
{
	int _index = -1;
	int flag=row->GetFlag();
	if (flag != -1)
	{
		if (FindRowByFlag(flag,_index))
		{
			_rows.RemoveItemAt(_index);
			this->GetItemsSource()->RemoveItemAt(_index);
			OnRowCountChanged();
		}
	}
}

void SimpleDataGrid::RemoveRow(int index)
{
	_rows.RemoveItemAt(index);
	this->GetItemsSource()->RemoveItemAt(index);
	OnRowCountChanged();
}

void SimpleDataGrid::ClearRows()
{
	_rows.Clear();
	this->GetItemsSource()->Clear();
}

void SimpleDataGrid::OnRowCountChanged()
{
	for(int i = 0;i < _rows.GetCount(); i++)
	{
		_rows.GetItem(i)->SetFlag(i);
	}
}

bool SimpleDataGrid::FindRowByFlag(int flag,int &index)
{
	for (int i = 0;i < _rows.GetCount(); i++)
	{
		if (_rows.GetItem(i)->GetFlag() == flag)
		{
			index = i;
			return true;
		}
	}
	return false;
}

void SimpleDataGrid::AddInvoker(DelegateFunC f)
{
	_invokeFunC+=f;
}

}

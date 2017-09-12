//Created by Wang.xp
//Created Time :2015/09/17

#include <Extend\Controls\GridSplitter.h>
#include <Framework\Controls\Decorator.h>

#include <System\Windows\Element.h>
#include <System\Tools\EventHelper.h>
#include <System\Tools\VisualTreeOp.h>

namespace suic
{

ImplementRTTIOfClass(GridSplitter, suic::Thumb);

suic::DpProperty* GridSplitter::DragIncrementProperty;
suic::DpProperty* GridSplitter::PreviewStyleProperty;
suic::DpProperty* GridSplitter::KeyboardIncrementProperty;
suic::DpProperty* GridSplitter::ResizeBehaviorProperty;
suic::DpProperty* GridSplitter::ResizeDirectionProperty;
suic::DpProperty* GridSplitter::ShowsPreviewProperty;

GridSplitter::GridSplitter()
    : _resizeData(NULL)
    , _parentGrid(NULL)
{
    GridSplitter::StaticInit();
}

void GridSplitter::StaticInit()
{
    if (NULL == DragIncrementProperty)
    {
        Thumb::StaticInit();

        DragIncrementProperty = suic::DpProperty::Register("DragIncrement",GridSplitter::RTTIType(),suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(new suic::OFloat(1.0)));
        PreviewStyleProperty = suic::DpProperty::Register("PreviewStyle",GridSplitter::RTTIType(), suic::Style::RTTIType(),suic::DpPropMemory::GetPropMeta(NULL));
        KeyboardIncrementProperty = suic::DpProperty::Register("KeyboardIncrement",GridSplitter::RTTIType(), suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(new suic::OFloat(1.0)));

        ResizeBehaviorProperty = suic::DpProperty::Register("ResizeBehavior",GridSplitter::RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetPosInt(GridResizeBehavior::grbBasedOnAlignment)));
        //ResizeBehaviorProperty->SetConvertValueCb(ResizeBehaviorConvertor::Convert);

        ResizeDirectionProperty = suic::DpProperty::Register("ResizeDirection",GridSplitter::RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetPosInt(GridResizeDirection::grdAuto)));
        //ResizeDirectionProperty->SetConvertValueCb(FrameworkCore::GridResizeDirectionConvertor::Convert);

        ShowsPreviewProperty = suic::DpProperty::Register("ShowsPreview",GridSplitter::RTTIType(), suic::Boolean::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::False));
        //ShowsPreviewProperty->SetConvertValueCb(FrameworkCore::BooleanConvertor::Convert);

        //ClipToBoundsProperty->OverrideMetadata(RTTIType(), new PropMetadata(Boolean::False));
        // ×¢²áÊÂ¼þ
        suic::EventHelper::RegisterClassHandler(GridSplitter::RTTIType(), suic::Thumb::DragStartedEvent, new suic::RoutedEventHandler(&GridSplitter::OnDragStartedTrunk), false);
        suic::EventHelper::RegisterClassHandler(GridSplitter::RTTIType(), suic::Thumb::DragDeltaEvent,new suic::RoutedEventHandler(&GridSplitter::OnDragDeltaTrunk), false);
        suic::EventHelper::RegisterClassHandler(GridSplitter::RTTIType(), suic::Thumb::DragCompletedEvent,new suic::RoutedEventHandler(&GridSplitter::OnDragCompletedTrunk), false);
    }
}

PreviewAdorner::PreviewAdorner(GridSplitter* gridSplitter, Style* previewStyle) 
    : Adorner(gridSplitter)
{
    Control* control = new Control();

    control->ref();
    control->SetStyle(previewStyle);
    control->Enable(false);
    _decorator = new Decorator();
    _decorator->ref();
    _decorator->SetChild(control);
    AddVisualChild(_decorator);
    control->unref();

    SetVerticalAlignment(VertAlign::vaTop);
    SetHorizontalAlignment(HoriAlign::haLeft);
}

PreviewAdorner::~PreviewAdorner()
{
    _decorator->unref();
}

Float PreviewAdorner::GetOffsetX()
{
    return GetMargin().left;
}

void PreviewAdorner::SetOffsetX(Float val)
{
    Rect rect = GetMargin();
    rect.left = (int)val;
    SetMargin(rect);
}

Float PreviewAdorner::GetOffsetY()
{
    return GetMargin().top;
}

void PreviewAdorner::SetOffsetY(Float val)
{
    Rect rect = GetMargin();
    rect.top = (int)val;
    SetMargin(rect);
}

void PreviewAdorner::OnArrange(const suic::Size& finalSize)
{
    _decorator->Arrange(Rect(new Point(), finalSize));
}

Element* PreviewAdorner::GetVisualChild(int index)
{
    return _decorator;
}

int PreviewAdorner::GetVisualChildrenCount()
{
    return 1;
}

GridSplitter::~GridSplitter()
{
    if (NULL != _resizeData)
    {
        delete _resizeData;
    }
}

suic::Style* GridSplitter::GetPreviewStyle()
{
    return RTTICast<suic::Style>(GetValue(PreviewStyleProperty));
}

void GridSplitter::SetPreviewStyle(suic::Style* val)
{
    SetValue(PreviewStyleProperty, val);
}

void GridSplitter::OnDragStartedTrunk(suic::DpObject* sender,suic::RoutedEventArg* e)
{
	GridSplitter* gridSplitter = RTTICast<GridSplitter>(sender);
    if (NULL != gridSplitter)
    {
        gridSplitter->OnDragStarted(e);
    }
}

void GridSplitter::OnDragDeltaTrunk(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    GridSplitter* gridSplitter = RTTICast<GridSplitter>(sender);
    if (NULL != gridSplitter)
    {
        gridSplitter->OnDragDelta((suic::DragDeltaEventArg*)e);
    }
}

void GridSplitter::OnDragCompletedTrunk(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    GridSplitter* gridSplitter = RTTICast<GridSplitter>(sender);
    if (NULL != gridSplitter)
    {
        gridSplitter->OnDragCompleted((suic::DragCompletedEventArg*)e);
    }
}

void GridSplitter::SetDragIncrement(suic::Float val)
{
	SetValue(DragIncrementProperty, new suic::OFloat(val));
}

void GridSplitter::SetKeyboardIncrement(suic::Float val)
{
	SetValue(KeyboardIncrementProperty, new suic::OFloat(val));
}

void GridSplitter::SetResizeBehavior(GridResizeBehavior behavior)
{
	SetValue(ResizeBehaviorProperty, suic::Integer::GetPosInt(behavior));
}

void GridSplitter::SetResizeDirection(GridResizeDirection direction)
{	
	SetValue(ResizeDirectionProperty, suic::Integer::GetPosInt(direction));
}

void GridSplitter::SetShowsPreview(bool val)
{
	SetValue(ShowsPreviewProperty, suic::Boolean::FromBool(val));
}

void GridSplitter::OnDragStarted(suic::RoutedEventArg* e)
{
	InitializeData();
    UpdatePreviewPos(0, 0);
    e->SetHandled(true);
}

void GridSplitter::OnDragDelta(suic::DragDeltaEventArg* e)
{
    e->SetHandled(true);
	if (NULL != _resizeData && IsFirstTamplateParentGrid())
	{
        double horizontalChange = e->GetHorizontalChange();
        double verticalChange = e->GetVerticalChange();

		if (_resizeData->ShowsPreview)
		{
			if (NULL != _resizeData->AdornerLay)
			{
				_resizeData->AdornerLay->InvalidateArrange();
			}
            _resizeData->Grid->InvalidateVisual();

			if (_resizeData->ResizeDirection == GridResizeDirection::grdColumns)
			{
                UpdatePreviewPos(horizontalChange, 0);
			}
			else
            {
                UpdatePreviewPos(0, verticalChange);
            }
		}
        else
        {
            MoveSplitter(horizontalChange, verticalChange);
        }
	}
}

void GridSplitter::OnDragCompleted(suic::DragCompletedEventArg* e)
{
    e->SetHandled(true);
	if (NULL != _resizeData && IsFirstTamplateParentGrid())
	{
		double horizontalChange = e->HorizontalChange();
        double verticalChange = e->VerticalChange();

		MoveSplitter(horizontalChange, verticalChange);
        if(_resizeData->ShowsPreview)	
        {
            RemovePreview();
        }
	}

    if (NULL != _resizeData)
    {
        delete _resizeData;
        _resizeData = NULL;
    }
}

GridResizeDirection GridSplitter::GetEffectiveResizeDirection()
{
	GridResizeDirection gridResizeDirection = GetResizeDirection();

	if (gridResizeDirection == GridResizeDirection::grdAuto)
	{
		if (GetHorizontalAlignment() != 3)
		{
			gridResizeDirection = GridResizeDirection::grdColumns;
		}
		else if (GetVerticalAlignment() != 3)
		{
			gridResizeDirection = GridResizeDirection::grdRows;
		}
		else if (GetActualWidth() <= GetActualHeight())
		{
			gridResizeDirection = GridResizeDirection::grdColumns;
		}
		else
		{
			gridResizeDirection = GridResizeDirection::grdRows;
		}
	}

	return gridResizeDirection;
}

GridResizeBehavior GridSplitter::GetEffectiveResizeBehavior(GridResizeDirection direction)
{
	GridResizeBehavior gridResizeBehavior = GetResizeBehavior();

	if (gridResizeBehavior == GridResizeBehavior::grbBasedOnAlignment)
	{
		if (direction == GridResizeDirection::grdColumns)
		{
			switch (GetHorizontalAlignment())
			{
			case suic::HoriAlign::haLeft:
				gridResizeBehavior = GridResizeBehavior::grbPreviousAndCurrent;
				return gridResizeBehavior;

			case suic::HoriAlign::haRight:
				gridResizeBehavior = GridResizeBehavior::grbCurrentAndNext;
				return gridResizeBehavior;
			}

			gridResizeBehavior = GridResizeBehavior::grbPreviousAndNext;
		}
		else
		{
			switch (GetVerticalAlignment())
			{
			case suic::VertAlign::vaTop:
				gridResizeBehavior = GridResizeBehavior::grbPreviousAndCurrent;
				return gridResizeBehavior;

			case suic::VertAlign::vaBottom:
				gridResizeBehavior = GridResizeBehavior::grbCurrentAndNext;
				return gridResizeBehavior;
			}

			gridResizeBehavior = GridResizeBehavior::grbPreviousAndNext;
		}
	}

	return gridResizeBehavior;
}

suic::Grid* GridSplitter::GetParentGrid()
{
    if (NULL == _parentGrid)
    {
        _parentGrid = RTTICast<Grid>(GetTemplatedParent());
    }
    return _parentGrid;
}

bool GridSplitter::SetupDefinitionsToResize()
{
	int num;
    Grid* grid = GetParentGrid();

	if(NULL != grid)
	{
		num = GetValue((_resizeData->ResizeDirection == GridResizeDirection::grdColumns) ? Grid::ColumnSpanProperty : Grid::RowSpanProperty)->ToInt();	
	}

	if (num == 1)
	{
		int num2 = GetValue((_resizeData->ResizeDirection == GridResizeDirection::grdColumns) ? Grid::ColumnProperty : Grid::RowProperty)->ToInt();

		int num3;
		int num4;

		switch (_resizeData->ResizeBehavior)
		{
		case GridResizeBehavior::grbCurrentAndNext:
			num3 = num2;
			num4 = num2 + 1;
			break;

		case GridResizeBehavior::grbPreviousAndCurrent:
			num3 = num2 - 1;
			num4 = num2;
			break;

		default:
			num3 = num2 - 1;
			num4 = num2 + 1;
			break;
		}

		int num5 = (_resizeData->ResizeDirection == GridResizeDirection::grdColumns) ? _resizeData->Grid->GetColumnDefs()->GetCount() : _resizeData->Grid->GetRowDefs()->GetCount();
		
        if (num3 >= 0 && num4 < num5)
		{
			_resizeData->SplitterIndex = num2;
			_resizeData->Definition1Index = num3;
			_resizeData->Definition1 = GridSplitter::GetGridDefinition(_resizeData->Grid, num3, _resizeData->ResizeDirection);
			_resizeData->OriginalDefinition1Length = new suic::GridSize();
			_resizeData->OriginalDefinition1Length->SetValue(_resizeData->Definition1->GetSizeCache());
			_resizeData->OriginalDefinition1ActualLength = GetActualLength(_resizeData->Definition1);
			_resizeData->Definition2Index = num4;
			_resizeData->Definition2 = GridSplitter::GetGridDefinition(_resizeData->Grid, num4, _resizeData->ResizeDirection);
			_resizeData->OriginalDefinition2Length = new suic::GridSize();
			_resizeData->OriginalDefinition2Length->SetValue(_resizeData->Definition2->GetSizeCache());
			_resizeData->OriginalDefinition2ActualLength = GetActualLength(_resizeData->Definition2);

			bool flag = GridSplitter::IsStar(_resizeData->Definition1);
			bool flag2 = GridSplitter::IsStar(_resizeData->Definition2);

			if (flag && flag2)
			{
				_resizeData->SplitBehavior = GridSplitter::SplitBehavior::Split;
			}
			else if(!flag && !flag2)
			{
				_resizeData->SplitBehavior = GridSplitter::SplitBehavior::ResizeAll;
			}
			else
			{
				_resizeData->SplitBehavior = ((!flag) ? GridSplitter::SplitBehavior::Resize1 : GridSplitter::SplitBehavior::Resize2);
			}

			return true;
		}
	}
	return false;
}

void GridSplitter::InitializeData()
{
	if (IsFirstTamplateParentGrid())
	{
		if (NULL != _parentGrid)
		{
            if (NULL == _resizeData)
            {
                _resizeData = new ResizeData();
                suic::MemZero(_resizeData, sizeof(ResizeData));

                _resizeData->Grid = GetParentGrid();
                _resizeData->ShowsPreview = GetShowsPreview();
                _resizeData->ResizeDirection = GetEffectiveResizeDirection();
                _resizeData->ResizeBehavior = GetEffectiveResizeBehavior(_resizeData->ResizeDirection);
                _resizeData->SplitterLength = min(GetActualWidth(), GetActualHeight());
            }

			if (!SetupDefinitionsToResize())
			{
                delete _resizeData;
                _resizeData = NULL;
				return;
			}
		}
	}

	StartPreview();
	
}

void GridSplitter::CancelResize()
{
	SetDefinitionLength(_resizeData->Definition1, _resizeData->OriginalDefinition1Length);
	SetDefinitionLength(_resizeData->Definition2, _resizeData->OriginalDefinition2Length);	
}

bool GridSplitter::IsStar(DefinitionBase* definition)
{
	return definition->IsStar();
}

bool GridSplitter::IsAuto(DefinitionBase* definition)
{
	return definition->IsAuto();
}

DefinitionBase* GridSplitter::GetGridDefinition(Grid* grid, int index, GridResizeDirection direction)
{
	if (direction != GridResizeDirection::grdColumns)
	{
		return grid->GetRowDefs()->GetItem(index);
	}
	return grid->GetColumnDefs()->GetItem(index);
}

double GridSplitter::GetActualLength(DefinitionBase* definition)
{
	return definition->GetSizeCache();
}

void GridSplitter::SetDefinitionLength(DefinitionBase* definition, GridSize* length)
{
	ColumnDefinition* columnDefinition =DynamicCast<ColumnDefinition>(definition);

	if (NULL != columnDefinition)
	{
		columnDefinition->SetValue(ColumnDefinition::WidthProperty,length);
	}
	else
	{
		(RTTICast<RowDefinition>(definition))->SetValue(RowDefinition::HeightProperty,length);
	}
}

void GridSplitter::SetLengths(double definition1Pixels, double definition2Pixels)
{
	if (_resizeData->SplitBehavior == GridSplitter::SplitBehavior::Split)
	{
		int num = 0;	
		if (_resizeData->ResizeDirection != GridResizeDirection::grdColumns)
		{
			suic::RowDefinitions* rowDefinitions = _resizeData->Grid->GetRowDefs();
			for(int i = 0; i < rowDefinitions->GetCount(); i++)
			{
				DefinitionBase* definition = DynamicCast<DefinitionBase>(rowDefinitions->GetItem(i));
				if (num == _resizeData->Definition1Index)
				{
					SetDefinitionLength(definition, new GridSize(suic::GridSizeType::Star,definition1Pixels));
				}
				else if (num == _resizeData->Definition2Index)
				{
					SetDefinitionLength(definition, new GridSize(suic::GridSizeType::Star,definition2Pixels));
				}
				else if (IsStar(definition))
				{
					SetDefinitionLength(definition, new GridSize(suic::GridSizeType::Star,GetActualLength(definition)));
				}

				num++;
			}

		}
		else
		{
			num = 0;			
			suic::ColumnDefinitions* columnDefinitions = _resizeData->Grid->GetColumnDefs();
			for(int i = 0;i < columnDefinitions->GetCount(); i++)
			{
				DefinitionBase* definition = DynamicCast<DefinitionBase>(columnDefinitions->GetItem(i));
				if (num == _resizeData->Definition1Index)
				{
					SetDefinitionLength(definition, new GridSize(suic::GridSizeType::Star,definition1Pixels));
				}
				else if (num == _resizeData->Definition2Index)
				{
					SetDefinitionLength(definition, new GridSize(suic::GridSizeType::Star,definition2Pixels));
				}
				else if (IsStar(definition))
				{
					SetDefinitionLength(definition, new GridSize(suic::GridSizeType::Star,GetActualLength(definition)));
				}
				num++;
			}
		}

		return;
	}

	if (_resizeData->SplitBehavior == GridSplitter::SplitBehavior::Resize1 || _resizeData->SplitBehavior == GridSplitter::SplitBehavior::ResizeAll)
	{
		if (IsStar(_resizeData->Definition1))
		{
			SetDefinitionLength(_resizeData->Definition1, new GridSize(suic::GridSizeType::Star,definition1Pixels));
		}
		else
        {
            SetDefinitionLength(_resizeData->Definition1, new GridSize(suic::GridSizeType::Pixel,definition1Pixels));
        }

		if (_resizeData->SplitBehavior == GridSplitter::SplitBehavior::Resize1)
        {
            return;
        }
	}

	if (IsStar(_resizeData->Definition2))
	{
		SetDefinitionLength(_resizeData->Definition2, new GridSize(suic::GridSizeType::Star,definition2Pixels));
	}
	else
    {
        SetDefinitionLength(_resizeData->Definition2, new GridSize(suic::GridSizeType::Pixel,definition2Pixels));
    }
}

bool GridSplitter::MoveSplitter(double horizontalChange, double verticalChange)
{
	double num;
	if (_resizeData->ResizeDirection == GridResizeDirection::grdColumns)
	{
		num = horizontalChange;
	}
	else
	{
		num = verticalChange;
	}

	DefinitionBase* definition = _resizeData->Definition1;
	DefinitionBase* definition2 = _resizeData->Definition2;

	if (NULL != definition && NULL != definition2)
	{
		Float actualLength = _resizeData->OriginalDefinition1ActualLength;//GetActualLength(definition);
		Float actualLength2 = _resizeData->OriginalDefinition2ActualLength;//GetActualLength(definition2);
        Float num21 = _resizeData->OriginalDefinition1ActualLength + _resizeData->OriginalDefinition2ActualLength;

		if (_resizeData->SplitBehavior == GridSplitter::SplitBehavior::Split && 
            !FloatUtil::AreClose((Float)(actualLength + actualLength2), num21))
		{
			CancelResize();
			return  false;
		}

		if (GetFlowDirection() != _resizeData->Grid->GetFlowDirection())
		{
			num = -num;
		}

		double num2 = actualLength + num;
		double definition2Pixels = actualLength + actualLength2 - num2;

		SetLengths(num2, definition2Pixels);

		if(_resizeData->Grid!=NULL)
		{
			UpdateGridElement();
			_resizeData->Grid->UpdateLayout();
		}

		return true;
	}

	return false;
}

bool GridSplitter::IsFirstTamplateParentGrid()
{	
	Grid* temgrid = RTTICast<Grid>(GetTemplatedParent());
    if (NULL == temgrid)
    {
        temgrid = RTTICast<Grid>(GetParent());
    }

	if (NULL != temgrid)
	{
		_parentGrid = temgrid;
		return true;
	}
	else
    {
        return false;
    }
}

void GridSplitter::UpdateGridElement()
{
	if (_resizeData->Grid != NULL)
	{
		for (int i = 0;i < _resizeData->Grid->GetChildren()->GetCount(); i++)
		{
			suic::FrameworkElement* _ele = DynamicCast<suic::FrameworkElement>(_resizeData->Grid->GetChildren()->GetAt(i));
			int rowIndex = suic::Grid::GetRow(_ele);
			int columnIndex = suic::Grid::GetColumn(_ele);

			if (_resizeData->ResizeDirection == GridResizeDirection::grdColumns)
			{
				if (_resizeData->Definition1Index==columnIndex || _resizeData->Definition2Index == columnIndex)
				{
					GridSplitter* _sp = DynamicCast<GridSplitter>(_ele);

					if (_sp == NULL)
					{
						if (_ele != NULL && _ele->GetWidth() > 0)
                        {
                            _ele->SetWidth(-1);
                        }
					}					
				}
			}
			else if (_resizeData->ResizeDirection == GridResizeDirection::grdRows)
			{
				if (_resizeData->Definition1Index == rowIndex || _resizeData->Definition2Index == rowIndex)
				{
					GridSplitter* _sp = DynamicCast<GridSplitter>(_ele);

					if (_sp == NULL)
					{
						if (_ele != NULL&& _ele->GetHeight() > 0)
                        {
                            _ele->SetHeight(-1);
                        }
					}
					
				}
			}
			else
			{
				GridSplitter* _sp = DynamicCast<GridSplitter>(_ele);

				if (_sp == NULL)
				{
					if (_ele != NULL && _ele->GetWidth() > 0)
                    {
                        _ele->SetWidth(-1);
                    }

					if (_ele != NULL && _ele->GetHeight() > 0)
                    {
                        _ele->SetHeight(-1);
                    }
				}
				
			}
		}
	}
}

void GridSplitter::UpdatePreviewPos(int x, int y)
{
    if (NULL != _resizeData->AdornerLay)
    {
        Point pos = TransformToAncestor(_resizeData->AdornerLay);
        _resizeData->Adorner->SetOffsetX(pos.x + x);
        _resizeData->Adorner->SetOffsetY(pos.y + y);
    }
}

void GridSplitter::StartPreview()
{
	if (_resizeData->ShowsPreview)
	{
		_resizeData->AdornerLay = suic::AdornerLayer::GetAdornerLayer(_resizeData->Grid);
		if (NULL != _resizeData->AdornerLay)
		{
            _resizeData->Adorner = new suic::PreviewAdorner(NULL, GetPreviewStyle());

            _resizeData->Adorner->SetWidth(GetActualWidth());
            _resizeData->Adorner->SetHeight(GetActualHeight());
            _resizeData->AdornerLay->Add(_resizeData->Adorner);	
		}
	}
}

void GridSplitter::RemovePreview()
{
	if (NULL != _resizeData->Adorner)
	{
        if (NULL != _resizeData->AdornerLay)
        {
            _resizeData->AdornerLay->Remove(_resizeData->Adorner);
            _resizeData->AdornerLay->UpdateLayout();
        }
        delete _resizeData;
        _resizeData = NULL;
	}
}

}

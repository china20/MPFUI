/////////////////////////////////////////////////////////////////////////
// VisualStates.cpp

#include <System/Windows/Object.h>
#include <System/Windows/VisualStates.h>

#include <System/Tools/Memory.h>
#include <System/Types/Integer.h>
#include <System/Resources/Trigger.h>

namespace suic
{

/*String VisualState::StateNormal = _T("Normal");
String VisualState::StatePressed = _T("Pressed"); 
String VisualState::StateDisabled = _T("Disabled"); 
String VisualState::StateMouseOver = _T("MouseOver");
String VisualState::StateUnfocused = _T("Unfocused");
String VisualState::StateFocused = _T("Focused"); 
String VisualState::StateFocusedDropDown = _T("FocusedDropDown");
String VisualState::StateSelected = _T("Selected"); 
String VisualState::StateUnselected = _T("Unselected"); 
String VisualState::StateSelectedUnfocused = _T("SelectedUnfocused");
String VisualState::StateSelectedInactive = _T("SelectedInactive");
String VisualState::StateActive = _T("Active"); 
String VisualState::StateInactive = _T("Inactive");
String VisualState::StateChecked = _T("Checked"); 
String VisualState::StateUnchecked = _T("Unchecked"); 

String VisualState::StateUnsorted = _T("Unsorted");
String VisualState::StateSortAscending = _T("SortAscending"); 
String VisualState::StateSortDescending = _T("SortDescending");

String VisualState::PropBackground = _T("Background");
String VisualState::PropForeground = _T("Foreground");
String VisualState::PropBorderBrush = _T("StrokeBrush");
String VisualState::PropBorderThickness = _T("StrokeThickness");

String VisualState::PropWidth = _T("Width");
String VisualState::PropHeight = _T("Height");
String VisualState::PropMinWidth = _T("MinWidth");
String VisualState::PropMinHeight = _T("MinHeight");
String VisualState::PropMaxWidth = _T("MaxWidth");
String VisualState::PropMaxHeight = _T("MaxHeight");*/

String VisualState::lbxKey = _T("x:Key");
String VisualState::lbStyle = _T("Style");
String VisualState::lbSetter = _T("Setter");

String VisualState::lbValue = _T("Value");
String VisualState::lbSource = _T("Source");
String VisualState::lbProperty = _T("Property");
String VisualState::lbTargetName = _T("TargetName");
String VisualState::lbTargetType = _T("TargetType");

String VisualState::lbTrigger = _T("Trigger");
String VisualState::lbMultiTrigger = _T("MultiTrigger");
String VisualState::lbMultiTriggerConditions = _T("MultiTrigger.Conditions");
String VisualState::lbStyleTrigger = _T("Style.Triggers");
String VisualState::lbTemplateBinding = _T("TemplateBinding");
String VisualState::lbResourceDictionary = _T("ResourceDictionary");
String VisualState::lbMergedDictionaries = _T("ResourceDictionary.MergedDictionaries");
String VisualState::lbControlTemplateTrigger = _T("ControlTemplate.Triggers");

void VisualState::GotoState(FrameworkElement* sender, const String& state)
{
}

}

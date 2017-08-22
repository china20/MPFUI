// 华勤科技版权所有 2010-2011
// 
// 文件名：uivisualstates.h>
// 功  能：定义系统状态。
// 
// 作  者：MPF开发组
// 时  间：2010-07-30
// 
// ============================================================================

#ifndef _UIVISUALSTATES_H_
#define _UIVISUALSTATES_H_

#include <System/Types/Types.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API VisualState
{
public:

    static String StateNormal;
    static String StatePressed; 
    static String StateDisabled; 
    static String StateMouseOver;
    static String StateUnfocused;
    static String StateFocused; 
    static String StateFocusedDropDown;
    static String StateSelected; 
    static String StateUnselected; 
    static String StateSelectedUnfocused;
    static String StateSelectedInactive;
    static String StateActive; 
    static String StateInactive;
    static String StateChecked; 
    static String StateUnchecked; 

    static String StateUnsorted;
    static String StateSortAscending; 
    static String StateSortDescending;

    // 定义常用属性名称
    static String PropBackground;
    static String PropForeground;
    static String PropBorderBrush;
    static String PropBorderThickness;

    static String PropWidth;
    static String PropHeight;
    static String PropMinWidth;
    static String PropMinHeight;
    static String PropMaxWidth;
    static String PropMaxHeight;

    static String PropOpacity;

    //-------------------------------------
    
    static String lbxKey;
    static String lbStyle;
    static String lbSetter;
    static String lbValue;
    static String lbSource;
    static String lbProperty;
    static String lbTargetName;
    static String lbTargetType;
    static String lbTrigger;
    static String lbMultiTrigger;
    static String lbMultiTriggerConditions;
    static String lbStyleTrigger;
    static String lbTemplateBinding;
    static String lbResourceDictionary;
    static String lbMergedDictionaries;
    static String lbControlTemplateTrigger;

    // 切换元素到指定状态做默认处理
    static void GotoState(FrameworkElement* sender, const String& state);
};

}

#endif

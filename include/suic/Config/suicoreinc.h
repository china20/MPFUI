// 华勤科技版权所有 2008-2022
// 
// 文件名：suicoreinc.h
// 功  能：定义核心库的配置文件信息。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _SUICOREINC_H_
#define _SUICOREINC_H_

#define __OS_WIN32

#ifdef _MSC_VER

#pragma warning (disable : 4530)

#pragma warning (disable : 4275)
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4748) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4251) // function or variable may be unsafe (deprecated)

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#endif // _MSC_VER
#ifdef __BORLANDC__
#pragma option -w-8027		   // function not expanded inline
#endif

#pragma comment(lib, "msimg32.lib")

#ifndef SUICORE_LIB

#ifndef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "suicoreud.lib")
#else
#pragma comment(lib, "suicoreu.lib")
#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "suicore64ud.lib")
#else
#pragma comment(lib, "suicore64u.lib")
#endif

#endif

#else

#pragma comment(lib, "usp10.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "windowscodecs.lib")

#ifdef _DEBUG
#if _MSC_VER >= 1900
#pragma comment(lib, "drawlibs14d.lib")
#else
#pragma comment(lib, "drawlibsd.lib")
#endif
#pragma comment(lib, "freetype253_d.lib")
#pragma comment(lib, "suicoresud.lib")
#else
#if _MSC_VER >= 1900
#pragma comment(lib, "drawlib14s.lib")
#else
#pragma comment(lib, "drawlibs.lib")
#endif
#pragma comment(lib, "freetype253.lib")
#pragma comment(lib, "suicoresu.lib")
#endif

#endif

#ifndef SUICORE_LIB

#ifdef SUICORE_EXPORTS

#define SUICORE_API __declspec(dllexport)
#else
#define SUICORE_API __declspec(dllimport)
#endif

#else

#define SUICORE_API

#endif

#include <tchar.h>
#include <stddef.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <windows.h>
#include <olectl.h>
#include <commctrl.h>

#include <System/Windows/Object.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/ScrollInfo.h>
#include <System/Windows/BuildFactory.h>
#include <System/Windows/VisualStates.h>
#include <System/Windows/CommandSource.h>

#include <System/Types/Numeric.h>
#include <System/Types/DateTime.h>
#include <System/Types/StreamDef.h>

#include <System/Tools/Math.h>
#include <System/Tools/Array.h>
#include <System/Tools/Locker.h>
#include <System/Tools/Thread.h>
#include <System/Tools/HashMap.h>
#include <System/Tools/TreeBuilder.h>
#include <System/Tools/AlphaHelper.h>
#include <System/Tools/AcceleratorOp.h>
#include <System/Tools/Collection.h>
#include <System/Tools/CollectionX.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Tools/DIBHelper.h>
#include <System/Tools/CoreHelper.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/IconHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Tools/TreeBuilder.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandOp.h>
#include <System/Tools/ThemeOp.h>

#include <Framework/Controls/Image.h>
#include <Framework/Controls/Label.h>
#include <Framework/Controls/Button.h>
#include <Framework/Controls/Border.h>
#include <Framework/Controls/Slider.h>
#include <Framework/Controls/Window.h>
#include <Framework/Controls/CheckBox.h>
#include <Framework/Controls/Expander.h>
#include <Framework/Controls/Separator.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Controls/RadioButton.h>
#include <Framework/Controls/RepeatButton.h>
#include <Framework/Controls/HwndHost.h>
#include <Framework/Controls/Line.h>
#include <Framework/Controls/Path.h>
#include <Framework/Controls/Ellipse.h>
#include <Framework/Controls/Rectangle.h>
#include <Framework/Controls/Application.h>
#include <Framework/Controls/CaretElement.h>
#include <Framework/Controls/DataTemplate.h>
#include <Framework/Controls/ProgressBar.h>
#include <Framework/Controls/HierarchicalItem.h>

#include <Framework/Controls/DatePicker.h>

#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Canvas.h>
#include <Framework/Controls/StackPanel.h>
#include <Framework/Controls/UniformGrid.h>
#include <Framework/Controls/VisualizeStackPanel.h>

#include <Framework/Controls/GridView.h>
#include <Framework/Controls/GridViewColumn.h>
#include <Framework/Controls/GridViewColumnHeader.h>
#include <Framework/Controls/GridViewHeaderRowPresenter.h>

#include <Framework/Animation/AnimationAsSeries.h>
#include <Framework/Animation/AnimationAsKeyframe.h>

#include <System/Windows/ResourceUri.h>
#include <System/Windows/RenderTarget.h>
#include <System/Windows/HwndAdapter.h>

#include <System/Graphics/Bitmap.h>
#include <System/Graphics/Drawing.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/Bitmaptool.h>
#include <System/Graphics/SolidColorBrush.h>
#include <System/Graphics/RadialGradientBrush.h>
#include <System/Graphics/LinearGradientBrush.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>
#include <System/Input/KeyboardNavigation.h>

#endif

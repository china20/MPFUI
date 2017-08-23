
#include "stdafx.h"
#include "BindingItem.h"

namespace FrameworkCore
{	

 /************************************************************************/
 /*
 PropertyDescriptors  
 用于绑定于支持动态生成列及值变化事件监听的控件   
 */
 /************************************************************************/
 PropertyDescriptors::PropertyDescriptors()
 {
     _flag=-1;
     _isChecked = suic::Boolean::False;
     _isReadOnly= suic::Boolean::False;
     _propertys.Clear();
 }

 PropertyDescriptors::~PropertyDescriptors()
 {
     _propertys.Clear();
 }

 void PropertyDescriptors::OnPropertyChanged(suic::String pro)
 {
     NotifyChanged(pro);
     //if(PropertyChanged)
     //    PropertyChanged(this,new suic::PropertyChangedEventArg(pro));
 }

 suic::Object* PropertyDescriptors::GetIsChecked()
 {
     return _isChecked;
 }

 void PropertyDescriptors::SetIsChecked(suic::Object* val)
 {
     _isChecked = suic::DynamicCast<suic::Boolean>(val);	
     OnPropertyChanged("IsChecked");
 }

 suic::Object* PropertyDescriptors::GetIsReadOnly()
 {
     return _isReadOnly;
 }
 void PropertyDescriptors::SetIsReadOnly(suic::Object* val)
 {
     _isReadOnly = suic::DynamicCast<suic::Boolean>(val);				
     OnPropertyChanged("IsReadOnly");
 }

 void PropertyDescriptors::SetTip(suic::String tip)
 {
     _tip=tip;
     OnPropertyChanged("Tip");
 }

 suic::String PropertyDescriptors::GetTip()
 {
     return _tip;
 }

 bool PropertyDescriptors::GetProperty(const suic::String& name, suic::ObjectPtr& val)
 {
     suic::Object* obj = NULL;
     bool bExist = _propertys.TryGetValue(name, obj);
     val = obj;
     return bExist;
 }

 bool PropertyDescriptors::SetProperty(const suic::String& name, suic::Object* val)
 {
     if (_propertys.ContainsKey(name))
     {
         _propertys.FindPair(name)->val=val;
     }
     else
     {
         _propertys.Add(name,val);
     }
     OnPropertyChanged(name);
     return true;
 }

 suic::String PropertyDescriptors::ToString()
 {
     return "PropertyDescriptors";
 }

 void PropertyDescriptors::SetTag(suic::Object* tag)
 {
     _bindingTag=tag;
     OnPropertyChanged("Tag");
 }

 suic::Object* PropertyDescriptors::GetTag()
 {
     return _bindingTag;
 }

 void PropertyDescriptors::SetFlag(int flag)
 {
     _flag = flag;
     OnPropertyChanged("Flag");
 }

 int PropertyDescriptors::GetFlag()
 {
     return _flag;
 }

 suic::StringAutoDic<suic::Object> &PropertyDescriptors::Properties()
 {
     return _propertys;
 }

}

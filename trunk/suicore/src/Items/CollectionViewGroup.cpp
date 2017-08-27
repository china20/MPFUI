
#include <Framework/Items/CollectionViewGroup.h>

namespace suic
{

ImplementRTTIOfClass(GroupStyle, suic::DpObject)
ImplementRTTIOfClass(GroupStyleSelector, suic::DpObject)

GroupStyle* GroupStyle::s_DefaultGroupStyle = new GroupStyle();
ItemsPanelTemplate* GroupStyle::DefaultGroupPanel;

}

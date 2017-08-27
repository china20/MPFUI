
#include <Framework/Controls/UserControl.h>

namespace suic
{

ImplementRTTIOfClass(UserControl, suic::ContentControl)

bool UserControl::StaticInit()
{
    return true;
}

UserControl::UserControl()
{
}

UserControl::~UserControl()
{
}

}

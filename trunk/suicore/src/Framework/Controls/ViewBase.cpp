/////////////////////////////////////////////////////////////////////////
// ViewBase.cpp


#include <Framework/Controls/ViewBase.h>

namespace suic
{

ImplementRTTIOfClass(ViewBase, DpObject)

bool ViewBase::StaticInit()
{
    return true;
}

//----------------------------------------------------

ViewBase::ViewBase()
{

}

ViewBase::~ViewBase()
{

}

void ViewBase::LinkViewOwner(DpObject* owner)
{

}

void ViewBase::UnlinkViewOwner(DpObject* owner)
{

}

void ViewBase::PrepareItem(DpObject* item)
{

}

}

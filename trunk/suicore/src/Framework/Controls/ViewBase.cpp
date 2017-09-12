/////////////////////////////////////////////////////////////////////////
// ViewBase.cpp


#include <Framework/Controls/ViewBase.h>

namespace suic
{

ImplementRTTIOfClass(ViewBase, DpObject)

void ViewBase::StaticInit()
{
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

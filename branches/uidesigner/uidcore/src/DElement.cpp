//////////////////////////////////////////////////////////////////////////////
// DElement.cpp

#include "stdafx.h"
#include "DElement.h"

DElement::DElement()
{
    _setters = new DSetters();
    _resources = new DResources();
}

DElement::~DElement()
{
}


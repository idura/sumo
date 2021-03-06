/****************************************************************************/
/// @file    AGBus.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// A bus driving in the city
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "AGBus.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void AGBus::setName(std::string name) {
    this->name= name;
}

int
AGBus::getDeparture() {
    return departureTime;
}

string
AGBus::getName() {
    return name;
}

void
AGBus::print() {
    cout << "- Bus:" << " name=" << name << " depTime=" << departureTime << endl;
}

/****************************************************************************/

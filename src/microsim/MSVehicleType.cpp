/****************************************************************************/
/// @file    MSVehicleType.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// The car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "MSVehicleType.h"
#include "MSNet.h"
#include <cassert>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSVehicleType::MSVehicleType(const string &id, SUMOReal length,
                             SUMOReal maxSpeed, SUMOReal accel,
                             SUMOReal decel, SUMOReal dawdle,
                             SUMOReal tau, SUMOReal prob,
                             SUMOReal speedFactor, SUMOReal speedDev,
                             SUMOVehicleClass vclass,
                             SUMOEmissionClass emissionClass,
                             SUMOVehicleShape shape,
                             SUMOReal guiWidth, SUMOReal guiOffset,
                             const string &cfModel, const string &lcModel,
                             const RGBColor &c) throw()
        : myID(id), myLength(length), myMaxSpeed(maxSpeed), myAccel(accel),
        myDecel(decel), myDawdle(dawdle), myTau(tau),
        myDefaultProbability(prob), mySpeedFactor(speedFactor),
        mySpeedDev(speedDev), myVehicleClass(vclass),
        myCarFollowModel(cfModel), myLaneChangeModel(lcModel),
        myEmissionClass(emissionClass), myColor(c),
        myWidth(guiWidth), myOffset(guiOffset), myShape(shape) {
    assert(myLength > 0);
    assert(getMaxSpeed() > 0);
    assert(myAccel > 0);
    assert(myDecel > 0);
    assert(myDawdle >= 0 && myDawdle <= 1);
    myInverseTwoDecel = SUMOReal(1) / (SUMOReal(2) * myDecel);
    myTauDecel = myDecel * myTau;
}


MSVehicleType::~MSVehicleType() throw() {}


void
MSVehicleType::saveState(std::ostream &os) {
    FileHelpers::writeString(os, myID);
    FileHelpers::writeFloat(os, myLength);
    FileHelpers::writeFloat(os, getMaxSpeed());
    FileHelpers::writeFloat(os, myAccel);
    FileHelpers::writeFloat(os, myDecel);
    FileHelpers::writeFloat(os, myDawdle);
    FileHelpers::writeFloat(os, myTau);
    FileHelpers::writeInt(os, (int) myVehicleClass);
    FileHelpers::writeInt(os, (int) myEmissionClass);
    FileHelpers::writeInt(os, (int) myShape);
    FileHelpers::writeFloat(os, myWidth);
    FileHelpers::writeFloat(os, myOffset);
    FileHelpers::writeFloat(os, myDefaultProbability);
    FileHelpers::writeFloat(os, mySpeedFactor);
    FileHelpers::writeFloat(os, mySpeedDev);
    FileHelpers::writeFloat(os, myColor.red());
    FileHelpers::writeFloat(os, myColor.green());
    FileHelpers::writeFloat(os, myColor.blue());
    FileHelpers::writeString(os, myCarFollowModel);
    FileHelpers::writeString(os, myLaneChangeModel);
}



/****************************************************************************/


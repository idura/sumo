/****************************************************************************/
/// @file    NIVissimVehicleType.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


#include "NIVissimVehicleType.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

NIVissimVehicleType::DictType NIVissimVehicleType::myDict;

NIVissimVehicleType::NIVissimVehicleType(int id,
        const std::string &name, const std::string &category, SUMOReal length,
        const RGBColor &color, SUMOReal amax, SUMOReal dmax)
        : myID(id), myName(name), myCategory(category), myLength(length),
        myColor(color), myAMax(amax), myDMax(dmax)
{}


NIVissimVehicleType::~NIVissimVehicleType()
{}



bool
NIVissimVehicleType::dictionary(int id,
                                const std::string &name, const std::string &category, SUMOReal length,
                                const RGBColor &color, SUMOReal amax, SUMOReal dmax)
{
    NIVissimVehicleType *o = new NIVissimVehicleType(id, name, category,
                             length, color, amax, dmax);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimVehicleType::dictionary(int id, NIVissimVehicleType *o)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimVehicleType *
NIVissimVehicleType::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

void
NIVissimVehicleType::clearDict()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}



/****************************************************************************/


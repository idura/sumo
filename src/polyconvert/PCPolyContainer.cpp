/****************************************************************************/
/// @file    PCPolyContainer.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A storage for loaded polygons and pois
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <map>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/shapes/Polygon2D.h>
#include "PCPolyContainer.h"
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
PCPolyContainer::PCPolyContainer(bool prune,
                                 const Boundary &prunningBoundary,
                                 const std::vector<std::string> &removeByNames) throw()
        : myPrunningBoundary(prunningBoundary), myDoPrunne(prune),
        myRemoveByNames(removeByNames) {}


PCPolyContainer::~PCPolyContainer() throw() {
    clear();
}


bool
PCPolyContainer::insert(const std::string &id, Polygon2D *poly,
                        int layer, bool ignorePrunning) throw() {
    // check whether the polygon lies within the wished area
    //  - if such an area was given
    if (myDoPrunne&&!ignorePrunning) {
        Boundary b = poly->getShape().getBoxBoundary();
        if (!b.partialWithin(myPrunningBoundary)) {
            delete poly;
            return true;
        }
    }
    // check whether the polygon was named to be a removed one
    if (find(myRemoveByNames.begin(), myRemoveByNames.end(), id)!=myRemoveByNames.end()) {
        delete poly;
        return true;
    }
    //
    PolyCont::iterator i=myPolyCont.find(id);
    if (i!=myPolyCont.end()) {
        return false;
    }
    myPolyCont[id] = poly;
    myPolyLayerMap[poly] = layer;
    return true;
}


bool
PCPolyContainer::insert(const std::string &id, PointOfInterest *poi,
                        int layer, bool ignorePrunning) throw() {
    // check whether the poi lies within the wished area
    //  - if such an area was given
    if (myDoPrunne&&!ignorePrunning) {
        if (!myPrunningBoundary.around(*poi)) {
            delete poi;
            return true;
        }
    }
    // check whether the polygon was named to be a removed one
    if (find(myRemoveByNames.begin(), myRemoveByNames.end(), id)!=myRemoveByNames.end()) {
        delete poi;
        return true;
    }
    //
    POICont::iterator i=myPOICont.find(id);
    if (i!=myPOICont.end()) {
        return false;
    }
    myPOICont[id] = poi;
    myPOILayerMap[poi] = layer;
    return true;
}


bool
PCPolyContainer::containsPolygon(const std::string &id) throw() {
    return myPolyCont.find(id)!=myPolyCont.end();
}


void
PCPolyContainer::clear() throw() {
    // polys
    for (PolyCont::iterator i=myPolyCont.begin(); i!=myPolyCont.end(); i++) {
        delete(*i).second;
    }
    myPolyCont.clear();
    myPolyLayerMap.clear();
    // pois
    for (POICont::iterator i=myPOICont.begin(); i!=myPOICont.end(); i++) {
        delete(*i).second;
    }
    myPOICont.clear();
    myPOILayerMap.clear();
}


void
PCPolyContainer::report() throw() {
    WRITE_MESSAGE("   " + toString(getNoPolygons()) + " polygons loaded.");
    WRITE_MESSAGE("   " + toString(getNoPOIs()) + " pois loaded.");
}


void
PCPolyContainer::save(const std::string &file) throw(IOError) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("shapes", " encoding=\"iso-8859-1\"");
    // write polygons
    for (PolyCont::iterator i=myPolyCont.begin(); i!=myPolyCont.end(); ++i) {
        out.openTag("poly") << " id=\"" << StringUtils::escapeXML((*i).second->getID())
        << "\" type=\"" << (*i).second->getType()
        << "\" color=\"" << (*i).second->getColor()
        << "\" fill=\"" << (*i).second->fill()
        << "\" layer=\"" << myPolyLayerMap[(*i).second]
        << "\" shape=\"" << (*i).second->getShape() << "\"";
        out.closeTag(true);
    }
    // write pois
    for (POICont::iterator i=myPOICont.begin(); i!=myPOICont.end(); ++i) {
        out.openTag("poi") << " id=\"" << StringUtils::escapeXML((*i).second->getID())
        << "\" type=\"" << StringUtils::escapeXML((*i).second->getType())
        << "\" color=\"" << *static_cast<RGBColor*>((*i).second)
        << "\" layer=\"" << myPOILayerMap[(*i).second]
        << "\" x=\"" << (*i).second->x()
        << "\" y=\"" << (*i).second->y() << "\"";
        out.closeTag(true);
    }
    out.close();
}


int
PCPolyContainer::getEnumIDFor(const std::string &key) throw() {
    if (myIDEnums.find(key)==myIDEnums.end()) {
        myIDEnums[key] = 0;
        return 0;
    } else {
        myIDEnums[key] = myIDEnums[key] + 1;
        return myIDEnums[key];
    }
}



/****************************************************************************/


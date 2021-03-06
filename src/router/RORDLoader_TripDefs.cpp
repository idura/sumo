/****************************************************************************/
/// @file    RORDLoader_TripDefs.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for loading trip definitions
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "RORouteDef_OrigDest.h"
#include "RORDLoader_TripDefs.h"
#include "ROVehicle.h"
#include "RORouteDef_Complete.h"
#include "ROAbstractRouteDefLoader.h"
#include <utils/xml/SUMOVehicleParserHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORDLoader_TripDefs::RORDLoader_TripDefs(RONet &net,
        SUMOTime begin, SUMOTime end,
        bool emptyDestinationsAllowed, bool withTaz,
        const std::string &fileName) throw(ProcessError)
        : ROTypedXMLRoutesLoader(net, begin, end, fileName),
        myEmptyDestinationsAllowed(emptyDestinationsAllowed),
        myWithTaz(withTaz),
        myDepartureTime(-1), myCurrentVehicleType(0),
        myParameter(0) {}


RORDLoader_TripDefs::~RORDLoader_TripDefs() throw() {}


void
RORDLoader_TripDefs::myStartElement(SumoXMLTag element,
                                    const SUMOSAXAttributes &attrs) throw(ProcessError) {
    // check whether a trip definition shall be parsed
    if (element==SUMO_TAG_TRIPDEF) {
        bool ok = true;
        // get the vehicle id, the edges, the speed and position and
        //  the departure time and other information
        std::string id = getVehicleID(attrs);
        myDepartureTime = attrs.getSUMOTimeReporting(SUMO_ATTR_DEPART, "tripdef", id.c_str(), ok);
        if (myWithTaz) {
            myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM_TAZ, id, false);
            myEndEdge = getEdge(attrs, "destination", SUMO_ATTR_TO_TAZ, id, myEmptyDestinationsAllowed);
        } else {
            myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, id, false);
            myEndEdge = getEdge(attrs, "destination", SUMO_ATTR_TO, id, myEmptyDestinationsAllowed);
        }
        myParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, true);
        myParameter->id = id;
        // recheck attributes
        if (!ok) {
            return;
        }
        if (myDepartureTime<0) {
            MsgHandler::getErrorInstance()->inform("The departure time must be positive.");
            return;
        }
    }
    // check whether a vehicle type shall be parsed
    if (element==SUMO_TAG_VTYPE) {
        myCurrentVehicleType = SUMOVehicleParserHelper::beginVTypeParsing(attrs);
    } else if (myCurrentVehicleType!=0) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVehicleType, element, attrs);
    }
}


std::string
RORDLoader_TripDefs::getVehicleID(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    attrs.setIDFromAttributes("tripdef", id, false);
    // get a valid vehicle id
    if (id=="") {
        id = myIdSupplier.getNext();
    }
    return id;
}


ROEdge *
RORDLoader_TripDefs::getEdge(const SUMOSAXAttributes &attrs,
                             const std::string &purpose,
                             SumoXMLAttr which, const std::string &vid,
                             bool emptyAllowed) {
    bool ok = true;
    std::string id = attrs.getStringReporting(which, 0, 0, ok, !emptyAllowed);
    if (which == SUMO_ATTR_FROM_TAZ) {
        id += "-source";
    }
    if (which == SUMO_ATTR_TO_TAZ) {
        id += "-sink";
    }
    ROEdge *e = myNet.getEdge(id);
    if (e==0 && !emptyAllowed) {
        MsgHandler::getErrorInstance()->inform("The edge '" + id + "' is not known.\n Vehicle id='" + vid + "'.");
    }
    return e;
}


SUMOTime
RORDLoader_TripDefs::getPeriod(const SUMOSAXAttributes &attrs,
                               const std::string &id) {
    if (!attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        return -1;
    }
    // get the repetition period
    bool ok = true;
    return attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, "tripdef", id.c_str(), ok);
}


int
RORDLoader_TripDefs::getRepetitionNumber(const SUMOSAXAttributes &attrs,
        const std::string &id) {
    if (!attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        return -1;
    }
    // get the repetition number
    bool ok = true;
    return attrs.getIntReporting(SUMO_ATTR_REPNUMBER, "tripdef", id.c_str(), ok);
}


std::string
RORDLoader_TripDefs::getLane(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    return attrs.getOptStringReporting(SUMO_ATTR_LANE, 0, 0, ok, "");
}


void
RORDLoader_TripDefs::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_TRIPDEF &&
            !MsgHandler::getErrorInstance()->wasInformed()) {

        if (myDepartureTime<myBegin||myDepartureTime>=myEnd) {
            delete myParameter;
            return;
        }
        RGBColor *col = myParameter->wasSet(VEHPARS_COLOR_SET) ? new RGBColor(myParameter->color) : 0;
        RORouteDef *route = new RORouteDef_OrigDest(myParameter->id, col, myBeginEdge, myEndEdge);
        SUMOVTypeParameter *type = myNet.getVehicleTypeSecure(myParameter->vtypeid);
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            return;
        }
        if (myNet.addRouteDef(route)) {
            myNextRouteRead = true;
            // build the vehicle
            ROVehicle *veh = new ROVehicle(*myParameter, route, type);
            myNet.addVehicle(myParameter->id, veh);
        } else {
            MsgHandler::getErrorInstance()->inform("The vehicle '" + myParameter->id + "' occurs at least twice.");
            delete route;
        }
        delete myParameter;
        myParameter = 0;
    }
    if (element==SUMO_TAG_VTYPE) {
        SUMOVehicleParserHelper::closeVTypeParsing(*myCurrentVehicleType);
        myNet.addVehicleType(myCurrentVehicleType);
        myCurrentVehicleType = 0;
    }
}


void
RORDLoader_TripDefs::beginNextRoute() throw() {
    myNextRouteRead = false;
}


/****************************************************************************/


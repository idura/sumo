/***************************************************************************
                          DFDetectorFlow.cpp
    Storage for flows within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2006/04/07 10:44:18  dkrajzew
// multiple detector and flows definitions can be read
//
// Revision 1.5  2006/04/05 05:35:26  dkrajzew
// further work on the dfrouter
//
// Revision 1.4  2006/03/17 09:04:25  dkrajzew
// class-documentation added/patched
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <cassert>
#include "DFDetectorFlow.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
DFDetectorFlows::DFDetectorFlows(SUMOTime startTime, SUMOTime endTime,
                                 SUMOTime stepOffset)
    : myBeginTime(startTime), myEndTime(endTime), myStepOffset(stepOffset)
{
}


DFDetectorFlows::~DFDetectorFlows()
{

}


void
DFDetectorFlows::addFlow( const std::string &id, int t, const FlowDef &fd )
{
    if(myFastAccessFlows.find(id)==myFastAccessFlows.end()) {
        size_t noItems = (size_t) ((myEndTime-myBeginTime)/myStepOffset);
        myFastAccessFlows[id] = std::vector<FlowDef>(noItems);
        std::vector<FlowDef> &cflows = myFastAccessFlows.find(id)->second;
        // initialise
        for(std::vector<FlowDef>::iterator i=cflows.begin(); i<cflows.end(); ++i) {
            (*i).qPKW = 0;
            (*i).qLKW = 0;
            (*i).vPKW = 0;
            (*i).vLKW = 0;
            (*i).fLKW = 0;
            (*i).isLKW = 0;
            (*i).firstSet = true;
        }
    }
    assert(t<myFastAccessFlows[id].size());
    FlowDef &ofd = myFastAccessFlows[id][t];
    if(ofd.firstSet) {
        ofd = fd;
        ofd.firstSet = false;
    } else {
        ofd.qLKW = ofd.qLKW + fd.qLKW;
        ofd.qPKW = ofd.qPKW + fd.qPKW;
        ofd.vLKW = ofd.vLKW + fd.vLKW;
        ofd.vPKW = ofd.vPKW + fd.vPKW;
    }
    if ( ofd.qLKW!=0 && ofd.qPKW!=0 ) {
	    ofd.fLKW = ofd.qLKW / ofd.qPKW ;
    } else {
	    ofd.fLKW = 0;
    }
}




void
DFDetectorFlows::setFlows(const std::string &detector_id,
                          std::vector<FlowDef> &flows)
{
    for(std::vector<FlowDef>::iterator i=flows.begin(); i<flows.end(); ++i) {
        FlowDef &ofd = *i;
        if ( ofd.qLKW!=0 && ofd.qPKW!=0 ) {
	        ofd.fLKW = ofd.qLKW / ofd.qPKW ;
        } else {
	        ofd.fLKW = 0;
        }
    }
    myFastAccessFlows[detector_id] = flows;
}


void
DFDetectorFlows::removeFlow(const std::string &detector_id)
{
    if(myFastAccessFlows.find(detector_id)!=myFastAccessFlows.end()) {
        myFastAccessFlows.erase(myFastAccessFlows.find(detector_id));
    }
}

/*
const FlowDef &
DFDetectorFlows::getFlowDef( const std::string &id, int t ) const
{
	return myCurrFlows.find(id)->second.find(t)->second;
}


const std::map< int, FlowDef > &
DFDetectorFlows::getFlowDefs( const std::string &id ) const
{
	return myCurrFlows.find(id)->second;
}
*/

bool
DFDetectorFlows::knows( const std::string &det_id ) const
{
	return myFastAccessFlows.find(det_id)!=myFastAccessFlows.end();
}


bool
DFDetectorFlows::knows( const std::string &det_id, SUMOTime time ) const
{
    if(myFastAccessFlows.find(det_id)==myFastAccessFlows.end()) {
        return false;
    }
    return true;//!!!
}

/*
void
DFDetectorFlows::buildFastAccess(SUMOTime startTime, SUMOTime endTime,
                                 SUMOTime stepOffset)
{
    myBeginTime = startTime;
    myEndTime = endTime;
    myStepOffset = stepOffset;
    size_t noItems = (size_t) ((endTime-startTime)/stepOffset);
    std::map<std::string, std::map<SUMOTime, FlowDef> >::iterator j;
    // go through detectors
    for(j=myCurrentFlows.begin(); j!=myCurrentFlows.end(); ++j) {
        myFastAccessFlows[(*j).first] = std::vector<FlowDef>(noItems);
        std::vector<FlowDef> cflows = myFastAccessFlows.find((*j).first)->second;
        // initialise
        for(size_t k=0; k<noItems; k++) {
            cflows[k].qPKW = 0;
            cflows[k].qLKW = 0;
            cflows[k].vPKW = 0;
            cflows[k].vLKW = 0;
            cflows[k].fLKW = 0;
            cflows[k].isLKW = 0;
        }
        // build
        std::map<SUMOTime, FlowDef> &prev = myCurrentFlows.find((*j).first)->second;
        for(std::map<SUMOTime, FlowDef>::iterator i=prev.begin(); i!=prev.end(); i++) {
            cflows[(*i).first - myBeginTime] = (*i).second;
        }
    }
}
*/

const std::vector<FlowDef> &
DFDetectorFlows::getFlowDefs( const std::string &id ) const
{
    assert(myFastAccessFlows.find(id)!=myFastAccessFlows.end());
    assert(myFastAccessFlows.find(id)->second.size()!=0);
    return myFastAccessFlows.find(id)->second;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


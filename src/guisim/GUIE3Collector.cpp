//---------------------------------------------------------------------------//
//                        GUIE3Collector.cpp -
//  The gui-version of the MSE3Collector, together with the according
//   wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
        "$Id$";
}
// $Log$
// Revision 1.4  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.3  2004/02/10 07:07:13  dkrajzew
// debugging of network loading after a network failed to be loaded; memory
//  leaks removal
//
// Revision 1.2  2004/02/05 16:30:59  dkrajzew
// multiplicate deletion of E3-detectors on application quit patched
//
// Revision 1.1  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets
//  and lengths for lsa-detectors; coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIE3Collector.h"
#include "GUIEdge.h"
#include <utils/geom/Line2D.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <gui/textures/GUITexturesHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
GUIE3Collector::InstanceVector GUIE3Collector::myInstances;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUIE3Collector::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIE3Collector::MyWrapper::MyWrapper(GUIE3Collector &detector,
                                     GUIGlObjectStorage &idStorage)
    : GUIDetectorWrapper(idStorage, string("E3 detector:")+detector.getId()),
    myDetector(detector)
{
    const Detector::CrossSections &entries = detector.getEntries();
    const Detector::CrossSections &exits = detector.getExits();
    Detector::CrossSections::const_iterator i;
    for(i=entries.begin(); i!=entries.end(); i++) {
        SingleCrossingDefinition def = buildDefinition(*i, false);
        myBoundery.add(def.mySGPosition);
        myBoundery.add(def.myFGPosition);
        myEntryDefinitions.push_back(def);
    }
    for(i=exits.begin(); i!=exits.end(); i++) {
        SingleCrossingDefinition def = buildDefinition(*i, true);
        myBoundery.add(def.mySGPosition);
        myBoundery.add(def.myFGPosition);
        myExitDefinitions.push_back(def);
    }
}


GUIE3Collector::MyWrapper::~MyWrapper()
{
}


GUIE3Collector::MyWrapper::SingleCrossingDefinition
GUIE3Collector::MyWrapper::buildDefinition(const MSCrossSection &section,
                                           bool exit)
{
    const MSLane *lane = section.laneM;
    double pos = section.posM;
    const GUILaneWrapper &lw =
        static_cast<const GUIEdge&>(lane->edge()).getLaneGeometry(lane);
    const Position2DVector &v =
        static_cast<const GUIEdge&>(lane->edge()).getLaneGeometry(static_cast<const MSLane*>(lane)).getShape();
    Line2D l(v.getBegin(), v.getEnd());

    SingleCrossingDefinition def;
    def.mySGPosition = l.getPositionAtDistance(pos);
    def.mySGRotation = -l.atan2DegreeAngle();
    def.myFGPosition = v.positionAtLengthPosition(pos);
    def.myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    return def;
}


Boundery
GUIE3Collector::MyWrapper::getBoundery() const
{
    return myBoundery;
}


GUIParameterTableWindow *
GUIE3Collector::MyWrapper::getParameterWindow(GUIApplicationWindow &app,
                                              GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 3);
    // add items
    myMkExistingItem(*ret, "mean travel time [s]",
        E3::MEAN_TRAVELTIME);
    myMkExistingItem(*ret, "mean halting number [n]",
        E3::MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE);
    myMkExistingItem(*ret, "vehicle number [n]",
        E3::NUMBER_OF_VEHICLES);
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIE3Collector::MyWrapper::myMkExistingItem(GUIParameterTableWindow &ret,
                                            const std::string &name,
                                            E3::DetType type)
{
    if(!myDetector.hasDetector(type)) {
        return;
    }
    MyValueRetriever *binding =
        new MyValueRetriever(myDetector, type, 1);
    ret.mkItem(name.c_str(), true, binding);
}


std::string
GUIE3Collector::MyWrapper::microsimID() const
{
    return myDetector.getId();
}


bool
GUIE3Collector::MyWrapper::active() const
{
    return true;
}


void
GUIE3Collector::MyWrapper::drawGL_SG(double scale,
                                     GUIBaseDetectorDrawer &drawer) const
{
    typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;
    CrossingDefinitions::const_iterator i;
    glColor3f(0, .8, 0);
    for(i=myEntryDefinitions.begin(); i!=myEntryDefinitions.end(); i++) {
        drawSingleCrossing((*i).mySGPosition, (*i).mySGRotation, drawer);
    }
    glColor3f(.8, 0, 0);
    for(i=myExitDefinitions.begin(); i!=myExitDefinitions.end(); i++) {
        drawSingleCrossing((*i).mySGPosition, (*i).mySGRotation, drawer);
    }
}


void
GUIE3Collector::MyWrapper::drawGL_FG(double scale,
                                     GUIBaseDetectorDrawer &drawer) const
{
    typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;
    CrossingDefinitions::const_iterator i;
    glColor3f(0, .8, 0);
    for(i=myEntryDefinitions.begin(); i!=myEntryDefinitions.end(); i++) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, drawer);
    }
    glColor3f(.8, 0, 0);
    for(i=myExitDefinitions.begin(); i!=myExitDefinitions.end(); i++) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, drawer);
    }
}


void
GUIE3Collector::MyWrapper::drawSingleCrossing(const Position2D &pos,
        double rot,
        GUIBaseDetectorDrawer &drawer) const
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPushMatrix();
    glTranslated(pos.x(), pos.y(), 0);
    glRotated( rot, 0, 0, 1 );
    glBegin( GL_LINES);
    glVertex2f(1.7, 0);
    glVertex2f(-1.7, 0);
    glEnd();
    glBegin( GL_QUADS );
    glVertex2f(-1.7, .5);
    glVertex2f(-1.7, -.5);
    glVertex2f(1.7, -.5);
    glVertex2f(1.7, .5);
    glEnd();
    // arrows
    glTranslated(1.5, 0, 0);
    GUITexturesHelper::drawTexturedBox(TEXTURE_LINKDIR_STRAIGHT, 1.0);
    glTranslated(-3, 0, 0);
    GUITexturesHelper::drawTexturedBox(TEXTURE_LINKDIR_STRAIGHT, 1.0);
    glPopMatrix();
}


Position2D
GUIE3Collector::MyWrapper::getPosition() const
{
    return myBoundery.getCenter();
}


GUIE3Collector &
GUIE3Collector::MyWrapper::getDetector()
{
    return myDetector;
}


/* -------------------------------------------------------------------------
 * GUIE3Collector-methods
 * ----------------------------------------------------------------------- */
GUIE3Collector::GUIE3Collector(std::string id,
        Detector::CrossSections entries,  Detector::CrossSections exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
        : MSE3Collector(id, entries,  exits,
        haltingTimeThreshold, haltingSpeedThreshold,
        deleteDataAfterSeconds)
{
    addBuild(this);
}


GUIE3Collector::~GUIE3Collector()
{
}


const Detector::CrossSections &
GUIE3Collector::getEntries() const
{
    return entriesM;
}


const Detector::CrossSections &
GUIE3Collector::getExits() const
{
    return exitsM;
}



void
GUIE3Collector::addBuild(GUIE3Collector *det)
{
    myInstances.push_back(det);
}


const GUIE3Collector::InstanceVector &
GUIE3Collector::getInstances()
{
    return myInstances;
}


GUIDetectorWrapper *
GUIE3Collector::buildDetectorWrapper(GUIGlObjectStorage &idStorage)
{
    return new MyWrapper(*this, idStorage);
}


void
GUIE3Collector::clearInstances()
{
    myInstances.clear();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


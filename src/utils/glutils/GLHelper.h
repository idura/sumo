#ifndef GLHelper_h
#define GLHelper_h
//---------------------------------------------------------------------------//
//                        GLHelper.h -
//  Some methods which help to draw certain geometrical objects in openGL
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// $Log$
// Revision 1.4  2005/09/15 12:18:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/07/02 09:46:28  dkrajzew
// some helper procedures for vss visualisation
//
// Revision 1.2  2003/10/02 14:55:58  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.1  2003/06/05 14:27:45  dkrajzew
// some helping functions added; Makefile added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <utility>
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GLHelper {
public:
    /// Draws a filled polygon described by the list of points
    static void drawFilledPoly(const Position2DVector &v, bool close);

    static void drawBoxLine(const Position2D &beg, double rot,
        double visLength, double width);

    static void drawLine(const Position2D &beg, double rot,
        double visLength);

    static void drawFilledCircle(double width, int steps=8);

    static void drawFilledCircle(double width, int steps,
        float beg, float end);

private:
    static std::vector<std::pair<float, float> > myCircleCoords;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


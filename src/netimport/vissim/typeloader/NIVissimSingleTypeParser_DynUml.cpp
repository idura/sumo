/***************************************************************************
                          NIVissimSingleTypeParser_DynUml.cpp

                             -------------------
    begin                : Wed, 18 Dec 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.6  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.3  2005/09/09 12:52:11  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.2  2005/04/26 07:54:05  dksumo
// level3 warnings patched; made containers non-static
//
// Revision 1.1.2.1  2005/04/15 09:56:30  dksumo
// level3 warnings removed; made the containers unstatic
//
// Revision 1.1  2004/10/22 12:50:04  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2003/08/18 12:39:23  dkrajzew
// missing handling of some vissim3.7-structures added
//
// Revision 1.3  2003/03/20 16:32:23  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 16:26:56  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
// Vissim import added (preview)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <vector>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_DynUml.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_DynUml::NIVissimSingleTypeParser_DynUml(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_DynUml::~NIVissimSingleTypeParser_DynUml()
{
}


bool
NIVissimSingleTypeParser_DynUml::parse(std::istream &from)
{
    std::vector<std::string> tmp;
    tmp.push_back("reisezeit");
    tmp.push_back("kante");
    readUntil(from, "kirchhoffexponent");
    string tag = readEndSecure(from, tmp);
    while(tag!="DATAEND") {
        tag = readEndSecure(from, tmp);
    }
    return true;
}


//---------------------------------------------------------------------------//
//                        NISUMOHandlerDepth.cpp -
//  A handler for deeper SUMO-import (connections and logics)
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:35  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/06/18 11:15:04  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 11:13:27  dkrajzew
// names changed
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
#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "NISUMOHandlerDepth.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


NISUMOHandlerDepth::NISUMOHandlerDepth(LoadFilter what)
    : SUMOSAXHandler("sumo-network"),
    _loading(what)
{
}


NISUMOHandlerDepth::~NISUMOHandlerDepth()
{
}


void
NISUMOHandlerDepth::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
}


void
NISUMOHandlerDepth::myCharacters(int element, const std::string &name,
                                 const std::string &chars)
{
}

void
NISUMOHandlerDepth::myEndElement(int element, const std::string &name)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



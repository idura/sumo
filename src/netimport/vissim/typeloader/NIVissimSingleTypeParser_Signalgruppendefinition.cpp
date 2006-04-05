/***************************************************************************
                          NIVissimSingleTypeParser_Signalgruppendefinition.cpp

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
// Revision 1.17  2006/04/05 05:32:27  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.16  2005/10/07 11:40:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.15  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.14  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.13  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.12  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.11  2003/07/07 08:29:54  dkrajzew
// Warnings are now reported to the MsgHandler
//
// Revision 1.10  2003/06/24 08:19:35  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.9  2003/06/18 11:35:30  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.8  2003/06/16 08:01:57  dkrajzew
// further work on Vissim-import
//
// Revision 1.7  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
// Revision 1.6  2003/04/16 09:59:53  dkrajzew
// further work on Vissim-import
//
// Revision 1.5  2003/04/09 15:53:24  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.4  2003/04/07 12:17:11  dkrajzew
// further work on traffic lights import
//
// Revision 1.3  2003/04/01 15:24:44  dkrajzew
// parsing of parking places patched
//
// Revision 1.2  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <cassert>
#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/DoubleVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgruppendefinition.h"

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
NIVissimSingleTypeParser_Signalgruppendefinition::NIVissimSingleTypeParser_Signalgruppendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Signalgruppendefinition::~NIVissimSingleTypeParser_Signalgruppendefinition()
{
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parse(std::istream &from)
{
    //
	int id;
    from >> id; // type-checking is missing!
    //
    string tag;
    tag = myRead(from);
    string name;
    if(tag=="name") {
        name = readName(from);
        tag = myRead(from);
    }
    //
    int lsaid;
    from >> lsaid;
    NIVissimTL *tl = NIVissimTL::dictionary(lsaid);
    if(tl==0) {
        MsgHandler::getErrorInstance()->inform("A traffic light group with an unknown traffic light occured.");
        MsgHandler::getErrorInstance()->inform("  Group-ID: " + toString<int>(id));
        MsgHandler::getErrorInstance()->inform("  TrafficLight-ID: " + toString<int>(lsaid));
        return false;
    }
    string type = tl->getType();
    if(type=="festzeit") {
        return parseFixedTime(id, name, lsaid, from);
    } if(type=="festzeit_fake") {
        return parseFixedTime(id, name, lsaid, from);
//        return parseExternFixedTime(id, name, lsaid, from);
    } if(type=="vas") {
        return parseVAS(id, name, lsaid, from);
    } if(type=="vsplus") {
        return parseVSPLUS(id, name, lsaid, from);
    } if(type=="trends") {
        return parseTRENDS(id, name, lsaid, from);
    } if(type=="vap") {
        return parseVAP(id, name, lsaid, from);
    } if(type=="tl") {
        return parseTL(id, name, lsaid, from);
    } if(type=="pos") {
        return parsePOS(id, name, lsaid, from);
    }
    MsgHandler::getErrorInstance()->inform("Unsupported LSA-Type '" + type + "' occured.");
    return false;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseFixedTime(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    //
    bool isGreenBegin;
    DoubleVector times;
    string tag = myRead(from);
    if(tag=="dauergruen") {
        isGreenBegin = true;
        from >> tag;
    } else if(tag=="dauerrot") {
        isGreenBegin = false;
        from >> tag;
    } else {
        // the first phase will be green
        isGreenBegin = true;
        while(tag=="rotende"||tag=="gruenanfang") {
            SUMOReal point;
            from >> point; // type-checking is missing!
            times.push_back(point);
            from >> tag;
            from >> point; // type-checking is missing!
            times.push_back(point);
            tag = myRead(from);
        }
    }
    //
    SUMOReal tredyellow, tyellow;
    from >> tredyellow;
    from >> tag;
    from >> tyellow;
    NIVissimTL::NIVissimTLSignalGroup *group =
        new NIVissimTL::NIVissimTLSignalGroup(
            lsaid, id, name, isGreenBegin, times, (SUMOTime) tredyellow, (SUMOTime) tyellow);
    if(!NIVissimTL::NIVissimTLSignalGroup::dictionary(lsaid, id, group)) {
        throw 1; // !!!
    }
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVAS(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    WRITE_WARNING("VAS traffic lights are not supported (lsa=" + toString<int>(lsaid) + ")");
    string tag;
    while(tag!="detektoren") {
        tag = myRead(from);
    }
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVSPLUS(
        int id, const std::string &, int lsaid, std::istream &)
{
    WRITE_WARNING("VSPLUS traffic lights are not supported (lsa="+ toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseTRENDS(
        int id, const std::string &, int lsaid, std::istream &)
{
    WRITE_WARNING("TRENDS traffic lights are not supported (lsa="+ toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVAP(
        int id, const std::string &, int lsaid, std::istream &)
{
    WRITE_WARNING("VAS traffic lights are not supported (lsa="+ toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseTL(
        int id, const std::string &, int lsaid, std::istream &)
{
    WRITE_WARNING("TL traffic lights are not supported (lsa="+ toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parsePOS(
        int id, const std::string &, int lsaid, std::istream &)
{
    WRITE_WARNING("POS traffic lights are not supported (lsa="+ toString<int>(lsaid) + ")");
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseExternFixedTime(
        int id, const std::string &, int lsaid, std::istream &)
{
    WRITE_WARNING("externally defined traffic lights are not supported (lsa="+ toString<int>(lsaid) + ")");
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



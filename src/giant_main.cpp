/***************************************************************************
                          main.cpp
              The main procedure for the conversion /
              building of networks
                             -------------------
    project              : SUMO
    subproject           : simulation
    begin                : Tue, 20 Nov 2001
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
// $Log: giant_main.cpp,v $
// Revision 1.16  2007/01/10 08:33:04  dkrajzew
// expanded the some option names when asking for them
//
// Revision 1.15  2006/12/12 12:15:24  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.14  2006/11/20 11:11:33  dkrajzew
// bug [ 1598346 ] (Versioning information in many places) patched - Version number is now read from windows_config.h/config.h
//
// Revision 1.13  2006/11/13 16:18:49  fxrb
// support for TCP/IP iodevices using DataReel library
//
// Revision 1.12  2006/08/01 07:19:56  dkrajzew
// removed build number information
//
// Revision 1.11  2006/04/18 08:21:48  dkrajzew
// beautifying: output consolidation
//
// Revision 1.10  2006/01/09 13:33:30  dkrajzew
// debugging error handling
//
// Revision 1.9  2005/11/30 08:56:49  dkrajzew
// final try/catch is now only used in the release version
//
// Revision 1.8  2005/11/29 13:42:03  dkrajzew
// added a minimum simulation speed definition before the simulation ends (unfinished)
//
// Revision 1.7  2005/11/15 10:15:49  dkrajzew
// debugging and beautifying for the next release
//
// Revision 1.6  2005/10/17 09:27:46  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.5  2005/10/07 11:48:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 12:55:27  dkrajzew
// build number output added
//
// Revision 1.1  2004/11/23 10:45:07  dkrajzew
// netedit by A. Gaubatz added
//
// Revision 1.1  2004/11/22 13:02:17  dksumo
// 'netedit' and 'giant' added
//
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

#include <ctime>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <fx.h>
#include <fx3d.h>
#include <microsim/MSNet.h>
#include <microsim/MSEmitControl.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIThreadFactory.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <gui/drawerimpl/GUIVehicleDrawer.h>
#include <utils/gui/div/GUIFrame.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

// conditionally add library search record to object file for DataReel
#ifdef USE_SOCKETS
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment (lib, "gxcoded.lib")
#else // _DEBUG
#pragma comment (lib, "gxcode.lib")
#endif // _DEBUG
#endif // _WIN32
#endif // USE_SOCKETS

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * methods
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * options initialisation
 * ----------------------------------------------------------------------- */
void
fillOptions(OptionsCont &oc)
{
    // give some application descriptions
    oc.setApplicationDescription("Graphical network editor and simulation.");
#ifdef WIN32
    oc.setApplicationName("guisim.exe");
#else
    oc.setApplicationName("sumo-guisim");
#endif
    oc.addCallExample("");
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    oc.addOptionSubTopic("Configuration");
    oc.addOptionSubTopic("Process");
    oc.addOptionSubTopic("Visualisation");
    oc.addOptionSubTopic("Open GL");
    oc.addOptionSubTopic("Report");


    // insert options
    GUIFrame::fillInitOptions(oc);
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
#ifndef _DEBUG
    try {
#endif
        int init_ret = SystemFrame::init(true, argc, argv, fillOptions);
        if(init_ret<0) {
            cout << "SUMO giant" << endl;
            cout << " (c) DLR/ZAIK 2000-2007; http://sumo.sourceforge.net" << endl;
            cout << " Version " << VERSION << endl;
            switch(init_ret) {
            case -2:
                OptionsSubSys::getOptions().printHelp(cout);
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0||!GUIFrame::checkInitOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // Make application
        FXApp application("SUMO GUINetEdit","DLR+ZAIK");
        gFXApp = &application;
        // Open display
        application.init(argc,argv);
        OptionsCont &oc = OptionsSubSys::getOptions();
        int minor, major;
        if(!FXGLVisual::supported(&application, major, minor)) {
            MsgHandler::getErrorInstance()->inform("This system has no OpenGL support. Exiting." );
            throw ProcessError();
        }
        // initialise global settings
        gQuitOnEnd = oc.getBool("quit-on-end");
        gAllowAggregatedFloating = oc.getBool("allow-floating-aggregated-views");
        gAllowAggregated = !oc.getBool("disable-aggregated-views");
        gAllowTextures = !oc.getBool("disable-textures");
        gSuppressEndInfo = oc.getBool("surpress-end-info");

        // build the main window
        GUIThreadFactory tf;
        GNEApplicationWindow * window =
            new GNEApplicationWindow(&application, tf,
                oc.getInt("w"), oc.getInt("h"),
                oc.getString("configuration-file"));
		gGradients = new GUIGradientStorage(window);
        // delete startup-options
        OptionsSubSys::close();
        // Create app
        application.addSignal(SIGINT,window, MID_QUIT);
        application.create();
        // Run
        ret = application.run();
#ifndef _DEBUG
    } catch(...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }
#endif
    SystemFrame::close();
    return ret;
}




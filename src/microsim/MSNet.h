/***************************************************************************
                          MSNet.h  -  We will simulate on this object.
                          Holds all necessary objects for micro-simulation.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

#ifndef MSNet_H
#define MSNet_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.5  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.4  2002/10/18 11:49:32  dkrajzew
// usage of MeanData rechecked for closing of the generated files and the destruction of allocated ressources
//
// Revision 1.3  2002/10/17 10:45:17  dkrajzew
// preinitialisation added; errors due to usage of local myStep instead of instance-global myStep patched
//
// Revision 1.2  2002/10/16 16:44:23  dkrajzew
// globa file include; no usage of MSPerson; single step execution implemented
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.13  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.12  2002/08/06 15:40:34  roessel
// Default constructor needs to be (dummy) implemented.
//
// Revision 1.11  2002/08/06 14:13:27  roessel
// New method preInit() and changes in init().
//
// Revision 1.10  2002/07/31 17:33:01  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/30 15:17:47  croessel
// Made MSNet-class a singleton-class.
//
// Revision 1.9  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.8  2002/05/14 07:53:09  dkrajzew
// Windows eol removed
//
// Revision 1.7  2002/05/14 07:45:21  dkrajzew
// new _SPEEDCHECK functions: all methods in MSNet, computation of UPS and MUPS
//
// Revision 1.6  2002/04/17 10:44:13  croessel
// (Windows) Carriage returns removed.
//
// Revision 1.5 2002/04/15 07:38:52 dkrajzew
// Addition of routes and detectors removed; a static information
// about the current time step (globaltime) implemented; output
// computation is now only invoked when needed
//
// Revision 1.4  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.3  2002/04/11 10:33:25  dkrajzew
// Addition of detectors added
//
// Revision 1.2  2002/04/10 16:19:34  croessel
// Modifications due to detector-implementation.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.2  2002/03/14 08:09:26  traffic
// Option for no raw output added
//
// Revision 2.1  2002/03/07 07:55:02  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/13 16:30:54  croessel
// Output goes in a file now.
//
// Revision 1.13  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.12  2002/01/16 10:03:34  croessel
// New method "static double deltaT()" and member "static double myDeltaT"
// added. DeltaT is the length of a timestep in seconds.
//
// Revision 1.11  2001/12/20 14:35:54  croessel
// using namespace std replaced by std::
//
// Revision 1.10  2001/12/19 16:36:25  croessel
// Moved methods (assignment, copy-ctor) to private and removed outcommented
// code.
//
// Revision 1.9  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.8  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.7  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.6  2001/10/23 09:31:29  traffic
// parser bugs removed
//
// Revision 1.4  2001/09/06 15:39:12  croessel
// Added simple text output to simulation-loop.
//
// Revision 1.3  2001/07/16 16:00:52  croessel
// Changed Route-Container type to map<string, Route*>. Added static dictionary
// methods to access it (same as id-handling).
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
//#include "MSPerson.h"
#ifdef _SPEEDCHECK
#include <ctime>
#endif


class MSEdge;
class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSEventControl;
class Event;
class MSDetector;
class PreStartInitialised;

/**
 */
class MSNet
{
    friend class GUINet;

public:
    /** Get a pointer to the unique instance of MSNet (singleton).
     * @return Pointer to the unique MSNet-instance.
     */
    static MSNet* getInstance( void );

    /// Type for time (seconds).
    typedef unsigned int Time;

    /// Detector-container type.
    typedef std::vector< MSDetector* > DetectorCont;

    /// List of times (intervals or similar)
    typedef std::vector< Time > TimeVector;

    /** Create unique instance of MSNet and initialize with the
     * beginning timestep. To finish the initialization call &ref
     * init.
     * @param startTimestep Timestep the simulation will start
     * with.
     */
    static void preInit( Time startTimestep );

    /** Initialize the unique MSNet-instance after creation in @ref
     * preInit.
     */
    static void init( std::string id,
                      MSEdgeControl* ec,
                      MSJunctionControl* jc,
                      MSEmitControl* emc,
                      MSEventControl* evc,
                      DetectorCont* detectors,
                      TimeVector dumpMeanDataIntervalls,
                      std::string baseNameDumpFiles,
                      bool withGUI );
    
    /// Destructor.
    ~MSNet();

    /** Simulates from timestep start to stop. start and stop in
        timesteps.  In each timestep we emit Vehicles, move Vehicles,
        the Vehicles change Lanes.  The method returns true when the
        simulation could be finished without errors, otherwise
        false. */
    bool simulate( std::ostream *craw, Time start, Time stop );

    void simulationStep( std::ostream *craw, Time start, Time step);

    /** Inserts a MSNet into the static dictionary and returns true if
        the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSNet* net );

    /** Returns the MSNet associated to the key id if exists,
        otherwise returns 0. */
    static MSNet* dictionary( std::string id );

    /** Clears the dictionary */
    static void clear();

    /// Returns the timestep-length in seconds.
    static double deltaT();

    /** Returns the current simulation time in seconds. Current means
        start-time plus runtime. */
    double simSeconds();

    /** Returns the current timestep. */
    Time timestep( void );

    /** Returns the number of unique mean-data-dump-intervalls. In
        vehicles and lanes you will need one element more for the
        GUI-dump. */
    unsigned getNDumpIntervalls( void );

    /** Returns wether we are using a GUI or not. The use of a GUI
        increases the elements of a meanData container. */
    bool withGUI( void );
    
    void addPreStartInitialisedItem(PreStartInitialised *preinit);

    void preStartInit();


#ifdef _DEBUG
    /** a visible variables for the current time step - for debugging
        purposes only */
    static Time globaltime;
#endif

#ifdef _SPEEDCHECK
    /** the number of vehicles moved */
    static long noVehicles;
    /** the begin of the speed measurement in s */
    static time_t begin;
    /** the end of the speed measurement in s */
    static time_t end;
#endif

protected:

//      /// Use this constructor only.
//      MSNet( std::string id,
//             MSEdgeControl* ec,
//             MSJunctionControl* jc,
//             MSEmitControl* emc,
//             MSEventControl* evc,
//             DetectorCont* detectors,
//             std::vector< Time > dumpMeanDataIntervalls,
//             std::string baseNameDumpFiles,
//             bool withGUI );

private:
    /// Copy constructor.
    MSNet( const MSNet& );

    /// Assignment operator.
    MSNet& operator=( const MSNet& );

    /// Unique instance of MSNet
    static MSNet* myInstance;

    /// Unique ID.
    std::string myID;

    /** Lane-changing is done by this object. */
    MSEdgeControl* myEdges;

    /// Sets the right-of-way rules and moves first cars.
    MSJunctionControl* myJunctions;

    /// Emits cars into the lanes.
    MSEmitControl* myEmitter;

    /** Time-dependant events like traffic-light-changes, output
        generation etc. */
    MSEventControl* myEvents;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSNet* > DictType;
    static DictType myDict;

    /// Container for items to initialise before starting
    typedef std::vector<PreStartInitialised*> PreStartVector;
    PreStartVector myPreStartInitialiseItems;

    /// Timestep [sec]
    static double myDeltaT;

    /// Current time step.
    Time myStep;

    /// Container of detectors.
    DetectorCont* myDetectors;

    /// The Net's meanData is a pair of an interval-length and a filehandle.
    class MeanData 
    {
    public:
        MeanData( Time t, std::ofstream* of ) 
            : interval( t ),
              file( of )
        { 
            (*file) << "<netstats>" << std::endl;
        }

        ~MeanData()
        {
            (*file) << "</netstats>" << std::endl;
            file->close();
        }

        Time interval;
        std::ofstream* file;
    };
    
    /** List of intervals and filehandles. At the end of each intervall
        the mean data (flow, density, speed ...) of each lane is calculated
        and written to file. */
    std::vector< MeanData* > myMeanData;

    /// Indicates if we are using a GUI.
    bool myWithGUI;
    
    /** Last timestep when mean-data was send to GUI. We need it to
     * calculate the intervall which may be not const for the GUI. */
    Time myLastGUIdumpTimestep;
    

    /** Default constructor. It makes no sense to build a net without
        initialisation. */
    MSNet(){};
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#ifndef DISABLE_INLINE
#include "MSNet.icc"
#endif

#endif

// Local Variables:
// mode:C++


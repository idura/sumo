#ifndef MSEmitControl_H
#define MSEmitControl_H
/***************************************************************************
                          MSEmitControl.h  -  Controls emission of
                          vehicles into the net.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:39:02  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:14  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.9  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.8  2001/12/19 17:01:22  croessel
// using namespace std replaced by std::
// Copy-ctor and assignment-operator moved to private.
//
// Revision 1.7  2001/12/06 13:11:22  traffic
// minor change
//
// Revision 1.6  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.5  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.4  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.3  2001/10/22 12:42:57  traffic
// single person simulation added
//
// Revision 1.2  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSVehicleContainer.h"
#include <vector>
#include <map>
#include <string>
#include "MSNet.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSEmitControl
 * A vehicle emitter; Holds a list of vehicles which may be extended by new
 * vehicles read by MSRouteLoaders. Tries to emit vehicles departing at a time
 * into the network when this time is reached and restores them when the emission
 * fails.
 * Otherwise, the control is given to the lanes.
 */
class MSEmitControl
{
public:
    /** Use this constructor only. It will sort the vehicles by their
        departure time. */
    MSEmitControl( std::string id);

    /// Destructor.
    ~MSEmitControl();

    /** @brief Emits vehicles at time, if which want to depart at this.
        If emission is not possible, the vehicles remain in the list.
		Returns the number of emitted vehicles */
    size_t emitVehicles( MSNet::Time time );

    /** @brief Adds a single vehicle for departure */
    void add( MSVehicle *veh );

    /// adds a list of vehicles to the container
    void moveFrom( MSVehicleContainer &cont );

    /** @brief Inserts emitcontrol into the static dictionary
        Returns true if the key id isn't already in the dictionary (the control
        is not added then). Otherwise returns false. */
    static bool dictionary( std::string id, MSEmitControl* emitControl );

    /** Returns the MSEdgeControl associated to the key id if exists,
        otherwise returns 0. */
    static MSEmitControl* dictionary( std::string id );

    /** Clears the dictionary */
    static void clear();

private:
    /** @brief Tries to emit the vehicle
        If the emission fails, the vehicle is inserted into the given
        container.
		Returns the number of emitted vehicles */
    size_t tryEmit(MSVehicle *veh,
        MSVehicleContainer::VehicleVector &refusedEmits);

    /** Moves all vehicles which should have been emitted previously to the given time
        into the container of previously refused vehicles */
    void checkPrevious(MSNet::Time time);

private:
    /// Unique ID.
    std::string myID;

    /** @brief The entirety of loaded vehicles that will drive through the net.
        The vehicles know their departure-time and route. The container
        is sorted by the vehicles departure time. */
    MSVehicleContainer myAllVeh;

    /// Definition of a static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSEmitControl* > DictType;

    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;

    /** Buffer#1 for vehicles that were not allowed to enter their lane. */
    MSVehicleContainer::VehicleVector myRefusedEmits1;

    /** Buffer#1 for vehicles that were not allowed to enter their lane. */
    MSVehicleContainer::VehicleVector myRefusedEmits2;

    /// Default constructor.
    MSEmitControl();

    /// Copy constructor.
    MSEmitControl(const MSEmitControl&);

    /// Assignment operator.
    MSEmitControl& operator=(const MSEmitControl&);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSEmitControl.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

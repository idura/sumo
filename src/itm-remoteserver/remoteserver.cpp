/****************************************************************************/
/// @file    remoteserver.cpp
/// @author  Thimor Bohn <bohn@itm.uni-luebeck.de>
/// @date    2007/03/13
/// @version $Id$
///
/// socket server user to control sumo by remote client
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// RemoteServer.cpp
// @author: Thimor Bohn <bohn@itm.uni-luebeck.de
// ===========================================================================
// compiler pragmas
// ===========================================================================


// ===========================================================================
// included modules
// ===========================================================================
#include "remoteconstants.h"
#include "remoteserver.h"
#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "utils/common/SUMOTime.h"
#include "microsim/MSNet.h"
#include "microsim/MSVehicleControl.h"
#include "microsim/MSVehicle.h"
#include "utils/geom/Position2D.h"
#include "microsim/MSEdge.h"
#include "microsim/MSRouteHandler.h"
#include "microsim/MSRouteLoaderControl.h"
#include "microsim/MSRouteLoader.h"

#include <string>
#include <map>
#include <iostream>
#include <cstdlib>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
namespace itm
  {
  /*****************************************************************************/

  RemoteServer::RemoteServer(int port, SUMOTime endTime, float penetration, string routeFile)
      : port_(port), endTime_(endTime), penetration_(penetration), isMapChanged_(true),
      routeFile_(routeFile), numEquippedVehicles_(0)
  {}

  /*****************************************************************************/

  RemoteServer::~RemoteServer()
  {}

  /*****************************************************************************/

  void
  RemoteServer::run()
  {
    try
      {
        Socket socket(port_);
        socket.accept();

        bool running = true;

        while (running)
          {
            Storage in;
            Storage out;
            socket.receiveExact(in);
            int N = in.readInt(); // number of commands
            for (int i=0; i<N; ++i)
              {
                unsigned char length = in.readChar(); // command length
                unsigned char cmd = in.readChar(); // command code
		
                switch (cmd)
                  { // parse command code

                  case CMD_SIMSTEP:
                    simStep(in, out, length);
                    break;

                  case CMD_SETMAXSPEED:
                    setMaximumSpeed(in, out, length);
                    break;

                  case CMD_CLOSE:
                    running = false;
                    break;

                  default:
                    running = false;
                    out.writeChar(RTYPE_ERR);
                    out.writeString("unknown command received. sumo shut down.");
                    cerr << "unkown command received. sumo shut down" << endl;
                    break;
                  }

                socket.sendExact(out);
              }
          }
      }
    catch (RemoteException e)
      {
        cerr << e.what() << endl;
      }
    catch (SocketException e)
      {
        cerr << e.what() << endl;
      }
  }

  /*****************************************************************************/

  void
  RemoteServer::simStep(tcpip::Storage &in, tcpip::Storage &out, int length)
  throw (RemoteException)
  {
    // prepare out
    out.reset();
    // check parameters
    int targetTime = in.readInt();
    if (targetTime > endTime_)
      {
        targetTime = endTime_;
      }
    unsigned char rtype = in.readChar();

    // do simulation step
    MSNet *net = MSNet::getInstance();
    SUMOTime currentTime    = net->getCurrentTimeStep();
    SUMOTime targetTimeStep = static_cast<SUMOTime>(targetTime);
    if (targetTimeStep - currentTime > 0)
      {
        net->simulate(currentTime, targetTimeStep);
        isMapChanged_ = true;
      }

    // prepare output
    try
      {
        // map containing all active equipped vehicles. maps external id to MSVehicle*
        map<int, const MSVehicle*> activeEquippedVehicles;
        // get access to all vehicles in simulation
        MSVehicleControl &vehControl = net->getVehicleControl();
        // iterate over all vehicles in simulation
        for (map<string, MSVehicle*>::const_iterator iter = vehControl.loadedVehBegin(); iter != vehControl.loadedVehEnd(); ++iter)
          {
            // selected vehicle
            const string vehicleId   = (*iter).first;
            const MSVehicle *vehicle = (*iter).second;
            // insert into equippedVehicleId if not contained
            if (equippedVehicles_.find(vehicleId) == equippedVehicles_.end())
              {
                // determine if vehicle is equipped
                double rnd = double(rand())/RAND_MAX;
                if (rnd <= penetration_)
                  {
                    // vehicle is equipped
                    equippedVehicles_[vehicleId] = numEquippedVehicles_++;
                  }
                else
                  {
                    // vehicle is not equipped
                    equippedVehicles_[vehicleId] = -1;
                  }
              }
            if (equippedVehicles_[vehicleId] >= 0)
              {
                int extId = equippedVehicles_[vehicleId];
                activeEquippedVehicles[extId] = vehicle;
                // vehicle is equipped
              }
          }

        out.writeChar( static_cast<unsigned char>(rtype) );
        out.writeInt(numEquippedVehicles_);
        // iterate over all active equipped vehicles
        for (map<int, const MSVehicle*>::iterator iter = activeEquippedVehicles.begin(); iter != activeEquippedVehicles.end(); ++iter)
          {
            int extId = (*iter).first;
            out.writeInt(extId);
            const MSVehicle* vehicle = (*iter).second;
            if (rtype == RTYPE_XY)
              {
                Position2D pos = vehicle->getPosition();
                out.writeFloat(pos.x());
                out.writeFloat(pos.y());
                out.writeFloat(vehicle->getSpeed());
              }
            if (rtype == RTYPE_REL)
              {
                out.writeString(vehicle->getEdge()->getID());
                out.writeFloat(vehicle->getPositionOnLane());
              }
          }

        if (isMapChanged_)
          {
            isMapChanged_ = false;
            ext2intId.clear();
            for (map<std::string, int>::iterator iter = equippedVehicles_.begin(); iter != equippedVehicles_.end(); ++iter)
              {
                std::string intId = (*iter).first;
                int extId = (*iter).second;
                ext2intId[extId] = intId;
              }
          }

      }
    catch (...)
      {
        out.writeChar(RTYPE_ERR);
        out.writeString("some error happen in command: simulation step. "
                        "sumo shut down");
        throw new RemoteException("some error happen in command: simulation step.");
        return;
      }

    return;
  }
  /*****************************************************************************/

  void
  RemoteServer::setMaximumSpeed(tcpip::Storage &in, tcpip::Storage &out, int length)
  throw (RemoteException)
  {
    cout << "x***" << endl;
    // prepare out
    out.reset();
    
    int extId = in.readInt(); // external node id (equipped vehicle number)
    cout << "extId = " << extId << endl;
    float maxspeed = in.readFloat();
    cout << "maxspeed = " << maxspeed << endl;
    std::string intId = ext2intId[extId];
    cout << "intId = " << intId << endl;
    MSVehicle *veh = MSNet::getInstance()->getVehicleControl().getVehicle( intId );
    if ( veh != NULL )
      {
        if (maxspeed>=0)
          {
	    cout << "maxspeed>=0" << endl;
            veh->setIndividualMaxSpeed( maxspeed );
          }
        else
          {
	    cout << "maxspeed<0" << endl;
            veh->unsetIndividualMaxSpeed();
          }
      } else {
        cout << "(veh == NULL)" << endl;
      }
    cout << "***x" << endl;

    return;
  }

  /*****************************************************************************/
}

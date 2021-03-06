/****************************************************************************/
/// @file    RONode.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for nodes used by the router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RONode_h
#define RONode_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Named.h>
#include <utils/geom/Position2D.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONode
 * @brief Base class for nodes used by the router
 */
class RONode : public Named {
public:
    /** @brief Constructor
     * @param[in] id The id of the node
     */
    RONode(const std::string &id) throw();


    /// @brief Destructor
    ~RONode() throw();


    /** @brief Sets the position of the node
     * @param[in] p The node's position
     */
    void setPosition(const Position2D &p) throw();


    /** @brief Returns the position of the node
     * @return This node's position
     */
    const Position2D &getPosition() throw() {
        return myPosition;
    }


private:
    /// @brief This node's position
    Position2D myPosition;


private:
    /// @brief Invalidated copy constructor
    RONode(const RONode &src);

    /// @brief Invalidated assignment operator
    RONode &operator=(const RONode &src);

};


#endif

/****************************************************************************/


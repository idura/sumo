#ifndef NGNet_h
#define NGNet_h
/***************************************************************************
                          NGNet.h
                             -------------------
    project              : SUMO
    begin                : Mar, 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
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
// Revision 1.4  2005/09/15 12:03:17  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 11:48:51  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.2  2003/07/21 11:05:31  dkrajzew
// patched some bugs found in first real-life execution
//
// Revision 1.1  2003/07/16 15:33:08  dkrajzew
// files needed to generate networks added
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

#include "NGNetElements.h"


class NBNetBuilder;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class TNGNet
{
public:
    /// Constructor
	TNGNet(NBNetBuilder &nb);

    /// Destructor
	~TNGNet();

    /// Returns the node at the given position
	TNode* FindNode(int xID, int yID);

    /// returns next ID and increases myLastID
	std::string GetID();

	float RadialToX(float radius, float phi);
	float RadialToY(float radius, float phi);

	/// creates NGNetwork in chequerboard pattern
	void CreateChequerBoard(int NumX, int NumY, float SpaceX, float SpaceY);

	/** @brief creates NGNetwork in spiderweb pattern
	    NumRadDiv >= 3
	    NumCircles >= 1 */
	void CreateSpiderWeb(int NumRadDiv, int NumCircles, float SpaceRad);

	/// save
	void SaveNet(char* FileName); // saves NGNetwork to file

    /// Transfer to netbuild-structures
    void toNB() const;


    /// list of nodes
	TNodeList NodeList;

    /// list of links
	TLinkList LinkList;

private:
    /// Connects both noes with two edges, one for each direction
    void connect(TNode *node1, TNode *node2);

private:
    /// last ID given to node or link
	int myLastID;

    NBNetBuilder &myNetBuilder;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

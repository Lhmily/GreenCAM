/*
 * Base.h
 *
 *  Created on: Sep 4, 2014
 *      Author: Lhmily
 */

#ifndef BASE_H_
#define BASE_H_
#include "Node.h"
#include "TcamNode.h"

bool NodeDstComp(Node i, Node j);
bool NodeOrderComp(Node i, Node j);
bool TcamNodeComp(Tcam_Node i, Tcam_Node j);
#endif /* BASE_H_ */

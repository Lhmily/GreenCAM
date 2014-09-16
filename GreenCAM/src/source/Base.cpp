/*
 * Base.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: Lhmily
 */
#include "../header/Base.h"

bool NodeDstComp(Node i, Node j) {
	return i.getDst() < j.getDst();
}

bool NodeOrderComp(Node i, Node j) {
	return i.getOrder() < j.getOrder();
}

bool TcamNodeComp(Tcam_Node i, Tcam_Node j) {
	return i.getNj() < j.getNj();
}

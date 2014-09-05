/*
 * Base.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: Lhmily
 */
#include "../header/Base.h"
bool CompressElementDstComp(CompressElement i, CompressElement j) {
	return i.getDst() < j.getDst();
}

bool CompressElementOrderDstComp(CompressElement i, CompressElement j) {
	return i.getOrder() < j.getOrder();
}

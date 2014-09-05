/*
 * CompressElement.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: Lhmily
 */

#include "../header/CompressElement.h"

CompressElement::CompressElement() {
	// TODO Auto-generated constructor stub

}

size_t CompressElement::getDst() const {
	return _dst;
}

void CompressElement::setDst(size_t dst) {
	_dst = dst;
}

const string& CompressElement::getDstCode() const {
	return _dst_code;
}

void CompressElement::setDstCode(const string& dstCode) {
	_dst_code = dstCode;
}

size_t CompressElement::getOrder() const {
	return _order;
}

void CompressElement::setOrder(size_t order) {
	_order = order;
}

size_t CompressElement::getSrc() const {
	return _src;
}

void CompressElement::setSrc(size_t src) {
	_src = src;
}

const string& CompressElement::getSrcCode() const {
	return _src_code;
}

void CompressElement::setSrcCode(const string& srcCode) {
	_src_code = srcCode;
}

CompressElement::~CompressElement() {
	// TODO Auto-generated destructor stub
}


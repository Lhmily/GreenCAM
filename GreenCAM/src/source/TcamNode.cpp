/*
 * TcamNode.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: Lhmily
 */

#include "../header/TcamNode.h"

Tcam_Node::Tcam_Node() {
	// TODO Auto-generated constructor stub

}

Tcam_Node::~Tcam_Node() {
	// TODO Auto-generated destructor stub
}

size_t Tcam_Node::getBits() const {
	return _bits;
}

void Tcam_Node::setBits(size_t bits) {
	_bits = bits;
}

size_t Tcam_Node::getBlockNum() const {
	return _block_num;
}

void Tcam_Node::setBlockNum(size_t blockNum) {
	_block_num = blockNum;
}

size_t Tcam_Node::getBlockSize() const {
	return _block_size;
}

void Tcam_Node::setBlockSize(size_t blockSize) {
	_block_size = blockSize;
}

float Tcam_Node::getNj() const {
	return _nj;
}

void Tcam_Node::setNj(float nj) {
	_nj = nj;
}

float Tcam_Node::getNs() const {
	return _ns;
}

void Tcam_Node::setNs(float ns) {
	_ns = ns;
}

/*
 * Node.cpp
 *
 *  Created on: Jul 20, 2014
 *      Author: Lhmily
 */

#include "../header/Node.h"

Node::Node() {
	// TODO Auto-generated constructor stub
	this->_parent = NULL;
	this->_state = 0;
	this->_code_mask = 0;
}

Node::~Node() {
	// TODO Auto-generated destructor stub
}

const vector<size_t>& Node::getAcceptRules() const {
	return _accept_rules;
}

void Node::setAcceptRules(const vector<size_t>& acceptRules) {
	_accept_rules = acceptRules;
}

const vector<Node*>& Node::getChild() const {
	return _child;
}

void Node::setChild(const vector<Node*>& child) {
	_child = child;
}

size_t Node::getCodeMask() const {
	return _code_mask;
}

void Node::setCodeMask(size_t codeMask) {
	_code_mask = codeMask;
}

size_t Node::getDepth() const {
	return _depth;
}

void Node::setDepth(size_t depth) {
	_depth = depth;
}

size_t Node::getDst() const {
	return _dst;
}

void Node::setDst(size_t dst) {
	_dst = dst;
}

const string& Node::getDstCode() const {
	return _dst_code;
}

void Node::setDstCode(const string& dstCode) {
	_dst_code = dstCode;
}

size_t Node::getOrder() const {
	return _order;
}

void Node::setOrder(size_t order) {
	_order = order;
}

const Node* Node::getParent() const {
	return _parent;
}

void Node::setParent(Node* parent) {
	_parent = parent;
}

const string& Node::getSrcCode() const {
	return _src_code;
}

void Node::setSrcCode(const string& srcCode) {
	_src_code = srcCode;
}

size_t Node::getState() const {
	return _state;
}

void Node::setState(size_t state) {
	_state = state;
}

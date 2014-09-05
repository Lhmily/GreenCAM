/*
 * Node.h
 *
 *  Created on: Jul 20, 2014
 *      Author: Lhmily
 */

#ifndef NODE_H_
#define NODE_H_
#include <string>
#include <vector>
using namespace std;

class Node {

private:
	size_t _state;
	size_t _depth;
	size_t _code_mask;
	vector<size_t> _accept_rules;

	string _src_code;
	string _dst_code;

	size_t _order;

	vector<Node *> _child;
	Node *_parent;
public:
	friend class Compress;
	friend class TransTable;
	Node();
	virtual ~Node();

};

#endif /* NODE_H_ */

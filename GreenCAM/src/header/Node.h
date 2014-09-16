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
	size_t _dst;
	size_t _depth;
	size_t _code_mask;
	vector<size_t> _accept_rules;

	string _src_code;
	string _dst_code;

	size_t _order;

	vector<Node *> _child;
	vector<size_t> _child_index;
	Node *_parent;
	size_t _parent_index;

	size_t _child_count;
	bool _is_block_root;
public:
	friend class Compress;
	friend class TransTable;
	Node();
	virtual ~Node();
	const vector<size_t>& getAcceptRules() const;
	void setAcceptRules(const vector<size_t>& acceptRules);
	const vector<Node*>& getChild() const;
	void setChild(const vector<Node*>& child);
	size_t getCodeMask() const;
	void setCodeMask(size_t codeMask);
	size_t getDepth() const;
	void setDepth(size_t depth);
	size_t getDst() const;
	void setDst(size_t dst);
	const string& getDstCode() const;
	void setDstCode(const string& dstCode);
	size_t getOrder() const;
	void setOrder(size_t order);
	const Node* getParent() const;
	void setParent(Node* parent);
	const string& getSrcCode() const;
	void setSrcCode(const string& srcCode);
	size_t getState() const;
	void setState(size_t state);
};

#endif /* NODE_H_ */

/*
 * Compress.h
 *
 *  Created on: Jul 20, 2014
 *      Author: Lhmily
 */

#ifndef COMPRESS_H_
#define COMPRESS_H_
#include "dfa.h"
#include "Node.h"
#include "stdinc.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "CompressElement.h"
#include "Base.h"

using namespace std;

class Compress {
private:
	DFA *_dfa;
	Node *_tree;
	Node *_root;
	size_t _state_size;

	//
	vector<vector<CompressElement> > _compress_table;

private:
	size_t calculate_code_mask(Node *node);
	void reorder_child(Node *node);
	void alloc_code(vector<string> &result, int required_size,
			const vector<size_t>&len_list, string &str_temp, bool left,
			size_t layer);
	void set_tree_order(Node *node, size_t &order);
public:
	Compress();
	virtual ~Compress();

	void init(DFA *dfa);

	void build_tree();

	void encode_tree(Node *node = NULL, string pre_str = "");

	void print_tree_dot(ofstream &fout) const;

	void print_compress_table(ofstream &fout, vector<vector<size_t> > &header);

	size_t isCover(size_t state_1, size_t state_2);

	friend class TransTable;
};

#endif /* COMPRESS_H_ */

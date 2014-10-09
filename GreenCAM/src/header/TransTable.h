/*
 * TransTable.h
 *
 *  Created on: Jul 21, 2014
 *      Author: Lhmily
 */

#ifndef TRANSTABLE_H_
#define TRANSTABLE_H_
#include <vector>
#include "Compress.h"
#include <cmath>

class TransTable {
private:

	state_t **_table;
	DFA *_dfa;
	Compress _compress;

	size_t _state_size;
	vector<vector<size_t> > _header;

	state_t **_table_2;

	size_t _column_2_size;

public:
	TransTable(DFA *dfa);
	virtual ~TransTable();
	void build_encode_tree();

	void character_compress();
	void prefix_compress();

	void print_tree_dot();

	void print_rebuild_tree_dot();

	void print_compress_table();

	void print();

	void generate_table_2();

	void print_table_2();

	void prefix_compress_2();

	void print_compress_table_2();

	void print_2();

private:
	void rebuild_tree(vector<Node> &list);

	void compress_one_table(Node *compress_temp, vector<Node> &ret);
};

#endif /* TRANSTABLE_H_ */

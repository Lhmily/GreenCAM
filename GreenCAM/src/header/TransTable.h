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

class TransTable {
private:

	state_t **_table;
	DFA *_dfa;
	Compress _compress;

	size_t _state_size;
	vector<vector<size_t> > _header;

public:
	TransTable(DFA *dfa);
	virtual ~TransTable();
	void build_encode_tree();

	void character_compress();
	void prefix_compress();

	void print_tree_dot();

	void print_compress_table();
};

#endif /* TRANSTABLE_H_ */

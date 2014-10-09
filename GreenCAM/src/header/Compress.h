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
#include <cmath>
#include <array>
#include "TcamNode.h"
#include "AsciiNode.h"

#include "Base.h"
extern "C" {
#include "tcam-power.h"
}
using namespace std;

class Compress {
private:
	DFA *_dfa;
	Node *_tree;
	Node *_root;
	size_t _state_size;

	//
	vector<vector<Node> > _compress_table;

	size_t _block_size;

	vector<vector<size_t> > _ascii_blocks;

	//tcam structs
	struct parameters tcam_parameters;
	struct search_power tcam_search_power;
	struct static_power_worst_case tcam_leakage_power;
	struct read_power tcam_read_power;
	struct write_power tcam_write_power;
	struct search_delay tcam_search_delay;

	vector<vector<Node> > _compress_table_2;

private:
	size_t calculate_code_mask(Node *node);
	void reorder_child(Node *node);
	void alloc_code(vector<string> &result, int required_size,
			const vector<size_t>&len_list, string &str_temp, bool left,
			size_t layer);
	void set_tree_order(Node *node, size_t &order);

	void tcam_parameters_init(size_t block_size, uint32_t bits,
			uint32_t block_num);

	size_t calculate_compress_tree_child_num(size_t i, size_t j);

	size_t get_state_split_block(size_t i, size_t j, size_t block_size);

	void state_split_tree();

	void state_split_block(size_t block_size);

	string state_convert_code(size_t s, const int bits) const;

	void print_state_split_block_detail(ofstream &fout, const size_t i,
			const size_t j, const size_t root);

public:
	Compress();
	virtual ~Compress();

	void init(DFA *dfa);

	void build_tree();

	void encode_tree(Node *node = NULL, string pre_str = "");

	void print_tree_dot(ofstream &fout) const;

	void print_compress_tree_dot(ofstream &fout, int index);

	void print_compress_table(ofstream &fout, vector<vector<size_t> > &header);

	void print_compress_table_2(ofstream &fout);

	void print_original_tcam(ofstream &fout, size_t block_size, uint32_t bits,
			uint32_t block_num);

	void print_charater_index_tcam(ofstream &fout, size_t block_size,
			uint32_t bits, uint32_t block_num);

	void print_transition_merge_tcam(ofstream &fout, size_t block_size,
			vector<vector<size_t> > &header);

	void print_state_split_tcam(ofstream &tcam_fout, ofstream &fout,
			size_t block_size, vector<vector<size_t> > &header);

	size_t isCover(size_t state_1, size_t state_2);

	friend class TransTable;
};

#endif /* COMPRESS_H_ */

/*
 * TransTable.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: Lhmily
 */

#include "../header/TransTable.h"
using namespace std;

TransTable::TransTable(DFA *dfa) :
		_dfa(dfa) {
	// TODO Auto-generated constructor stub
	this->_compress.init(_dfa);
	_table = _dfa->get_state_table();
	this->_state_size = _dfa->size();
}

TransTable::~TransTable() {
	// TODO Auto-generated destructor stub
}

void TransTable::build_encode_tree() {
	cout << endl << endl << "Building the tree...";
	_compress.build_tree();
	cout << endl << endl << "Encoding the tree...";
	_compress.encode_tree();
}

void TransTable::print_tree_dot() {
	ofstream fout_tree_dot;
	fout_tree_dot.open("tree_dot.dot");
	_compress.print_tree_dot(fout_tree_dot);
	fout_tree_dot.close();
}
void TransTable::character_compress() {

	_header.push_back(vector<size_t>(1, 0));
	bool is_exist = false;
	size_t it_j_index = 0;

	vector<vector<size_t> >::iterator header_it;

	for (int ascii = 1; ascii < 256; ++ascii) {
		is_exist = false;

		for (header_it = _header.begin(); header_it != _header.end();
				++header_it) {
			it_j_index = (*header_it)[0];
			for (size_t it_i = 0; it_i < _state_size; ++it_i) {
				if (_table[it_i][it_j_index] == _table[it_i][ascii]) {
					is_exist = true;
				} else {
					is_exist = false;
					break;
				}
			}
			if (is_exist) {
				header_it->push_back(ascii);
				break;
			}
		}
		if (!is_exist) {
			_header.push_back(vector<size_t>(1, ascii));
		}
	}

}
void TransTable::prefix_compress() {

	vector<vector<size_t> >::iterator header_it;
	size_t j = 0;
	size_t index = 0, end = 0, cur_dst = 0;
	vector<size_t> roots;
	vector<size_t>::iterator roots_it;

	Node *compress_temp = new Node[_state_size];
	vector<Node> addElement;
	for (header_it = _header.begin(); header_it != _header.end(); ++header_it) {

		j = (*header_it)[0];

		for (size_t i = 0; i < _state_size; ++i) {
			compress_temp[i]._state = i;
			compress_temp[i]._dst = _table[i][j];
			compress_temp[i]._src_code = _compress._tree[i]._src_code;
			compress_temp[i]._dst_code =
					_compress._tree[compress_temp[i]._dst]._dst_code;
			compress_temp[i]._order = _compress._tree[i]._order;
			compress_temp[i]._accept_rules = _compress._tree[i]._accept_rules;
		}
		stable_sort(compress_temp, compress_temp + _state_size, NodeDstComp);

		index = 0;
		end = 0;
		addElement.clear();

		while (index < _state_size) {
			cur_dst = compress_temp[index]._dst;
			end = index + 1;
			while ((end < _state_size) && (compress_temp[end]._dst == cur_dst))
				++end;

			roots.clear();
			roots.push_back(index++);
			size_t parent;
			while (index < end) {
				parent = _state_size + 1;
				for (roots_it = roots.begin(); roots_it != roots.end();
						++roots_it) {

					parent = this->_compress.isCover(
							compress_temp[*roots_it]._state,
							compress_temp[index]._state);

					if (parent != _state_size + 1)
						break;
				}
				if (parent == _state_size + 1)
					roots.push_back(index++);
				else if (parent == compress_temp[index]._state)
					roots.erase(roots_it);
				else
					++index;
			}

			for (roots_it = roots.begin(); roots_it != roots.end();
					++roots_it) {
				addElement.push_back(compress_temp[*roots_it]);
			}

		}
		stable_sort(addElement.begin(), addElement.end(), NodeOrderComp);
		rebuild_tree(addElement);
		_compress._compress_table.push_back(addElement);
	}

	delete[] compress_temp;
}
void TransTable::rebuild_tree(vector<Node> &list) {

	size_t parent = 0;

	for (size_t i = 0; i < list.size(); ++i) {
		for (size_t j = i + 1; j < list.size(); ++j) {
			parent = this->_compress.isCover(list[i]._state, list[j]._state);
			if (parent == list[j]._state) {
				list[i]._parent_index = j;
				list[j]._child_index.push_back(i);
				break;
			}
		}
	}

	list[list.size() - 1]._parent_index = list.size() - 1;
}
void TransTable::print_compress_table() {
	ofstream fout;
	fout.open("prefix_compress_table.txt");
	_compress.print_compress_table(fout, _header);
	fout.close();
}
void TransTable::print_rebuild_tree_dot() {
	ofstream fout;
	string prefix = "compress_tree_";
	string suffix = ".dot";
	const char *prefix_s = prefix.c_str();
	const char *suffix_s = suffix.c_str();
	char buffer[20];

	for (size_t i = 0; i < this->_header.size(); ++i) {
		sprintf(buffer, "%s%d%s", prefix_s, i, suffix_s);
		fout.open(buffer);
		_compress.print_compress_tree_dot(fout, i);
		fout.close();
	}
}
void TransTable::print() {

	size_t block_size = 32;

	string performances[] = { "Performance_DFA.txt",
			"Performance_Character_Index.txt", "Performance_TranMerg.txt",
			"Performance_StateSplit.txt" };
	ofstream DFA_fout, Character_Index_fout, Trans_Merge_fout, State_split_fout,
			TCAM_fout;

	//print header of DFA_fout
	DFA_fout.open(performances[0].c_str());
	uint32_t original_rows = _state_size * 256;
	uint32_t original_bits = ceil(log(_state_size) / log(2)) + 8;
	uint32_t original_block_num = 0;
	DFA_fout << "#Total Entries: " << original_rows << endl;
	DFA_fout << "#bits: " << original_bits << endl << endl;
	DFA_fout << "block_size\tns\tnj\t#block_num";

	//print header of Character_Index_fout
	Character_Index_fout.open(performances[1].c_str());
	uint32_t character_index_rows = _state_size;
	uint32_t character_index_bits = original_bits - 8;
	uint32_t character_index_block_num = 0;
	Character_Index_fout << "#Total Entries: "
			<< character_index_rows * _header.size() << endl;
	Character_Index_fout << "#bits: " << character_index_bits << endl;
	Character_Index_fout << "#Index Entries: " << 256 << endl;
	Character_Index_fout << "#Index table bits:" << 8 << endl << endl;
	Character_Index_fout
			<< "block_size\tns\tnj\t#block_num\tindex(ns)\tindex(nj)\tindex(block_num)";

	//print header of Trans_Merge_fout
	Trans_Merge_fout.open(performances[2].c_str());
	size_t trans_merge_rows = 0;
	for (vector<Node> item : _compress._compress_table) {
		trans_merge_rows += item.size();
	}
	Trans_Merge_fout << "#Total Entries: " << trans_merge_rows << endl;
	Trans_Merge_fout << "#bits: "
			<< _compress._compress_table[0][0]._dst_code.size() << endl << endl;
	Trans_Merge_fout
			<< "block_size	avg(ns)	avg(nj)	#block	max(ns)	max(nj)	#block	min(ns)	min(nj)	#block	mid(ns)	mid(nj)	#block	#entries	#total_block";

	//print state_split
	State_split_fout.open(performances[3].c_str());
	char buffer[20];

	for (int i = 0; i < 8; ++i) {
		block_size = 32 * pow(2, i);
		sprintf(buffer, "%s%d%s", "TCAM_", block_size, ".txt");
		TCAM_fout.open(buffer);

		original_block_num = ceil(original_rows * 1.0 / block_size);
		character_index_block_num = ceil(
				character_index_rows * 1.0 / block_size);

		//print original DFA TCAM
		_compress.print_original_tcam(DFA_fout, block_size, original_bits,
				original_block_num);

		//print character_index TCAM
		_compress.print_charater_index_tcam(Character_Index_fout, block_size,
				character_index_bits, character_index_block_num);

		//print transition_merge TCAM
		_compress.print_transition_merge_tcam(Trans_Merge_fout, block_size,
				_header);

		_compress.print_state_split_tcam(TCAM_fout, State_split_fout,
				block_size, _header);

		TCAM_fout.close();
	}
	DFA_fout.close();
	Character_Index_fout.close();
	Trans_Merge_fout.close();
	State_split_fout.close();
}

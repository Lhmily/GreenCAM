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
	printf("\n\nBuilding the tree...");
	_compress.build_tree();
	printf("\n\nEncoding the tree...");
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

	CompressElement *compress_temp = new CompressElement[_state_size];
	vector<CompressElement> addElement;
	for (header_it = _header.begin(); header_it != _header.end(); ++header_it) {

		j = (*header_it)[0];

		for (size_t i = 0; i < _state_size; ++i) {
			compress_temp[i]._src = i;
			compress_temp[i]._dst = _table[i][j];
			compress_temp[i]._src_code = _compress._tree[i]._src_code;
			compress_temp[i]._dst_code =
					_compress._tree[compress_temp[i]._dst]._dst_code;
			compress_temp[i]._order = _compress._tree[i]._order;
		}
		stable_sort(compress_temp, compress_temp + _state_size,
				CompressElementDstComp);

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
							compress_temp[*roots_it]._src,
							compress_temp[index]._src);

					if (parent != _state_size + 1)
						break;
				}
				if (parent == _state_size + 1)
					roots.push_back(index++);
				else if (parent == compress_temp[index]._src)
					roots.erase(roots_it);
				else
					++index;
			}

			for (roots_it = roots.begin(); roots_it != roots.end();
					++roots_it) {
				addElement.push_back(compress_temp[*roots_it]);
			}

		}
		stable_sort(addElement.begin(), addElement.end(),
				CompressElementOrderDstComp);
		_compress._compress_table.push_back(addElement);
	}

	delete[] compress_temp;
}

void TransTable::print_compress_table() {
	ofstream fout;
	fout.open("prefix_compress_table.txt");
	_compress.print_compress_table(fout, _header);
	fout.close();
}

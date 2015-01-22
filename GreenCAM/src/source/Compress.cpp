/*
 * Compress.cpp
 *
 *  Created on: Jul 20, 2014
 *      Author: Lhmily
 */

#include "../header/Compress.h"

Compress::Compress() {
	// TODO Auto-generated constructor stub
	tcam_parameters.CMOS_tech = 0.18;
	tcam_parameters.Nrd = 1;
}

void Compress::init(DFA *dfa) {
	this->_dfa = dfa;
	this->_state_size = _dfa->size();
	this->_tree = _root = NULL;
	_dfa->set_depth();
}
Compress::~Compress() {
	// TODO Auto-generated destructor stub
	if (NULL != _tree) {
		delete[] _tree;
		_tree = NULL;
	}
}

void Compress::build_tree() {
	state_t *default_tx = _dfa->get_default_tx();
	linked_set **accepted_rules = _dfa->get_accepted_rules();
	this->_tree = new Node[_state_size];
	linked_set *ls = NULL;
	state_t *depth = _dfa->get_depth();

	for (size_t s = 0; s < _state_size; ++s) {
		_tree[s]._state = s;
		_tree[s]._parent = &_tree[default_tx[s]];
		_tree[s]._depth = depth[s];

		if (s == default_tx[s])
			_root = &_tree[s];
		else
			_tree[s]._parent->_child.push_back(&_tree[s]);

		if (!accepted_rules[s]->empty()) {
			ls = accepted_rules[s];
			while (NULL != ls) {
				_tree[s]._accept_rules.push_back(ls->value());
				ls = ls->succ();
			}
		}
	}

	this->calculate_code_mask(_root);
	this->reorder_child(_root);
	size_t order = 1;
	this->set_tree_order(_root, order);
}

void Compress::set_tree_order(Node *node, size_t &order) {

	vector<Node *>::iterator it = node->_child.begin();
	for (; it != node->_child.end(); ++it) {
		this->set_tree_order(*it, order);
	}
	node->_order = order++;

}
void Compress::reorder_child(Node *node) {
	if (node->_child.empty())
		return;
	stable_sort(node->_child.begin(), node->_child.end(), [](Node *a,Node *b) {
		return a->_code_mask>b->_code_mask;
	});
	for (Node *elem : node->_child)
		this->reorder_child(elem);

}
size_t Compress::calculate_code_mask(Node *node) {
	if (node->_child.empty()) {
		node->_code_mask = 0;
		return 1;
	}
	size_t temp = 1;
	for (Node *child_elem : node->_child) {
		temp += this->calculate_code_mask(child_elem);
	}
	node->_code_mask = ceil(log(temp) / log(2));
	return pow(2, node->_code_mask);
}

void Compress::encode_tree(Node *node, string pre_str) {
	if (NULL == node)
		node = _root;
	node->_dst_code.resize(_root->_code_mask);
	node->_dst_code.assign(_root->_code_mask, '0');
	for (size_t i = 0; i < pre_str.size(); ++i)
		node->_dst_code[i] = pre_str[i];

	node->_src_code.resize(_root->_code_mask);
	node->_src_code.assign(_root->_code_mask, '*');
	for (size_t i = 0; i < pre_str.size(); ++i)
		node->_src_code[i] = pre_str[i];

	if (node->_child.empty())
		return;
	string str_temp;
	str_temp.resize(node->_code_mask - (*(node->_child.end() - 1))->_code_mask);
	vector<string> alloc_code;
	vector<size_t> len_list;
	for (Node *elem : node->_child)
		len_list.push_back(node->_code_mask - elem->_code_mask);
	this->alloc_code(alloc_code, len_list.size(), len_list, str_temp, true, 1);
	this->alloc_code(alloc_code, len_list.size(), len_list, str_temp, false, 1);

	int it = 0;
	for (Node *elem : node->_child)
		this->encode_tree(elem, pre_str + alloc_code[it++]);
}

void Compress::alloc_code(vector<string> &result, int required_size,
		const vector<size_t>&len_list, string &str_temp, bool left,
		size_t layer) {
	if (result.size() >= required_size)
		return;
	str_temp[layer - 1] = left ? '1' : '0';
	if (layer == len_list[result.size()]) {
		result.push_back(str_temp.substr(0, layer));
		return;
	}
	this->alloc_code(result, required_size, len_list, str_temp, true,
			layer + 1);
	this->alloc_code(result, required_size, len_list, str_temp, false,
			layer + 1);
}

void Compress::print_tree_dot(ofstream &fout) const {
	if (NULL == _root)
		return;
	fout << "digraph dot{" << endl;

	for (size_t s = 0; s < _state_size; ++s) {
		fout << s << "[shape="
				<< (_tree[s]._accept_rules.empty() ? "circle" : "doublecircle")
				<< ",label=\"" << s << "-" << _tree[s]._depth << "";
		if (!_tree[s]._accept_rules.empty()) {
			fout << "/";
			for (vector<size_t>::iterator it = _tree[s]._accept_rules.begin();
					it != _tree[s]._accept_rules.end() - 1; ++it)
				fout << *it << ",";
			fout << *(_tree[s]._accept_rules.end() - 1);
		}
		fout << "\n" << _tree[s]._code_mask << "\n" << _tree[s]._src_code
				<< "\n" << _tree[s]._dst_code << "\"];" << endl;
	}

	for (size_t s = 0; s < _state_size; ++s)
		fout << s << " -> " << _tree[s]._parent->_state << ";" << endl;
	fout << "}";
}
/**
 * if state_1 is parent return state_1
 * if state_2 is parent return state_2
 * otherwise return bigger than state_size. That is state_size+1.
 */
size_t Compress::isCover(size_t state_1, size_t state_2) {
	if (state_1 >= this->_state_size || state_2 >= this->_state_size)
		return _state_size + 1;

	Node *n1 = &(_tree[state_1]);
	Node *n2 = &(_tree[state_2]);

	while (true) {
		if (n1->_state == state_2)
			return state_2;

		if (n1 == _root)
			break;
		n1 = n1->_parent;
	}

	n1 = &_tree[state_1];

	while (true) {
		if (n2->_state == state_1)
			return state_1;

		if (n2 == _root)
			break;
		n2 = n2->_parent;
	}
	return _state_size + 1;
}
void Compress::print_compress_table(ofstream &fout,
		vector<vector<size_t> > &header) {

	vector<vector<Node> >::iterator table_it = _compress_table.begin();
	vector<Node>::iterator code_it;
	vector<vector<size_t> >::iterator header_it = header.begin();
	vector<size_t>::iterator h_it;

	size_t index = 0, pre = 0, end = 0;
	for (; table_it != _compress_table.end(); ++table_it) {

		fout << endl << "ASCII:";
		index = pre = 0;
		end = 1;
		while (index < header_it->size()) {
			pre = index;
			while ((end < header_it->size())
					&& ((*header_it)[end] == (*header_it)[pre] + 1)) {
				pre = end;
				++end;
			}
			if (index != pre)
				fout << (*header_it)[index] << "-" << (*header_it)[pre] << " ";
			else
				fout << (*header_it)[index] << " ";
			index = end;
			++end;
		}

		fout << "Transition entries: " << table_it->size() << endl << endl;
		for (code_it = table_it->begin(); code_it != table_it->end();
				++code_it) {
			fout << code_it->_src_code << " -> " << code_it->_dst_code << "("
					<< code_it->_dst << ")" << endl;
		}
		++header_it;
	}

}
void Compress::print_compress_tree_dot(ofstream &fout, int index) {
	if (NULL == _root)
		return;
	fout << "digraph dot{" << endl;
	vector<Node> &list = _compress_table[index];
	for (size_t s = 0; s < list.size(); ++s) {
		fout << list[s]._state << "[shape="
				<< (list[s]._accept_rules.empty() ? "circle" : "doublecircle")
				<< ",label=\"";
		if (!list[s]._accept_rules.empty()) {
			fout << "/";
			for (vector<size_t>::iterator it = list[s]._accept_rules.begin();
					it != list[s]._accept_rules.end() - 1; ++it)
				fout << *it << ",";
			fout << *(list[s]._accept_rules.end() - 1);
		}
		fout << "\n" << list[s]._src_code << "\n" << list[s]._dst_code << "("
				<< list[s]._dst << ")" << "\"];" << endl;
	}

	for (size_t s = 0; s < list.size(); ++s)
		fout << list[s]._state << " -> " << list[list[s]._parent_index]._state
				<< ";" << endl;
	fout << "}";
}

void Compress::tcam_parameters_init(size_t block_size, uint32_t bits,
		uint32_t block_num) {
	tcam_parameters.number_of_rows = block_size * block_num;
	tcam_parameters.number_of_column_bits = bits;
	tcam_parameters.number_of_subbanks = block_num;

}

void Compress::print_original_tcam(ofstream &fout, size_t block_size,
		uint32_t bits, uint32_t block_num) {
	_block_size = block_size;
	this->tcam_parameters_init(_block_size, bits, block_num);

	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);

	fout << endl << _block_size << "\t" << tcam_search_delay.max_delay * 1e9
			<< "\t" << tcam_search_power.total_power * 1e9 << "\t" << block_num;
}

void Compress::print_charater_index_tcam(ofstream &fout, size_t block_size,
		uint32_t bits, uint32_t block_num, int step) {
	_block_size = block_size;
	this->tcam_parameters_init(_block_size, bits, block_num);

	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);

	//print data blocks
	fout << endl << _block_size << "\t" << tcam_search_delay.max_delay * 1e9
			<< "\t" << tcam_search_power.total_power * 1e9 << "\t" << block_num;

	block_num = ceil(pow(256, step) * 1.0 / _block_size);
	this->tcam_parameters_init(_block_size, 8 * step, block_num);

	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);

	fout << "\t" << tcam_search_delay.max_delay * 1e9 << "\t"
			<< tcam_search_power.total_power * 1e9 << "\t" << block_num;
}

size_t Compress::calculate_compress_tree_child_num(size_t i, size_t j,
		vector<vector<Node> > &compress_table) {

	size_t ret_val = 1;
	compress_table[i][j]._is_block_root = false;
	for (size_t child : compress_table[i][j]._child_index) {
		ret_val += calculate_compress_tree_child_num(i, child, compress_table);
	}
	compress_table[i][j]._child_count = ret_val;
	return ret_val;
}
void Compress::state_split_tree(vector<vector<Node> > &compress_table) {
	for (size_t i = 0; i < compress_table.size(); ++i) {
		calculate_compress_tree_child_num(i, compress_table[i].size() - 1,
				compress_table);
	}
}

void Compress::state_split_block(size_t block_size,
		vector<vector<Node> > &compress_table,
		vector<vector<size_t> > &ascii_blocks) {

	ascii_blocks.clear();
	vector<size_t> temp;
	size_t block_index = 0, size = 0;

	for (size_t i = 0; i < compress_table.size(); ++i) {
		temp.clear();
		ascii_blocks.push_back(temp);
		size = ascii_blocks.size();
		do {
			block_index = get_state_split_block(i, compress_table[i].size() - 1,
					block_size, compress_table);
			ascii_blocks[size - 1].push_back(block_index);
		} while (block_index != compress_table[i].size() - 1);

	}

}

size_t Compress::get_state_split_block(size_t i, size_t j, size_t block_size,
		vector<vector<Node> > &compress_table) {

	if ((compress_table[i][j]._child_count <= block_size)
			&& !(compress_table[i][j]._is_block_root)) {

		compress_table[i][j]._is_block_root = true;
		if (j != compress_table[i].size() - 1) {

			for (size_t temp = compress_table[i][j]._parent_index;
					temp < compress_table[i].size() - 1; temp =
							compress_table[i][temp]._parent_index) {
				compress_table[i][temp]._child_count -=
						compress_table[i][j]._child_count;
			}

			compress_table[i][compress_table[i].size() - 1]._child_count -=
					compress_table[i][j]._child_count;
		}
		return j;
	}

	vector<size_t> index_temp;

	for (size_t child : compress_table[i][j]._child_index) {
		if (!(compress_table[i][child]._is_block_root)) {
			index_temp.push_back(child);
		}
	}
	if (index_temp.empty())
		return j;

	size_t max_index = index_temp[0];
	for (size_t temp = 1; temp < index_temp.size(); ++temp) {
		if (compress_table[i][index_temp[temp]]._child_count
				> compress_table[i][max_index]._child_count) {
			max_index = index_temp[temp];
		}
	}
	return get_state_split_block(i, max_index, block_size, compress_table);
}

void Compress::print_transition_merge_tcam(ofstream &fout, size_t block_size,
		vector<vector<size_t> > &header) {

	_block_size = block_size;
	Tcam_Node tcam_Node_array[256];

	size_t transition_rows = 0, transition_block_num = 0, bits =
			_compress_table[0][0]._dst_code.size(), total_block_num = 0,
			avg_block_num = 0;
	float ns = 0.0, nj = 0.0, total_ns = 0.0, total_nj = 0.0, avg_ns = 0.0,
			avg_nj = 0.0;

	for (size_t index = 0; index < header.size(); ++index) {
		transition_rows = this->_compress_table[index].size();
		transition_block_num = ceil(transition_rows * 1.0 / _block_size);

		this->tcam_parameters_init(_block_size, bits, transition_block_num);

		calculate_tcam_power(tcam_parameters, &tcam_search_power,
				&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
				&tcam_search_delay);

		ns = tcam_search_delay.max_delay * 1e9;
		nj = tcam_search_power.total_power * 1e9;
		total_ns += ns * header[index].size();
		total_nj += nj * header[index].size();
		total_block_num += transition_block_num * header[index].size();

		for (size_t item : header[index]) {
			tcam_Node_array[item]._block_size = _block_size;
			tcam_Node_array[item]._block_num = transition_block_num;
			tcam_Node_array[item]._bits = bits;
			tcam_Node_array[item]._ns = ns;
			tcam_Node_array[item]._nj = nj;
		}
	}
	avg_ns = total_ns / 256;
	avg_nj = total_nj / 256;
	avg_block_num = ceil(total_block_num * 1.0 / 256);
	stable_sort(tcam_Node_array, tcam_Node_array + 256, TcamNodeComp);
	//print avg
	fout << endl << _block_size << "\t" << avg_ns << "\t" << avg_nj << "\t"
			<< avg_block_num << "\t";
	//print max
	fout << tcam_Node_array[255]._ns << "\t" << tcam_Node_array[255]._nj << "\t"
			<< tcam_Node_array[255]._block_num << "\t";

	//print min
	fout << tcam_Node_array[0]._ns << "\t" << tcam_Node_array[0]._nj << "\t"
			<< tcam_Node_array[0]._block_num << "\t";

	//print mid

	fout << (tcam_Node_array[127]._ns + tcam_Node_array[128]._ns) / 2 << "\t"
			<< (tcam_Node_array[127]._nj + tcam_Node_array[128]._nj) / 2 << "\t"
			<< tcam_Node_array[127]._block_num << "\t";
	//print total blocks
	fout << total_block_num;
}

void Compress::print_state_split_tcam(ofstream &tcam_fout, ofstream &fout,
		size_t block_size, vector<vector<size_t> > &header) {

	this->state_split_tree(this->_compress_table);

	this->state_split_block(block_size, _compress_table, _ascii_blocks);

	size_t index_block_num = ceil(256 * 1.0 / block_size);

	this->print_index_tcam_table_header(tcam_fout, index_block_num);

	//print index blocks
	AsciiNode asciiIndex[256];

	size_t block_count = 0;
	for (size_t i = 0; i < header.size(); ++i) {
		for (size_t j = 0; j < header[i].size(); ++j) {
			asciiIndex[header[i][j]]._ascii_blocks_index = i;
			asciiIndex[header[i][j]]._ascii = header[i][j];
			asciiIndex[header[i][j]]._ascii_code = state_convert_code(
					header[i][j], 8);
			for (size_t block_it = 0; block_it < _ascii_blocks[i].size();
					++block_it) {
				asciiIndex[header[i][j]]._index.push_back(
						block_count + block_it);
			}
		}
		block_count += _ascii_blocks[i].size();
	}

	vector<pair<size_t, size_t> > state_split_index_blocks;
	size_t ss_count = 0, ss_temp = 0;
	for (size_t i = 0; i < _ascii_blocks.size(); ++i) {
		ss_temp = ceil(_ascii_blocks[i].size() * 1.0 / block_size);

		state_split_index_blocks.push_back(
				make_pair(ss_count, ss_count + ss_temp));

		ss_count += ss_temp;
	}

	size_t flag = 0, flag_count = 0;
	size_t character_index_start = 0, character_index_end = 0;
	for (size_t i = 0; i < 256; ++i) {
		if (i == flag) {
			flag += block_size;
			tcam_fout << "# Block " << flag_count++ << endl;
		}
		character_index_start =
				state_split_index_blocks[asciiIndex[i]._ascii_blocks_index].first;

		character_index_end =
				state_split_index_blocks[asciiIndex[i]._ascii_blocks_index].second;
		tcam_fout << asciiIndex[i]._ascii_code << ", (";

		for (size_t m = character_index_start; m < character_index_end - 1;
				++m) {
			tcam_fout << m << ", ";
		}

		tcam_fout << character_index_end - 1;
		tcam_fout << ")" << endl;
	}

	//print data TCAM blocks
	size_t ss_block_num =
			state_split_index_blocks[state_split_index_blocks.size() - 1].second;
	this->print_data_tcam_table_header(tcam_fout, block_count + ss_block_num);

	flag = 0;
	flag_count = 0;

	size_t total_blocks_count = ss_block_num;

	for (size_t i = 0; i < state_split_index_blocks.size(); ++i) {

		flag = 0;
		for (size_t j = 0; j < _ascii_blocks[i].size(); ++j) {
			if (j == flag) {
				flag += block_size;
				tcam_fout << "# Block " << flag_count++ << endl;
			}
			tcam_fout << _compress_table[i][_ascii_blocks[i][j]]._src_code
					<< ", 1, " << total_blocks_count++ << ", (0)" << endl;
		}

	}
	for (size_t i = 0; i < _ascii_blocks.size(); ++i) {
		for (size_t j = 0; j < _ascii_blocks[i].size(); ++j) {
			tcam_fout << "# Block " << flag_count++ << endl;
			print_state_split_block_detail(tcam_fout, i, _ascii_blocks[i][j],
					_ascii_blocks[i][j], _compress_table);
		}
	}
	fout << endl << block_size << "\t";
	//print character index tcam
	size_t bits = _compress_table[0][0]._dst_code.size();
	size_t character_block_num = ceil(256 * 1.0 / _block_size);
	float ns = 0.0, nj = 0.0;
	this->tcam_parameters_init(_block_size, bits, character_block_num);
	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);
	ns = tcam_search_delay.max_delay * 1e9;
	nj = tcam_search_power.total_power * 1e9;
	fout << ns << "\t" << nj << "\t";

	Tcam_Node ss_block_array[256];
	size_t ss_index_block_num = 0;
	size_t total_block_num = 0;
	float total_ns = 0.0, total_nj = 0.0;
	for (size_t i = 0; i < header.size(); ++i) {
		ss_index_block_num = state_split_index_blocks[i].second
				- state_split_index_blocks[i].first;
		tcam_parameters_init(_block_size, bits, ss_index_block_num);
		calculate_tcam_power(tcam_parameters, &tcam_search_power,
				&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
				&tcam_search_delay);
		ns = tcam_search_delay.max_delay * 1e9;
		nj = tcam_search_power.total_power * 1e9;

		total_block_num += ss_index_block_num * header[i].size();
		total_ns += ns * header[i].size();
		total_nj += nj * header[i].size();
		for (size_t j = 0; j < header[i].size(); ++j) {
			ss_block_array[header[i][j]]._ns = ns;
			ss_block_array[header[i][j]]._nj = nj;
			ss_block_array[header[i][j]]._block_num = ss_index_block_num;
			//cout << header[i][j] << " -> " << ss_index_block_num;
		}
	}

	stable_sort(ss_block_array, ss_block_array + 256, TcamNodeComp);
	float avg_ns = total_ns / 256;
	float avg_nj = total_nj / 256;
	size_t avg_block = ceil(total_block_num * 1.0 / 256);

	fout << avg_ns << "\t" << avg_nj << "\t" << avg_block << "\t";

	fout << ss_block_array[255]._ns << "\t" << ss_block_array[255]._nj << "\t"
			<< ss_block_array[255]._block_num << "\t";

	fout << ss_block_array[0]._ns << "\t" << ss_block_array[0]._nj << "\t"
			<< ss_block_array[0]._block_num << "\t";

	fout << (ss_block_array[127]._ns + ss_block_array[128]._ns) / 2 << "\t"
			<< (ss_block_array[127]._nj + ss_block_array[128]._nj) / 2 << "\t"
			<< ss_block_array[127]._block_num << "\t";

	tcam_parameters_init(_block_size, bits, 1);
	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);

	ns = tcam_search_delay.max_delay * 1e9;
	nj = tcam_search_power.total_power * 1e9;
	fout << ns << "\t" << nj << "\t" << total_block_num;

}

void Compress::print_state_split_block_detail(ofstream &fout, const size_t i,
		const size_t j, const size_t root,
		vector<vector<Node> > &compress_table) {

	if (root != j) {
		if (compress_table[i][j]._is_block_root)
			return;
	}

	for (size_t child : compress_table[i][j]._child_index) {
		print_state_split_block_detail(fout, i, child, root, compress_table);
	}

	fout << compress_table[i][j]._src_code << ", 0, "
			<< compress_table[i][j]._dst_code << ", (";
	if (compress_table[i][j]._accept_rules.empty())
		fout << 0;
	else {
		for (size_t m = 0; m < compress_table[i][j]._accept_rules.size() - 1;
				++m) {
			fout << compress_table[i][j]._accept_rules[m] << ", ";
		}
		fout
				<< compress_table[i][j]._accept_rules[compress_table[i][j]._accept_rules.size()
						- 1];
	}
	fout << ")" << endl;
}

string Compress::state_convert_code(size_t s, const int bits) const {
	string ret;
	int i, j;
	for (i = bits - 1, j = 0; i >= 0; i--, j++) {
		if (0 == ((s >> i) & 0x01)) {
			ret.append("0");
		} else {
			ret.append("1");
		}
	}
	return ret;
}

void Compress::print_compress_table_2(ofstream &fout,
		vector<vector<size_t> > &header) {
	vector<vector<Node> >::iterator table_it = _compress_table_2.begin();
	vector<Node>::iterator code_it;

	for (; table_it != _compress_table_2.end(); ++table_it) {

		//head_index = count++ / mod;

		fout << "------------------------------------------------" << endl;
		fout << "Transition entries: " << table_it->size() << endl << endl;
		for (code_it = table_it->begin(); code_it != table_it->end();
				++code_it) {
			fout << code_it->_src_code << " -> " << code_it->_dst_code << "("
					<< code_it->_dst << ")" << endl;
		}
	}
}

void Compress::print_transition_merge_tcam_2(ofstream &fout, size_t block_size,
		vector<vector<size_t> > &header) {

	_block_size = block_size;

	size_t transition_rows = 0, transition_block_num = 0, bits =
			_compress_table[0][0]._dst_code.size(), total_block_num = 0,
			avg_block_num = 0;

	float ns = 0.0, nj = 0.0, total_ns = 0.0, total_nj = 0.0, avg_ns = 0.0,
			avg_nj = 0.0;

	size_t header_size = header.size();
	size_t cur_header_size = 0;

	vector<Tcam_Node> tcam_Node_array(65536);

	for (size_t index = 0; index < header_size; ++index) {
		transition_rows = this->_compress_table_2[index].size();
		transition_block_num = ceil(transition_rows * 1.0 / _block_size);

		this->tcam_parameters_init(_block_size, bits, transition_block_num);

		calculate_tcam_power(tcam_parameters, &tcam_search_power,
				&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
				&tcam_search_delay);

		ns = tcam_search_delay.max_delay * 1e9;
		nj = tcam_search_power.total_power * 1e9;
		cur_header_size = header[index].size();
		total_ns += ns * cur_header_size;
		total_nj += nj * cur_header_size;
		total_block_num += transition_block_num * cur_header_size;

		for (size_t item : header[index]) {

			tcam_Node_array[item]._block_size = _block_size;
			tcam_Node_array[item]._block_num = transition_block_num;
			tcam_Node_array[item]._bits = bits;
			tcam_Node_array[item]._ns = ns;
			tcam_Node_array[item]._nj = nj;
		}
	}

	avg_ns = total_ns / 65536;
	avg_nj = total_nj / 65536;
	avg_block_num = ceil(total_block_num * 1.0 / 65536);
	stable_sort(tcam_Node_array.begin(), tcam_Node_array.end(), TcamNodeComp);
	//print avg
	fout << endl << _block_size << "\t" << avg_ns << "\t" << avg_nj << "\t"
			<< avg_block_num << "\t";
	//print max
	fout << tcam_Node_array[65535]._ns << "\t" << tcam_Node_array[65535]._nj
			<< "\t" << tcam_Node_array[65535]._block_num << "\t";

	//print min
	fout << tcam_Node_array[0]._ns << "\t" << tcam_Node_array[0]._nj << "\t"
			<< tcam_Node_array[0]._block_num << "\t";

	//print mid

	fout << (tcam_Node_array[32767]._ns + tcam_Node_array[32768]._ns) / 2
			<< "\t"
			<< (tcam_Node_array[32767]._nj + tcam_Node_array[32768]._nj) / 2
			<< "\t" << tcam_Node_array[32767]._block_num << "\t";
	fout << total_block_num;
}

void Compress::get_state_split_index_blocks(
		vector<pair<size_t, size_t> > &state_split_index_blocks,
		size_t block_size) {
	size_t ss_count = 0, ss_temp = 0;
	for (size_t i = 0; i < _ascii_blocks_2.size(); ++i) {
		ss_temp = ceil(_ascii_blocks_2[i].size() * 1.0 / block_size);

		state_split_index_blocks.push_back(
				make_pair(ss_count, ss_count + ss_temp));

		ss_count += ss_temp;
	}
}

void Compress::print_split_tcam_detail_2(ofstream &tcam_fout, ofstream &fout,
		size_t block_size, vector<vector<size_t> > &header) {

	this->state_split_tree(_compress_table_2);

	this->state_split_block(block_size, _compress_table_2, _ascii_blocks_2);

	size_t index_block_num = ceil(65536 * 1.0 / block_size);

	this->print_index_tcam_table_header(tcam_fout, index_block_num);

	//print index blocks
	vector<AsciiNode> asciiIndex(65536);
	size_t block_count = 0;

	block_count = this->generate_asciiIndex_2(header, asciiIndex);

	vector<pair<size_t, size_t> > state_split_index_blocks;
	this->get_state_split_index_blocks(state_split_index_blocks, block_size);

	size_t flag = 0, flag_count = 0;
	size_t character_index_start = 0, character_index_end = 0;
	for (size_t i = 0; i < 65536; ++i) {
		if (i == flag) {
			flag += block_size;
			tcam_fout << "# Block " << flag_count++ << endl;
		}
		character_index_start =
				state_split_index_blocks[asciiIndex[i]._ascii_blocks_index].first;

		character_index_end =
				state_split_index_blocks[asciiIndex[i]._ascii_blocks_index].second;
		tcam_fout << asciiIndex[i]._ascii_code << ", (";

		for (size_t m = character_index_start; m < character_index_end - 1;
				++m) {
			tcam_fout << m << ", ";
		}

		tcam_fout << character_index_end - 1;
		tcam_fout << ")" << endl;
	}

	//print data TCAM blocks

	size_t ss_block_num =
			state_split_index_blocks[state_split_index_blocks.size() - 1].second;
	this->print_data_tcam_table_header(tcam_fout, block_count + ss_block_num);

	flag = 0;
	flag_count = 0;

	size_t total_blocks_count = ss_block_num;

	for (size_t i = 0; i < state_split_index_blocks.size(); ++i) {

		flag = 0;
		for (size_t j = 0; j < _ascii_blocks_2[i].size(); ++j) {
			if (j == flag) {
				flag += block_size;
				tcam_fout << "# Block " << flag_count++ << endl;
			}
			tcam_fout << _compress_table_2[i][_ascii_blocks_2[i][j]]._src_code
					<< ", 1, " << total_blocks_count++ << ", (0)" << endl;
		}

	}
	for (size_t i = 0; i < _ascii_blocks_2.size(); ++i) {
		for (size_t j = 0; j < _ascii_blocks_2[i].size(); ++j) {
			tcam_fout << "# Block " << flag_count++ << endl;
			print_state_split_block_detail(tcam_fout, i, _ascii_blocks_2[i][j],
					_ascii_blocks_2[i][j], _compress_table_2);
		}
	}

	fout << endl << block_size << "\t";
	//print character index tcam
	size_t bits = _compress_table_2[0][0]._dst_code.size();
	size_t character_block_num = ceil(65536 * 1.0 / _block_size);
	float ns = 0.0, nj = 0.0;
	this->tcam_parameters_init(_block_size, bits, character_block_num);
	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);
	ns = tcam_search_delay.max_delay * 1e9;
	nj = tcam_search_power.total_power * 1e9;
	fout << ns << "\t" << nj << "\t";

	vector<Tcam_Node> ss_block_array(65536);
	size_t ss_index_block_num = 0;
	size_t total_block_num = 0;
	float total_ns = 0.0, total_nj = 0.0;
	for (size_t i = 0; i < header.size(); ++i) {
		ss_index_block_num = state_split_index_blocks[i].second
				- state_split_index_blocks[i].first;
		tcam_parameters_init(_block_size, bits, ss_index_block_num);
		calculate_tcam_power(tcam_parameters, &tcam_search_power,
				&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
				&tcam_search_delay);
		ns = tcam_search_delay.max_delay * 1e9;
		nj = tcam_search_power.total_power * 1e9;

		total_block_num += ss_index_block_num * header[i].size();
		total_ns += ns * header[i].size();
		total_nj += nj * header[i].size();
		for (size_t j = 0; j < header[i].size(); ++j) {
			ss_block_array[header[i][j]]._ns = ns;
			ss_block_array[header[i][j]]._nj = nj;
			ss_block_array[header[i][j]]._block_num = ss_index_block_num;
			//cout << header[i][j] << " -> " << ss_index_block_num;
		}
	}

	stable_sort(ss_block_array.begin(), ss_block_array.end(), TcamNodeComp);
	float avg_ns = total_ns / 65536;
	float avg_nj = total_nj / 65536;
	size_t avg_block = ceil(total_block_num * 1.0 / 65536);

	fout << avg_ns << "\t" << avg_nj << "\t" << avg_block << "\t";

	fout << ss_block_array[65535]._ns << "\t" << ss_block_array[65535]._nj
			<< "\t" << ss_block_array[65535]._block_num << "\t";

	fout << ss_block_array[0]._ns << "\t" << ss_block_array[0]._nj << "\t"
			<< ss_block_array[0]._block_num << "\t";

	fout << (ss_block_array[32767]._ns + ss_block_array[32768]._ns) / 2 << "\t"
			<< (ss_block_array[32767]._nj + ss_block_array[32768]._nj) / 2
			<< "\t" << ss_block_array[32767]._block_num << "\t";

	tcam_parameters_init(_block_size, bits, 1);
	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);

	ns = tcam_search_delay.max_delay * 1e9;
	nj = tcam_search_power.total_power * 1e9;
	fout << ns << "\t" << nj << "\t" << total_block_num;
}

void Compress::print_index_tcam_table_header(ofstream &tcam_fout,
		size_t index_block_num) {
	tcam_fout << "# Index TCAM Table" << endl;
	tcam_fout << "# <#Block>" << endl;
	tcam_fout << index_block_num << endl;
	tcam_fout << "# Block <#Num>";
	tcam_fout
			<< "# TCAM Part (Input Character), SRAM Part (A list of Block IDs)"
			<< endl;
}

void Compress::print_data_tcam_table_header(ofstream &tcam_fout,
		size_t data_block_num) {
	tcam_fout << "# Data TCAM Table" << endl;
	tcam_fout << "# <#Block>" << endl;
	tcam_fout << data_block_num << endl;
	tcam_fout << "# Block <#Num>" << endl;
	tcam_fout
			<< "# TCAM Part (Src State ID), SRAM Part (Flag1, Dst State ID or Block ID, Flag2)"
			<< endl;
	tcam_fout
			<< "# Flag1 is one bit (0 or 1) that denotes that the next is a dst state ID or a block ID "
			<< endl;
	tcam_fout
			<< "# Flag2 is a list of integer values (0, 1, 2...) that denote the index of the matching RegEx patterns"
			<< endl;
}

size_t Compress::generate_asciiIndex_2(vector<vector<size_t> > &header,
		vector<AsciiNode> &asciiIndex) {
	size_t block_count = 0;
	char first, second;
	string first_str, second_str;

	for (size_t i = 0; i < header.size(); ++i) {

		for (size_t j = 0; j < header[i].size(); ++j) {

			asciiIndex[header[i][j]]._ascii_blocks_index = i;
			asciiIndex[header[i][j]]._ascii = header[i][j];
			recover_input_2_by_index(header[i][j], first, second);
			first_str = state_convert_code(first, 8);
			second_str = state_convert_code(second, 8);

			asciiIndex[header[i][j]]._ascii_code = first_str + second_str;
			for (size_t block_it = 0; block_it < _ascii_blocks_2[i].size();
					++block_it) {
				asciiIndex[header[i][j]]._index.push_back(
						block_count + block_it);
			}
		}
		block_count += _ascii_blocks_2[i].size();
	}
	return block_count;
}

/*
 *
 * size_t flag = 0, flag_count = 0;
 size_t character_index_start = 0, character_index_end = 0;
 for (size_t i = 0; i < 65536; ++i) {
 if (i == flag) {
 flag += block_size;
 tcam_fout << "# Block " << flag_count++ << endl;
 }
 character_index_start =
 state_split_index_blocks[asciiIndex[i]._ascii_blocks_index].first;

 character_index_end =
 state_split_index_blocks[asciiIndex[i]._ascii_blocks_index].second;
 tcam_fout << asciiIndex[i]._ascii_code << ", (";

 for (size_t m = character_index_start; m < character_index_end - 1;
 ++m) {
 tcam_fout << m << ", ";
 }

 tcam_fout << character_index_end - 1;
 tcam_fout << ")" << endl;
 }
 */

void Compress::generate_ascii_star(vector<AsciiNode> &asciiIndex,
		vector<AsciiNode> &asciiStar,
		vector<pair<size_t, size_t> > &state_split_index_blocks,
		bool is_reverse) {

	int index = 0;
	bool is_exist = false;
	size_t character_index_start = 0, character_index_end = 0;

	for (int i = 0; i < 256; ++i) {
		asciiStar[i]._ascii = i;
		asciiStar[i]._ascii_code = this->state_convert_code(i, 8);
		asciiStar[i]._ascii_blocks_index = i;
		for (int j = 0; j < 256; ++j) {
			index = is_reverse ? (i + 256 * j) : (i * 256 + j);

			character_index_start =
					state_split_index_blocks[asciiIndex[index]._ascii_blocks_index].first;

			character_index_end =
					state_split_index_blocks[asciiIndex[index]._ascii_blocks_index].second;

			for (size_t item = character_index_start;
					item < character_index_end; ++item) {
				is_exist = false;
				for (size_t jtem : asciiStar[i]._index) {
					if (item == jtem) {
						is_exist = true;
						break;
					}
				}
				if (!is_exist)
					asciiStar[i]._index.push_back(item);
			}

		}
		stable_sort(asciiStar[i]._index.begin(), asciiStar[i]._index.end());
	}

}

void Compress::print_split_tcam_2(ofstream &tcam_fout, ofstream &fout,
		size_t block_size, vector<vector<size_t> > &header) {

	this->state_split_tree(_compress_table_2);

	this->state_split_block(block_size, _compress_table_2, _ascii_blocks_2);

	size_t index_block_num = ceil(65536 * 1.0 / block_size);

	this->print_index_tcam_table_header(tcam_fout, index_block_num);

	vector<AsciiNode> asciiIndex(65536);
	size_t block_count = 0;

	block_count = this->generate_asciiIndex_2(header, asciiIndex);

	vector<pair<size_t, size_t> > state_split_index_blocks;

	this->get_state_split_index_blocks(state_split_index_blocks, block_size);

	vector<AsciiNode> asciiStar(256);
	vector<AsciiNode> starAscii(256);
	this->generate_ascii_star(asciiIndex, asciiStar, state_split_index_blocks,
			false);
	this->generate_ascii_star(asciiIndex, starAscii, state_split_index_blocks,
			true);

//print index blocks
	tcam_fout << " ASCII *" << endl;
	size_t flag = 0, flag_count = 0, index_size = 0;

	for (size_t i = 0; i < 256; ++i) {
		if (i == flag) {
			flag += block_size;
			tcam_fout << "# Block " << flag_count++ << endl;
		}
		tcam_fout << asciiStar[i]._ascii_code << ", (";
		index_size = asciiStar[i]._index.size();
		for (size_t j = 0; j < index_size - 1; ++j) {
			tcam_fout << asciiStar[i]._index[j] << ",";
		}
		if (index_size >= 1)
			tcam_fout << asciiStar[i]._index[index_size - 1] << ")" << endl;

	}
	tcam_fout
			<< "/////////////////////////////////////////////////////////////////////"
			<< endl << " * ASCII" << endl;
	for (size_t i = 0, flag = 0; i < 256; ++i) {
		if (i == flag) {
			flag += block_size;
			tcam_fout << "# Block " << flag_count++ << endl;
		}
		tcam_fout << starAscii[i]._ascii_code << ", (";
		index_size = starAscii[i]._index.size();
		for (size_t j = 0; j < index_size - 1; ++j) {
			tcam_fout << starAscii[i]._index[j] << ",";
		}
		if (index_size >= 1)
			tcam_fout << starAscii[i]._index[index_size - 1] << ")" << endl;

	}

	//print data TCAM blocks
	size_t ss_block_num = 0, ascii_block_size = _ascii_blocks_2.size(),
			size_temp = 0;
	for (size_t i = 0; i < ascii_block_size; ++i) {
		size_temp = _ascii_blocks_2[i].size();
		ss_block_num += ceil(size_temp * 1.0 / block_size);
	}

	this->print_data_tcam_table_header(tcam_fout, block_count + ss_block_num);

	flag = 0;
	flag_count = 0;

	size_t total_blocks_count = ss_block_num;

	for (size_t i = 0; i < ss_block_num; ++i) {

		flag = 0;
		size_temp = _ascii_blocks_2[i].size();
		for (size_t j = 0; j < size_temp; ++j) {
			if (j == flag) {
				flag += block_size;
				tcam_fout << "# Block " << flag_count++ << endl;
			}
			tcam_fout << _compress_table_2[i][_ascii_blocks_2[i][j]]._src_code
					<< ", 1, " << total_blocks_count++ << ", (0)" << endl;
		}

	}
	for (size_t i = 0; i < _ascii_blocks_2.size(); ++i) {
		for (size_t j = 0; j < _ascii_blocks_2[i].size(); ++j) {
			tcam_fout << "# Block " << flag_count++ << endl;
			print_state_split_block_detail(tcam_fout, i, _ascii_blocks_2[i][j],
					_ascii_blocks_2[i][j], _compress_table_2);
		}
	}

	//print character index tcam
	fout << endl << block_size << "\t";
	size_t bits = _compress_table_2[0][0]._dst_code.size();
	size_t character_block_num = ceil(256 * 1.0 / _block_size);
	float ns = 0.0, nj = 0.0;
	this->tcam_parameters_init(_block_size, bits, character_block_num);
	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);
	ns = tcam_search_delay.max_delay * 1e9;
	nj = tcam_search_power.total_power * 1e9;
	fout << ns << "\t" << nj << "\t";

	vector<Tcam_Node> ss_block_array(65536);
	size_t ss_index_block_num = 0;
	size_t total_block_num = 0;
	float total_ns = 0.0, total_nj = 0.0;
	for (size_t i = 0; i < header.size(); ++i) {
		ss_index_block_num = ceil(_ascii_blocks_2[i].size() * 1.0 / block_size);
		tcam_parameters_init(_block_size, bits, ss_index_block_num);
		calculate_tcam_power(tcam_parameters, &tcam_search_power,
				&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
				&tcam_search_delay);
		ns = tcam_search_delay.max_delay * 1e9;
		nj = tcam_search_power.total_power * 1e9;

		total_block_num += ss_index_block_num * header[i].size();
		total_ns += ns * header[i].size();
		total_nj += nj * header[i].size();
		for (size_t j = 0; j < header[i].size(); ++j) {
			ss_block_array[header[i][j]]._ns = ns;
			ss_block_array[header[i][j]]._nj = nj;
			ss_block_array[header[i][j]]._block_num = ss_index_block_num;
			//cout << header[i][j] << " -> " << ss_index_block_num;
		}
	}

	stable_sort(ss_block_array.begin(), ss_block_array.end(), TcamNodeComp);
	float avg_ns = total_ns / 65536;
	float avg_nj = total_nj / 65536;
	size_t avg_block = ceil(total_block_num * 1.0 / 65536);

	fout << avg_ns << "\t" << avg_nj << "\t" << avg_block << "\t";

	fout << ss_block_array[65535]._ns << "\t" << ss_block_array[65535]._nj
			<< "\t" << ss_block_array[65535]._block_num << "\t";

	fout << ss_block_array[0]._ns << "\t" << ss_block_array[0]._nj << "\t"
			<< ss_block_array[0]._block_num << "\t";

	fout << (ss_block_array[32767]._ns + ss_block_array[32768]._ns) / 2 << "\t"
			<< (ss_block_array[32767]._nj + ss_block_array[32768]._nj) / 2
			<< "\t" << ss_block_array[32767]._block_num << "\t";

	tcam_parameters_init(_block_size, bits, 1);
	calculate_tcam_power(tcam_parameters, &tcam_search_power,
			&tcam_leakage_power, &tcam_write_power, &tcam_read_power,
			&tcam_search_delay);

	ns = tcam_search_delay.max_delay * 1e9;
	nj = tcam_search_power.total_power * 1e9;
	fout << ns << "\t" << nj << "\t" << total_block_num;
}

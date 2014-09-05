/*
 * Compress.cpp
 *
 *  Created on: Jul 20, 2014
 *      Author: Lhmily
 */

#include "../header/Compress.h"

Compress::Compress() {
	// TODO Auto-generated constructor stub
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

	vector<vector<CompressElement> >::iterator table_it =
			_compress_table.begin();
	vector<CompressElement>::iterator code_it;
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

		fout << endl;
		for (code_it = table_it->begin(); code_it != table_it->end();
				++code_it) {
			fout << code_it->_src_code << " -> " << code_it->_dst_code << "("
					<< code_it->_dst << ")" << endl;
		}
		++header_it;
	}

}

/*
 * AsciiNode.h
 *
 *  Created on: Sep 9, 2014
 *      Author: Lhmily
 */

#ifndef ASCIINODE_H_
#define ASCIINODE_H_
#include <string>
#include <vector>

using namespace std;

class AsciiNode {
public:
	AsciiNode();
	virtual ~AsciiNode();
	friend class Compress;
private:
	size_t _ascii;
	string _ascii_code;
	vector<size_t> _index;
	size_t _ascii_blocks_index;
};

#endif /* ASCIINODE_H_ */

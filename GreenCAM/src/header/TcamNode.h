/*
 * TcamNode.h
 *
 *  Created on: Sep 8, 2014
 *      Author: Lhmily
 */

#ifndef TCAMNODE_H_
#define TCAMNODE_H_
#include <string>

using namespace std;

class Tcam_Node {
public:
	Tcam_Node();
	virtual ~Tcam_Node();
	size_t getBits() const;
	void setBits(size_t bits);
	size_t getBlockNum() const;
	void setBlockNum(size_t blockNum);
	size_t getBlockSize() const;
	void setBlockSize(size_t blockSize);
	float getNj() const;
	void setNj(float nj);
	float getNs() const;
	void setNs(float ns);

	friend class Compress;
private:
	size_t _block_size;
	size_t _block_num;
	size_t _bits;
	float _ns;
	float _nj;

};

#endif /* TCAMNODE_H_ */

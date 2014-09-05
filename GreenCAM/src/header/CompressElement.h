/*
 * CompressElement.h
 *
 *  Created on: Sep 4, 2014
 *      Author: Lhmily
 */

#ifndef COMPRESSELEMENT_H_
#define COMPRESSELEMENT_H_
#include <string>

using namespace std;

class CompressElement {
private:
	string _src_code;
	string _dst_code;

	size_t _src;
	size_t _dst;

	size_t _order;

public:
	CompressElement();
	virtual ~CompressElement();
	size_t getDst() const;
	void setDst(size_t dst);
	const string& getDstCode() const;
	void setDstCode(const string& dstCode);
	size_t getOrder() const;
	void setOrder(size_t order);
	size_t getSrc() const;
	void setSrc(size_t src);
	const string& getSrcCode() const;
	void setSrcCode(const string& srcCode);

	friend class Compress;
	friend class TransTable;
};

#endif /* COMPRESSELEMENT_H_ */

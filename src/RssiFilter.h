/*
 * RssiFilter.h
 *
 *  Created on: 2015年4月7日
 *      Author: ghosty
 */

#ifndef RSSIFILTER_H_
#define RSSIFILTER_H_

class CRssiFilter {
public:
	CRssiFilter();
	virtual ~CRssiFilter();
	float Sampling(int value);
private:
	static const int filterLength = 10;
	int sampleValue[filterLength];
	int sampleIndex;
};

#endif /* RSSIFILTER_H_ */

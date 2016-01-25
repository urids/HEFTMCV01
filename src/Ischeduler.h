/*
 * Ischeduler.h
 *
 *  Created on: Jan 14, 2016
 *      Author: uriel
 */

#ifndef ISCHEDULER_H_
#define ISCHEDULER_H_
#include <iostream>
#include <map>
#include <vector>

class Ischeduler {
public:
	//virtual ~Ischeduler();
	virtual std::map<int,int> matchMake(int NumTsk, int NumDvs,float* W, int* Adj,std::vector<int> SR)=0;
};

#endif /* ISCHEDULER_H_ */

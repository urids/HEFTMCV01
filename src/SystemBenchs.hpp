/*
 * SystemBenchs.hpp
 *
 *  Created on: Jan 15, 2016
 *      Author: uriel
 */

#ifndef SYSTEMBENCHS_HPP_
#define SYSTEMBENCHS_HPP_
#include <vector>
#include "stdlib.h"

extern int NumTsk;
extern int NumDvs;

typedef struct systemBenchs_st{
	float * L_Mtx;
	float * BW_Mtx;
	float  L;
	float  BW;

	std::vector<int> AS;

}systemBenchs_t;


class SystemBench {
public:
	 SystemBench();
	 systemBenchs_t benchInfo;
	 void setBenchInfo();
	 systemBenchs_t& getBenchInfo();

};

#endif /* SYSTEMBENCHS_HPP_ */

/*
 * HEFTMC.hpp
 *
 *  Created on: Jan 14, 2016
 *      Author: uriel
 */

#ifndef HEFTMC_HPP_
#define HEFTMC_HPP_
#include <algorithm>
#include <list>
#include "Ischeduler.h"
#include "SystemBenchs.hpp"
#include <stdlib.h>
#include <map>



class HEFTMC : public Ischeduler{
public:
	virtual std::map<int,int> matchMake(int NumTsk, int NumDvs,float * W, int * Adj,std::vector<int> SR);
};


#endif /* HEFTMC_HPP_ */

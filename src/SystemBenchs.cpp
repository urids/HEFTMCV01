/*
 * SystemBenchs.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: uriel
 */

#include "SystemBenchs.hpp"

SystemBench::SystemBench(){
	SystemBench::setBenchInfo();
}

void SystemBench::setBenchInfo() {

	int i,j;

	//MY FAKE LATENCY && BW
		float * L_Mtx=(float*)malloc(NumTsk*NumDvs*sizeof(float*));
		for (i = 0; i < NumTsk; i++) {
			for (j = 0; j < NumDvs; j++) {
				L_Mtx[NumDvs*i+j]=.01;
			}
		}

		float * BW_Mtx=(float*)malloc(NumTsk*NumTsk*sizeof(float*));
		for (i = 0; i < NumTsk; i++) {
			for (j = 0; j < NumTsk; j++) {
				BW_Mtx[NumDvs*i+j] = 3.7;
			}
		}

		float L=0.25;
		float BW=1.2;
		this->benchInfo.L=L;
		this->benchInfo.BW=BW;
		this->benchInfo.L_Mtx=L_Mtx;
		this->benchInfo.BW_Mtx=BW_Mtx;
	//end myFake comm bench.

		//myFake AS[];

		for(i=0;i<NumDvs;i++){
			this->benchInfo.AS.push_back(512);
		}



}

 systemBenchs_t& SystemBench::getBenchInfo() {

	return this->benchInfo;
}

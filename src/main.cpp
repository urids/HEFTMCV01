/*
 * main.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: uriel
 */
#include "HEFTMC.hpp"
#include <iostream>
using namespace std;
int NumTsk;
int NumDvs;

int main(int argc, char **argv) {
	int i, j;
	NumTsk = 23;
	NumDvs = 7;


	float * W = (float*) malloc(NumTsk * NumDvs * sizeof(float));
	for (i = 0; i < NumTsk; i++) {
		for (j = 0; j < NumDvs; j++) {
			W[NumDvs * i + j] = 4;
		}
	}

	int * Adj = (int*) malloc(NumTsk * NumTsk * sizeof(int));
	FILE*fp = fopen("AdjMtx.csv", "r");
	if (fp != NULL) {
		for (i = 0; i < NumTsk; i++) {
			for (j = 0; j < NumTsk; j++) {
				fscanf(fp, "%d", &Adj[NumTsk * i + j]);
			}
		}
	}

	std::vector<int> SR;
	for (i = 0; i < NumTsk; i++) {
		SR.push_back(64);
	}

	Ischeduler *myScheduler = new HEFTMC();
	std::map<int, int> myMap = myScheduler->matchMake(NumTsk, NumDvs, W,
			Adj, SR);

	for_each(myMap.begin(), myMap.end(),
			[](pair<int,int> i) {cout << i.first <<"--"<< i.second << endl;});
	return 0;
}


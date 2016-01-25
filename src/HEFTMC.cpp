/*
 * HEFTMC.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: uriel
 */

#include "HEFTMC.hpp"

using namespace std;
int i,j;//index exclusive.


float get_WAvg(vector<float>  WT_i){
	float WAvg_i=0;
	vector<float>::iterator it;
	for(it=WT_i.begin();it!=WT_i.end();it++){
		WAvg_i+=*it;
	}
	return WAvg_i/WT_i.size();
}

void get_CAvg(float LAvg, float BWAvg, vector<float> Adj,vector<float> CAvg){
	vector<float>::iterator itC,itAdj;
	for(itC=CAvg.begin(),itAdj=Adj.begin(); itAdj!=Adj.end();itC++,itAdj++){
		*itC=LAvg+*itAdj/BWAvg;
	}
}

int getOD_v(vector<float> Adj){
	//vector<float>::iterator it1;
	return std::count_if(Adj.begin(), Adj.end(), [](float i)-> int {
		int od=0;
		if(i) od++;
		return od;
		});
}

bool pairCompare(const std::pair<int, float>& firstElem, const std::pair<int, float>& secondElem) {
	//We break (rank_u) tie by task ID.
	if(firstElem.second == secondElem.second)
		return firstElem.first < secondElem.first;
	else
		return firstElem.second > secondElem.second;


}

map<int,int> HEFTMC::matchMake(int NumTsk, int NumDvs,  float * W, int * AdjMtx, std::vector<int> SR){

		map<int,int> taskMap;
	/*************************
	* STEP 01
	*************************/
// compute average computation costs:
	std::vector<float> AvgWT;
	for(i=0;i<NumTsk;++i){
		std::vector<float> WT_i (W+NumDvs*i, W+(NumDvs*i+NumDvs));
		AvgWT.push_back(get_WAvg(WT_i));
	}

// compute average communication costs:
	SystemBench *SBs=new SystemBench();
	float Avg_L=SBs->benchInfo.L;
	float Avg_BW=SBs->benchInfo.BW;

	vector<float> AdjM(AdjMtx,AdjMtx+(NumTsk*NumTsk));
	vector<float> AvgC(NumTsk*NumTsk);
	get_CAvg(Avg_L, Avg_BW, AdjM, AvgC);

	/*************************
	 * STEP 02
	 *************************/
//compute the O_D(v).

	vector<int> OD_v;
	for(i=0;i<NumTsk;++i){
		std::vector<float> Adj_i (AdjM.begin()+NumTsk*i, AdjM.begin()+(NumTsk*i+NumTsk));
		OD_v.push_back(getOD_v(Adj_i));
	}

/*************************
 * STEP 03
 *************************/
//compute the rank_u starting from the exit task.


//3.1- Find the exit task.
	static int s = 0;
	find_if(OD_v.begin(), OD_v.end(),
			[](int i) {if(i==0) return true;
					   else {++s; return false;}});
//3.2- Compute the transposed graph
int *T_AdjMtx=(int*)malloc(NumTsk*NumTsk*sizeof(int));

	for(i=0;i<NumTsk;++i){
			for(j=0;j<NumTsk;++j){
				T_AdjMtx[NumTsk*j+i]=AdjMtx[NumTsk*i+j];
			}
	}

	list<pair<int,int> > *AdjList_T; // < successor, edgeCost >
	AdjList_T=new list<pair<int,int> >[NumTsk];

	for(i=0;i<NumTsk;++i){
		for(j=0;j<NumTsk;++j){
			if(T_AdjMtx[NumTsk*i+j]){
				AdjList_T[i].push_back(make_pair(j,T_AdjMtx[NumTsk*i+j]));
			}
		}
	}

/*for(int j=22;j>=0;j--){
	for_each(AdjList[j].begin(),AdjList[j].end(),[](pair<int,int> i){cout<<"--"<<i.first;});
cout<<endl;
}*/

//3.3 compute the upperRank //we can use BFS without marking visited because there is no cycles!!! =).
	vector<pair<int,float> > UR(NumTsk);
	for(i=0;i<NumTsk;i++)
		 UR[i]=make_pair(i,0);

	list<int> queue;

	queue.push_back(s);
	UR[s].second=AvgWT[s];

	list<pair<int,int> >::iterator i;
	while(!queue.empty()){
		s=queue.front();
		queue.pop_front();

		/*for(i=AdjList[s].begin();i!=AdjList[s].end();++i){
			queue.push_back(*i);
		}*/

		for_each(AdjList_T[s].begin(), AdjList_T[s].end(), [&queue,&UR,&AvgWT](pair<int,int> i) {
			//cout<<s<<"-"<<i.first<<endl;
			queue.push_back(i.first);
			float val=AvgWT[i.first]+ i.second+UR[s].second;
			UR[i.first].second=(UR[i.first].second<val)?val:UR[i.first].second;
			//cout << UR[i.first];
		});
	}

	/*************************
	 * STEP 04
	 *************************/
	//sort the task by UR.

	sort(UR.begin(),UR.end(),pairCompare);
	//for_each(UR.begin(),UR.end(),[](pair<int,int> i){cout<<i.first<<"  "<<i.second<<endl;});
	/*************************
	 * STEP 05,06
	 *************************/
	//schedule the tasks.

	vector<float> avail(NumDvs,0);
	vector<float> AFT(NumTsk,0);
	vector<float> EFT(NumDvs,0);
	vector<float> EST(NumTsk,0);
	//float *EST=(float*)calloc(NumTsk*NumDvs,sizeof(float));

	for_each(UR.begin(),UR.end(),[&SR,&SBs,&NumDvs,&NumTsk,&W, &AdjList_T, &AFT , &EST, &EFT, &avail, &taskMap](pair<int,float> ur){
		//STEP 07 create Qp.
		vector<int> Qp;
		for(int dev_i=0;dev_i<NumDvs;dev_i++){
			int ASDev_i=SBs->benchInfo.AS[dev_i];
			if(ASDev_i > SR[ur.first]){
				Qp.push_back(dev_i);
			}
		}
		//STEP 08 if there exists devices with space.
		if(Qp.size()>0){
			EFT.clear();
			//STEP 09 find the fastest device
			for_each(Qp.begin(),Qp.end(),[&ur,&NumDvs,&NumTsk,&W, &AdjList_T, &AFT,&EST, &EFT,&avail,&taskMap]
			                              (int dev){
			//STEP 10 compute the EFT
				//STEP 10.1 get the MAX time of all dependencies.
				float max_AST=0.0;
				//cout<<"TASK: "<<ur.first<<endl;
				for_each(AdjList_T[ur.first].begin(),AdjList_T[ur.first].end(),[&dev,&AFT,&max_AST]
				                                                             (pair<int,float> Pred_t){
					int BW=1,L=0;
					//cout<<"Pred:"<<Pred_t.first <<"--"<<AFT[Pred_t.first]<<endl;
					//cout<<endl<<Pred_t.first<<"--> "<<AFT[Pred_t.first]<<"+"<<Pred_t.second<<endl;
					max_AST=(max_AST > (AFT[Pred_t.first]+(Pred_t.second) )? max_AST : (AFT[Pred_t.first]+(Pred_t.second)));
				});
				//cout<<"max start time on device"<< dev <<": "<<max_AST<<endl;
				//STEP 10.2 compute the EST

				float est=(avail[dev]>max_AST)?avail[dev]:max_AST;
				cout<<est<<"- ";
				//cout<<dev<<"="<<avail[dev]<<","<<max_AST<<endl;
				//EST.push_back(est);
				EFT.push_back(W[NumDvs*ur.first+dev]+est);
			});
			//STEP 12.1 find the device that minimizes the EFT.
			int bestDev;
			cout <<endl;
			for_each(EFT.begin(),EFT.end(),[](int eft){cout<<eft<<"  ";});
			cout <<endl;
			bestDev=distance(EFT.begin(),min_element(EFT.begin(),EFT.end()));
			cout << "TASK: "<<ur.first <<" BestDevice: "<< bestDev <<endl;

			//STEP 12.2 Assign the task to it.
			taskMap[ur.first]=bestDev;
			AFT[ur.first]=EFT[bestDev];
			cout<<"tsk:"<<ur.first <<"--"<<AFT[ur.first]<<endl;
			avail[bestDev]=EFT[bestDev];
			for_each(avail.begin(),avail.end(),[](float av) {cout<<av<<"  ";});
			cout <<endl;
			cout <<endl;


		}
		else{
			cout << "Tasks could not be scheduled due to the lack of memory space";
			exit (EXIT_FAILURE);
		}

	});

	return taskMap;
}




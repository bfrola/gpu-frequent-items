#pragma once

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>

#include "Utils.h"

using namespace std;

void testReadRealData()
{
	//string datasetPath = "../../Datasets/retail.dat";
	//string datasetPath = "../../Datasets/nasa.dat";
	string datasetPath = "../../Datasets/q148.dat";
	//string datasetPath = "../../Datasets/kosarak.dat";

	vector<int> data;
	readDataset<int>(datasetPath, data);

	cout << data.size() << endl;
	cout << "MAX VAL: " << *max_element(data.begin(), data.end()) << endl; 
	
	system("pause");

}
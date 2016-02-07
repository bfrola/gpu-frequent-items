/* ******************************************************
/* ******************************************************

Frequent Items on GPU.
Sourceforge project: https://sourceforge.net/projects/figpu/

	This work is licensed under the Creative Commons
	Attribution-NonCommercial License. To view a copy of this license,
	visit http://creativecommons.org/licenses/by-nc/1.0/ or send a letter
	to Creative Commons, 559 Nathan Abbott Way, Stanford, California
	94305, USA.

	Algorithms and sources have been introduced and analysed in the following two scientific papers: 
	*** U. Erra and B. Frola, In Proceedings of The International Conference on Computational Science (ICCS), Omaha, Nebraska, USA, June 4-6, 2012.
		http://www.sciencedirect.com/science/article/pii/S1877050912001317
	*** G. Cormode, M. Hadjieleftheriou, Finding the frequent items in streams of data, Commun. ACM 52 (2009) 97–105
		http://dl.acm.org/citation.cfm?id=1454225
	Please, cite them in your publication/work/project. Thank you.

	Content of the "Frequent Items" Visual Studio Project was downloaded from:
	http://www2.research.att.com/~marioh/frequent-items/index.html

   ******************************************************/
/* ******************************************************

Author: Bernardino Frola
Created: 27-apr-2011

Check TestFrequentAlgorithm.h for an example of arguments.

/ *******************************************************/

#pragma once

#include <iostream>
#include <stdlib.h>

#include "TestStreamProcessor.h"
#include "TestPThread.h"
#include "TestStripProcessor.h"
#include "TestFrequentAlgorithm.h"
#include "TestThrust.h"
#include "TestSorting.h"

#include "TestReadRealData.h"

using namespace std;

int main(int argc, char **argv) 
{
	cout << "*** Frequent Items Experiment Application ***\n";

	 //Main function, runs experiments, produces output for analysis
	testFrequentAlgorithm(argc, argv);

	// Uncomment one of the following for unit testing

	//testReadRealData();
	//testStreamProcessor();
	//testPosixThreads();
	//testStripProcessor();
	//testThrust(1 << 22);
	//testThrustBinarySearch(1 << 24, 10);
	//testThrustBinarySearch(64, 10);
	//testSorting(1 << 10);

	system ("pause");
}


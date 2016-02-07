/* ******************************************************

Author: Bernardino Frola
Created: 27-apr-2011

/ *******************************************************/

#pragma once

#include <iostream>
#include <stdlib.h>

#include "cutil.h"

#include "FrequentStripProcessor.h"
#include "ParFreqStripProcessor.h"

#include "HelperPrint.h"

void testPerformancesOf(
	StripProcessor* sp, 
	StripDataVector<int>* data,
	int stripSize, int testRepeatitions)
{
	cout << endl << "////////////////////////////////////" << endl;

	// CUDA timer
	unsigned int timer;
	cutCreateTimer(&timer);
	cutResetTimer(timer);

	sp->initialize();

	cout << "Process start (repeat " << testRepeatitions << " times)" << endl;
	
	//for (int i = 0; i < testRepeatitions; i ++)
		//sp->process(data); // TODO 9: send data

	uint64_t nsecs;
	StartTheClock(nsecs);
	/*FrequentStripProcessor* spFreq = (FrequentStripProcessor*) sp;
	StripDataVector<int>* intData = (StripDataVector<int>*) data;
	
	for (size_t i = 0; i < intData->size(); ++i)
	{
		Freq_Update(spFreq->Freq(), (*intData)[i]);
	}*/
	sp->process(data);
	const float frequentElapsedTime = StopTheClock(nsecs) / testRepeatitions;

	const float updatesPerMillisecond = /*sp->getData()->size()*/
		stripSize / frequentElapsedTime; // TODO: 10 FIXTHIS
	const float memPerSecond = updatesPerMillisecond * sizeof(int) * 1000;

	cout << "Results:" << endl;
	cout << "\tElapsed time: " << frequentElapsedTime << "ms" << endl;
	cout << "\t\t" << updatesPerMillisecond << " updates/ms" << endl;
	cout << "\t\t" << memPerSecond / (1024 * 1024) << "Mb/s" << endl;

	sp->destroy();
	
}


void testStripProcessor()
{
	cout << "Strip Processor\n";

	//const int stripSize = (int) 1E1 * 1E3; // X * One thousand
	const int stripSize = 1000000; // X * One thousand
	const int testRep = 1;

	
	// Genarate data

	StripDataVector<int>* data = new StripDataVector<int>();
	for (int i = 0; i < stripSize; i ++)
	{
		data->push_back(100);
	}
	
	cout << "Size: " << stripSize << " " << data->size() << endl; 
	

	StripProcessor* spFrequent = new FrequentStripProcessor(0.01);
	ParFreqStripProcessor* spParFreq = new ParFreqStripProcessor(0.01, 2);
		
	//testPerformancesOf(spDummy, stripSize, testRep);
	testPerformancesOf(spFrequent, data, stripSize, testRep);
	//testPerformancesOf(spParFreq, stripSize, testRep);
	 
	PrintRes("SPFreq", ((FrequentStripProcessor*) spFrequent)->getOutput(1));
	
	delete spFrequent;
	delete spParFreq;

	// TODO: 8 Compare with result on paper and HH
	
}
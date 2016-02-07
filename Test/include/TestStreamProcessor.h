/* ******************************************************

Author: Bernardino Frola
Created: 15-feb-2011

/ *******************************************************/

#pragma once

#include <iostream>
#include <stdlib.h>

#include "TestPThread.h"
#include "FrequentStripProcessor.h"
#include "StreamProcessor.h"

#include "frequent.h"

using namespace BF;


void testNoStream(int dataSize)
{
	cout << "----------------" << endl;
	

	freq_type* freq = Freq_Init(0.01f);


	cout << "Direct" << endl;

	int timeUpdate = timeGetTime();

	for (int index = 0; index < dataSize; index ++)
	{
		Freq_Update(freq, index);
	}
	
	cout << "Complete in " << timeGetTime() - timeUpdate << "ms" << endl;

	Freq_Destroy(freq);
}

void testStreamProcessor()
{
	std::cout << "TestStreamProcessor" << std::endl;

	int dataSize = 1000;
	int stripSize = 100;
	
	StreamProcessor<int>* m_StreamProcessor = 
		new StreamProcessor<int> (new FrequentStripProcessor(0.01));

	// Emulate stream
	for (int index = 0; index < dataSize; index ++)
	{
		int element = index;
		m_StreamProcessor->addElement (element);
	}



	//m_StreamProcess

	delete m_StreamProcessor;

	testNoStream(dataSize);
}

// -------------------------------------------------------
// -------------------------------------------------------


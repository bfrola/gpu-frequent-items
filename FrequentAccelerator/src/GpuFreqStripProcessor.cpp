/* ******************************************************
   ******************************************************
   
Frequent Items on GPU.
Sourceforge project: https://sourceforge.net/projects/figpu/

	This work is licensed under the Creative Commons
	Attribution-NonCommercial License. To view a copy of this license,
	visit http://creativecommons.org/licenses/by-nc/1.0/ or send a letter
	to Creative Commons, 559 Nathan Abbott Way, Stanford, California
	94305, USA.

	Algorithms and sources have been introduced and analysed in the following scientific paper: 
	*** U. Erra and B. Frola, In Proceedings of The International Conference on Computational Science (ICCS), Omaha, Nebraska, USA, June 4-6, 2012.
		http://www.sciencedirect.com/science/article/pii/S1877050912001317
	Please, cite this paper in your publication/work/project. Thank you.

   ******************************************************/
/* *****************************************************

Author: Bernardino Frola
Created: 10-may-2011

/ *******************************************************/

#include "GPUFreqStripProcessor.h"
#include "ParFreqStripPRocessor.h"

GPUFreqStripProcessor::GPUFreqStripProcessor (float phi, int gpuStreamsCount) : 
FrequentStripProcessor(phi)
{
	m_GpuFreq = new GPUFrequent(phi, gpuStreamsCount);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

GPUFreqStripProcessor::~GPUFreqStripProcessor()
{
	delete m_GpuFreq;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void GPUFreqStripProcessor::initialize(){}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void GPUFreqStripProcessor::process(StripData* data)
{
	m_GpuFreq->update(data, m_StripSize, m_StripOffset);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void GPUFreqStripProcessor::destroy(){}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType GPUFreqStripProcessor::getOutput(int thresh)
{
	int poolSize = m_GpuFreq->getStreamCount();
	
	// First element
	OutputType tempOutput = m_GpuFreq->getOutput(thresh, 0);
	//return tempOutput;
	
	// Now: O(n); Should be: O(log(n)) 
	for (int i = 1; i < poolSize; i++) 
	{
		OutputType currOutput = m_GpuFreq->getOutput(thresh, i);
		
		tempOutput = ParFreqStripProcessor::reductionOperator(
			tempOutput, currOutput, m_GpuFreq->getK());
	}

	return tempOutput;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

class OutputThreadArgs 
{
public:
	GPUFrequent* m_GpuFreq;
	int m_Index;
	int m_Size;
	int m_Thresh;
	OutputType* m_Output;
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// Apply reductionOperator to threadArgs->m_Size elements starting from 
// threadArgs->m_Index * threadArgs->m_Size 
inline void* parallelOutputProcess(void* args) {
	OutputThreadArgs* threadArgs = 
		(OutputThreadArgs*) args;

	int indexBase = threadArgs->m_Index * threadArgs->m_Size;

	OutputType* output = new OutputType();
	*output = threadArgs->m_GpuFreq->getOutput(
			threadArgs->m_Thresh, indexBase);

	for (int i = 1; i < threadArgs->m_Size; i++) 
	{
		int currIndex = indexBase + i;
		OutputType currOutput = threadArgs->m_GpuFreq->getOutput(
			threadArgs->m_Thresh, currIndex);
		
		*output = ParFreqStripProcessor::reductionOperator(
			*output, currOutput, threadArgs->m_GpuFreq->getK());
	}

	//cout << "Par output [" << indexBase << 
	//	"x" << threadArgs->m_Size << "] -> " << 
	//	output->size() << endl;

	threadArgs->m_Output = output;
	return 0;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// Parallel version of getOutput
OutputType GPUFreqStripProcessor::getOutput2(int thresh)
{
	int poolSize = m_GpuFreq->getStreamCount();
	
	// This just init the output generation
	m_GpuFreq->getOutput(thresh, 0);

	int threadsCount = 8;

	OutputType* threadOutput = new OutputType[threadsCount];
	int outputsPerThread = poolSize / threadsCount;

	pthread_t* threadObject = new pthread_t[threadsCount];
	OutputThreadArgs** threadArgs = new OutputThreadArgs*[threadsCount];

	for (int i = 0; i < threadsCount; i++) {
		// Setup thread args
		threadArgs[i] = new OutputThreadArgs();
		threadArgs[i]->m_GpuFreq = m_GpuFreq;
		threadArgs[i]->m_Index = i;
		threadArgs[i]->m_Size = outputsPerThread;		
		threadArgs[i]->m_Thresh = thresh;
		
		// Launch the thread
		pthread_create(&threadObject[i], NULL, 
			parallelOutputProcess, threadArgs[i]);	
	}

	// Wait for the end of threads
	for (int i = 0; i < threadsCount; i++) pthread_join(threadObject[i], NULL);

	OutputType tempOutput = *threadArgs[0]->m_Output;
	
	// First element
	for (int i = 1; i < threadsCount; i++) 
	{
		tempOutput = ParFreqStripProcessor::reductionOperator(
			tempOutput,  *threadArgs[i]->m_Output, m_GpuFreq->getK());
	}

	// Cleaning
	for (int i = 0; i < threadsCount; i ++) delete threadArgs[i];
	delete [] threadOutput;
	delete [] threadArgs;
	delete [] threadObject;

	return tempOutput;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int GPUFreqStripProcessor::getSize()
{
	// TODO 7: get size of host and device memory of T
	return m_GpuFreq->getMemSize();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
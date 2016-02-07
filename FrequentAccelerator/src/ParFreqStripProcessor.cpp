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
Created: 27-apr-2011

/ *******************************************************/

#include "ParFreqStripProcessor.h"
#include "SFStripProcessor.h"


ParFIStripProcessor::ParFIStripProcessor (float phi, int poolSize, FISPFactory* fispFactory) : 
	FIStripProcessor(phi), m_PoolSize(poolSize)
{
	m_ThreadPool = new pthread_t[m_PoolSize];
	m_ThreadArguments = new ParallelProcessArguments*[m_PoolSize];

	// Allocate the pool
	for (int i = 0; i < m_PoolSize; i ++)
	{
		// Setup thread arguments
		m_ThreadArguments[i] = new ParallelProcessArguments();
		m_ThreadArguments[i]->m_SpFreq = fispFactory->getNewSPInstance(phi);
		
	}

}

ParFIStripProcessor::~ParFIStripProcessor()
{
	for (int i = 0; i < m_PoolSize; i ++) delete m_ThreadArguments[i];
	// delete []: delete all the elements (pointers) 
	// no implicit call to delete on pointers
	delete [] m_ThreadArguments; 
}


void ParFIStripProcessor::initialize()
{
	cout << "ParFIStripProcessor::initialize\n";
}



// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

inline void* parallelProcess(void* threadArgs) {

	ParallelProcessArguments* processArgs = 
		(ParallelProcessArguments*) threadArgs;

	//uint64_t nsecs;
	//StartTheClock(nsecs);
	// Just process the element of the pool
	processArgs->m_SpFreq->process(processArgs->m_Data);
	
	return 0;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void ParFIStripProcessor::process(StripData* data)
{
	// TODO 7: Check typeid of data here
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
			
	// Same of FrequentStripProcessor
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size(); 
	stripSize /= m_PoolSize;

	for (int i = 0; i < m_PoolSize; i++) {

		// Get the i-th element of the pool
		m_ThreadArguments[i]->m_SpFreq->StripSize(stripSize);
		m_ThreadArguments[i]->m_SpFreq->StripOffset(m_StripOffset + stripSize * i);
		m_ThreadArguments[i]->m_Data = data;
		
		// Launch the thread
		pthread_create(&m_ThreadPool[i], NULL, parallelProcess, m_ThreadArguments[i]);	
	}

	// Wait for the end of all the threads
	for (int i = 0; i < m_PoolSize; i++) pthread_join(m_ThreadPool[i], NULL);
	
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void ParFIStripProcessor::destroy()
{
	cout << "ParFIStripProcessor::destroy\n";
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void localPrintRes(string title, OutputType res)
{
	cout << endl << " -------------- " << title << "(" << res.size() << ")" << endl;
	OutputType::iterator it;
	for (it = res.begin(); it != res.end(); ++it)
	{
		cout << 
			it->first << " [x" << 
			it->second << "] ";
	}
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int ParFIStripProcessor::getSize()
{
	return m_ThreadArguments[0]->m_SpFreq->getSize() * m_PoolSize;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int ParFIStripProcessor::getK()
{
	return m_ThreadArguments[0]->m_SpFreq->getK();
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// ParFreqStripProcessor

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

ParFreqStripProcessor::ParFreqStripProcessor (float phi, int poolSize) : 
	ParFIStripProcessor(phi, poolSize, new FrequentSPFactory())
{
	
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType ParFreqStripProcessor::reductionOperator(OutputType res1, OutputType res2, int k)
{
	OutputType outRes;
	OutputType tempRes;

	if (res1.size() == 0 && res2.size() == 0)
		return outRes;
	if (res1.size() == 0)
		return res2;
	if (res2.size() == 0)
		return res1;

	// DEBUG
	//localPrintRes("R1:", res1);
	//localPrintRes("R2:", res2);


	// ------------------------------------------

	int minCounter = res1.begin()->second;

	FreqOutputIt it;

	// Add element of res1 to tempRes
	for (it = res1.begin(); it != res1.end(); ++it)
	{
		tempRes[it->first] += it->second;

		// Update minCounter
		minCounter = it->second < minCounter ? it->second : minCounter;
	}

	// Add element of res2 to tempRes
	for (it = res2.begin(); it != res2.end(); ++it)
	{
		tempRes[it->first] += it->second;

		// Update minCounter
		minCounter = it->second < minCounter ? it->second : minCounter;
	}

	// ------------------------------------------
	
	// TODO 5: Improve perfomances of this operation
	for (it = tempRes.begin(); it != tempRes.end(); ++it)
	{
		while (it->second > 0)
		{
			// If there is enought available space 
			if (outRes.size() < k)
			{
				outRes[it->first] = it->second;
				it->second = 0;
			} 
			else
			// Not enought available space
			{
				minCounter = it->second < minCounter ? it->second : minCounter;
				FreqOutputIt it2;

				vector<int> keysToErase;
				for (it2 = outRes.begin(); it2 != outRes.end(); ++it2)
				{
					//FreqOutputIt erase_element = it2++;
					it2->second -= minCounter;

					if (it2->second == 0)
						keysToErase.push_back(it2->first);
					else
						minCounter = it2->second < minCounter ? it2->second : minCounter;
				} // for outRes
			
				// Erase elements of map whose keys are in keysToDelete
				vector<int>::iterator it3;
				for (it3 = keysToErase.begin(); it3 != keysToErase.end(); ++it3)
				{
					outRes.erase(*it3); // TODO 5: Slow operation (??) If yes, use bitmaps
				}
				// TODO 6: decrease counters in temp?
			} // while
		} // else
	} // for tempRes
	
	return outRes;
}


OutputType ParFreqStripProcessor::getOutput(int thresh)
{
	// First element
	OutputType tempOutput = m_ThreadArguments[0]->m_SpFreq->getOutput(thresh);

	int k = m_ThreadArguments[0]->m_SpFreq->getK();

	// p = m_PoolSize -> require p - 1 steps (not the best solution)
	// should be: ln(p), using binary reduction
	// reductionSteps = log(((double) m_PoolSize)) / log(2.0);
	for (int i = 1; i < m_PoolSize; i++) 
	{
		tempOutput = reductionOperator(
			tempOutput, m_ThreadArguments[i]->m_SpFreq->getOutput(thresh), k);
	}

	return tempOutput;
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

ParSSStripProcessor::ParSSStripProcessor (float phi, int poolSize) : 
	ParFIStripProcessor(phi, poolSize, new SSSPFactory())
{
	m_OutputComposer = new OutuputComposer(m_Phi, getK() * m_PoolSize);
}
	
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

ParSSStripProcessor::~ParSSStripProcessor()
{
	delete m_OutputComposer;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType ParSSStripProcessor::reductionOperator(OutputType res1, OutputType res2, int k)
{
	return OutputType();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutuputComposer::OutuputComposer(float phi, int stripSize) : GpuSFStripProcessor(phi, stripSize) {}
	
// -------------------------------------------------------------------------

void OutuputComposer::fillT(ParallelProcessArguments* threadArgs[], int poolSize, int thresh)
{
	int k = threadArgs[0]->m_SpFreq->getK();

	thrust::host_vector<int> h_items(m_T.size());
	thrust::host_vector<int> h_counters(m_T.size());
		
	int thSize = poolSize * k;
	cout << "FillT: size: " << m_T.size() << " th. size: " << thSize << " strip size: " << getStripSize() << endl;

	for (int i = 0; i < poolSize; i ++)
	{
		OutputType currRes = threadArgs[i]->m_SpFreq->getOutput(0);
		OutputType::iterator it;

		if (i == 0) // just for the fist k items
			m_MinCounter = currRes.begin()->second;

		int j = 0;
		for (it = currRes.begin(); it != currRes.end(); ++it)
		{
			int currIndex = j + k * i;
			//cout << currIndex << " = " << j << " + " << k << " * " << i << " => " << it->first << " x " << it->second << endl;

			h_items[currIndex] = it->first;
			h_counters[currIndex] = it->second;

			if (i == 0) // just for the fist k items
				m_MinCounter = min<int>(m_MinCounter, it->second);

			j ++;
		}
	}


	thrust::copy(h_items.begin(), h_items.begin() + m_T.size(), 
		m_T.items().begin());
			
	thrust::copy(h_counters.begin(), h_counters.begin() + m_T.size(), 
		m_T.counters().begin());

}
	
// -------------------------------------------------------------------------

void OutuputComposer::processSubStrip(StripDataVector<int>* data, int scOffset, int scSize)
{
	incrementCounters(-m_MinCounter); // Method fillT computes the value of m_MinCounter
	sortStripByItems();	
	reduceStripOV();
	sortStripByCounters();
	incrementCounters(m_MinCounter);
}
	
// -------------------------------------------------------------------------



OutputType ParSSStripProcessor::getOutput(int thresh)
{
	m_OutputComposer->fillT(m_ThreadArguments, m_PoolSize, thresh);
	m_OutputComposer->processSubStrip(NULL, 0, 0);

	OutputType finalOutput = m_OutputComposer->getOutput(thresh);
			
	return finalOutput;
}
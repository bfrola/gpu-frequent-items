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

#include "FrequentStripProcessor.h"

FrequentStripProcessor::FrequentStripProcessor (float phi) : FIStripProcessor(phi)
{
	m_Freq = Freq_Init(m_Phi);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

FrequentStripProcessor::~FrequentStripProcessor()
{
	Freq_Destroy(m_Freq);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void FrequentStripProcessor::initialize() { }

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void FrequentStripProcessor::process(StripData* data)
{
	// TODO 7: Check typeid of data here
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size();
	 
	int* testVector = &(*intData)[m_StripOffset]; // TODO 9: TEST OK (delete)

	for (int index = m_StripOffset; index < stripSize + m_StripOffset; index ++)
	{
		Freq_Update(m_Freq, (*intData)[index]);
	}
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType FrequentStripProcessor::reductionOperator(OutputType res1, OutputType res2)
{
	OutputType outRes;
	OutputType tempRes;

	if (res1.size() == 0 && res2.size() == 0)
		return outRes;
	if (res1.size() == 0)
		return res2;
	if (res2.size() == 0)
		return res1;

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
			if (outRes.size() < getK())
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

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void FrequentStripProcessor::destroy() { }


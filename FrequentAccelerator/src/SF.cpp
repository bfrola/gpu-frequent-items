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
Created: 27-jun-2011

/ *******************************************************/

#include "SF.h"
#include <math.h>

using namespace BF;

SF::SF ()
{}

SF::SF (float phi)
{
	m_K = getK(phi);	
	int targetTSize = getNextMultipleOf(
		m_K * 2, m_TSizeMultipleOf);
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void SF::initialize() {}
void SF::destroy() {}
	
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

SF::SF (float phi, int stripSize)
{
	m_K = getK(phi);

	int targetTSize = m_K + 1 + stripSize;
	// Multiple of 512
	m_TSize = getNextMultipleOf(targetTSize, 512);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int SF::getStripSize()
{
	return m_TSize - m_K - 1;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int SF::getNextMultipleOf(int value, int multipleOf)
{
	return multipleOf * ceil ( (float) value / multipleOf);
	//return (int) pow(2.0, ceil(log((double) value) / log(2.0) ));
}

// ------------------------------------------------------------------------- 
// -------------------------------------------------------------------------

int SF::getK(float phi)
{
	return  (int) ceil (1.0/phi);
}

// ------------------------------------------------------------------------- 
// -------------------------------------------------------------------------

void SF::processSubStrip(StripDataVector<int>* intData, int scOffset, int scSize)
{
	// Get the starting pointer
	fillStrip(intData, scOffset, scSize);

	int minCounter = getIncDecValue();

	incrementCounters(-minCounter);
	
	sortStripByItems();
	
	reduceStrip();

	sortStripByCounters();

	incrementCounters(minCounter);

}

// ------------------------------------------------------------------------- 
// -------------------------------------------------------------------------

void SF::process(StripData* data)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size();
	
	int subStripSize = getStripSize();	
	int subStripCount = stripSize / subStripSize;
	// Print these information with the method printSubStripInfo

	int subScriptIdx;
	for (subScriptIdx = 0; subScriptIdx < subStripCount; subScriptIdx ++)
	{
		processSubStrip(intData, m_StripOffset + subScriptIdx * subStripSize, subStripSize);
	}

	int remaining = stripSize - subScriptIdx * subStripSize;
	if (remaining > 0)
	{
		processSubStrip(intData, m_StripOffset + subScriptIdx * subStripSize, remaining);
	}

	/*
	cout << "Data: " << stripSize << " m_K: " << m_K << 
		" |T|: " << m_T.size() <<
		" subStrip: " << subStripCount << "x" << subStripSize <<
		"+" << remaining << 
		endl;
	*/
	
}

	
void SF::processSingle(StripData* data)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size();

	processSubStrip(intData, m_StripOffset, stripSize);
}
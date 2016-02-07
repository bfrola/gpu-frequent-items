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
Created: 07-sept-2011

/ *******************************************************/

#include "SSStripProcessor.h"

SSStripProcessor::SSStripProcessor (float phi) : FIStripProcessor(m_Phi)
{
	m_SS = LCU_Init(phi);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

SSStripProcessor::~SSStripProcessor()
{
	LCU_Destroy(m_SS);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void SSStripProcessor::initialize() { }

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void SSStripProcessor::process(StripData* data)
{
	// TODO 7: Check typeid of data here
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size();
	 
	int* testVector = &(*intData)[m_StripOffset]; // TODO 9: TEST OK (delete)

	for (int index = m_StripOffset; index < stripSize + m_StripOffset; index ++)
	{
		LCU_Update(m_SS, (*intData)[index]);
	}
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void SSStripProcessor::destroy() { }


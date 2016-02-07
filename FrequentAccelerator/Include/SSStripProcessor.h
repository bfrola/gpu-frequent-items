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

#pragma once

#include "FIStripProcessor.h"
#include "lossycount.h"
#include "FrequentCommonTypes.h"

using namespace BF;

namespace BF
{
	class SSStripProcessor : public FIStripProcessor
	{
		// Constructor-destructor
	public:
		SSStripProcessor (float phi);
		~SSStripProcessor();

		// Fields
	protected:
		LCU_type* m_SS;

		// Methods
	public:
		void initialize();
		void process(StripData* data);
		void destroy();

	public:
		OutputType getOutput(int thresh) { return LCU_Output(m_SS, thresh); }
		int getSize() { return LCU_Size(m_SS); }
		int getK() { return m_SS->k; }

		LCU_type* Freq() const { return m_SS; }
		void Freq(LCU_type* val) { m_SS = val; }
	};
}
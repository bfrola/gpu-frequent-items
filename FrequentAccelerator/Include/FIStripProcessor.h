/* ******************************************************
/* ******************************************************

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

/* ******************************************************
/* *****************************************************

Author: Bernardino Frola
Created: 09-sept-2011

Frequent Items Strip Processor
Interface for strip processors

/ *******************************************************/

#pragma once

#include "StripProcessor.h"
#include "FrequentCommonTypes.h"

using namespace BF;

namespace BF
{
	class FIStripProcessor : public StripProcessor
	{
		// Constructor-destructor
	public:
		FIStripProcessor (float phi) : m_Phi(phi) {}
		~FIStripProcessor() {}

		// Fields
	protected:
		float m_Phi;

		// Required abstract methods
	public:
		void initialize() = 0;
		void destroy() = 0;
		
		// Update 
		virtual void process(StripData* data) = 0;
		
		// Query
		virtual OutputType getOutput(int thresh) = 0;

		virtual int getSize() = 0;
		virtual int getK() = 0;
	};
}

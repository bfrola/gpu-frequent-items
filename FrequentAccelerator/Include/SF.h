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

   ******************************************************
   ******************************************************/
/* *****************************************************

Author: Bernardino Frola
Created: 27-jun-2011

/ *******************************************************/

#pragma once

#include "StripProcessor.h"
#include <iostream>

using namespace std;

namespace BF
{

	// Base class for SFStripProcessor and GpuSFStripProcessor
	class SF : public StripProcessor
	{
	
		// Constructor-destructor
	public:

		SF ();
		SF (float phi);
		SF (float phi, int stripSize);

		// Methods
	public:
		int getNextMultipleOf(int value, int multipleOf);
		int getK(float phi);		

		int getStripSize();

		// StripProcessor methods
		void initialize();
		void destroy();
		void process(StripData* data);
		void processSingle(StripData* data);

		// Sub-steps of processSubStrip
		virtual void fillStrip(StripDataVector<int>* intData, int scOffset, int scSize) = 0;
		virtual int getIncDecValue() = 0;
		virtual void incrementCounters(int value) = 0;
		virtual void sortStripByItems() = 0;
		virtual void reduceStrip() = 0;
		virtual void sortStripByCounters() = 0;
		

		// Default combination of sub-steps 
		virtual void processSubStrip(StripDataVector<int>* intData, int scOffset, int scSize);

		// Fields
	protected:
		int m_K;
		// Only T size, T depends on extensions
		int m_TSize;
		static const int m_TSizeMultipleOf = 512;

	};
}
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

   ******************************************************/
/* *****************************************************

Author: Bernardino Frola
Created: 10-05-2011

/ *******************************************************/

#pragma once

#include "FrequentStripProcessor.h"
#include "GPUFrequent.h"

using namespace BF;

namespace BF
{
	class GPUFreqStripProcessor : public FrequentStripProcessor
	{
		// Constructor-destructor
	public:
		GPUFreqStripProcessor (float phi, int gpuStreamsCount);
		~GPUFreqStripProcessor();
		// Fields
	protected:
		GPUFrequent* m_GpuFreq;

		// Methods
	public:
		void initialize();
		// Override the method of FrequentStripProcessor
		void process(StripData* data);
		void destroy();
		
		// Override methods of FrequentStripProcessor
		OutputType getOutput(int thresh);
		OutputType getOutput2(int thresh);
		
		int getSize();
	};
}

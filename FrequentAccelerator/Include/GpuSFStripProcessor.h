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
Created: 30-jun-2011

/ *******************************************************/

#pragma once

#include "StripProcessor.h"
#include "FrequentCommonTypes.h"
#include "SF.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

using namespace BF;

namespace BF
{
	template <class Type>
	class device_T
	{
		int m_Size;

		thrust::device_vector<Type> d_Items;
		thrust::device_vector<Type> d_Counters;
	public:
		device_T () {}

		device_T (int size) : m_Size(size)
		{
			resize(m_Size);
		}

		//thrust::host_vector<Type>& h_items() { return h_Items; }
		//thrust::host_vector<Type>& h_counters() { return h_Counters; }

		thrust::device_vector<Type>& items() { return d_Items; }
		thrust::device_vector<Type>& counters() { return d_Counters; }

		
		int size() { return m_Size; }
		

		void resize(int size);
		void set(int pos, Type item, Type counter);

		string toStream(string caption, int sizeLimit = 0);
		void print(string caption = "----------------------", int sizeLimit = 0);


	};

	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------

	// Sorting Frequent Strip Processor
	class GpuSFStripProcessor : public SF
	{
		// Constructor-destructor
	public:
		GpuSFStripProcessor (float phi, int stripSize);
		~GpuSFStripProcessor();

		// Fields
	protected:
		device_T<int> m_T;
		bool m_EnableNewEntriesAdv;
		
		// Methods
	public:

		void printSubStripInfo(StripData* data);
		void printT(string caption, int sizeLimit = 0);

		// Sub-steps of processSubStrip
		void fillStrip(StripDataVector<int>* intData, int scOffset, int scSize);
		int getIncDecValue();
		void incrementCounters(int value);
		void sortStripByItems();
		void reduceStrip();
		void sortStripByCounters();
		
		void reduceStripOV();
		//void processSubStrip(StripDataVector<int>* intData, int scOffset, int scSize);
		 

	public:
		virtual OutputType getOutput(int thresh);
		virtual int getSize();
		void setEnableNewEntriesAdv(bool enableNewEntriesAdv) { m_EnableNewEntriesAdv = enableNewEntriesAdv; } // TODO: Move to SF
	};
}
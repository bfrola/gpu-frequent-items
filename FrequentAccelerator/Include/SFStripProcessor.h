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
#include "FrequentCommonTypes.h"
#include "SF.h"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace BF;

namespace BF
{
	template <class Type>
	class T
	{
		int m_Size;
		vector<Type> m_Items;
		vector<Type> m_Counters;
	public:
		T () {}

		T (int size) : m_Size(size)
		{
			resize(m_Size);
		}
		vector<Type>& items() { return m_Items; }
		vector<Type>& counters() { return m_Counters; }
		int size() { return m_Size; }
		

		void resize(int size);
		void set(int pos, Type item, Type counter);

		string toStream(string caption);
		void print();
		void print(string caption);


	};

	// Sorting Frequent Strip Processor
	class SFStripProcessor : public SF
	{
		// Constructor-destructor
	public:
		SFStripProcessor (float phi, int stripSize);
		~SFStripProcessor();

		// Fields
	protected:
		T<int> m_T;
		T<int> m_SortedT;
		T<int> m_PackedT;
		vector<int> m_SortKey;
		vector<int> m_SortKeyFixed;

		// Methods
	public:
		//void initialize();
		//virtual void process(StripData* data);
		//void destroy();

		//virtual void processSubStrip(StripDataVector<int>* data, int scOffset, int scSize);
		
		// Sub-steps of processSubStrip
		void fillStrip(StripDataVector<int>* intData, int scOffset, int scSize);
		int getIncDecValue();
		void incrementCounters(int value);
		void sortStripByItems();
		void reduceStrip();
		void sortStripByCounters();
		

		

	public:
		virtual OutputType getOutput(int thresh);
		virtual int getSize() { return 3 * 2 * m_K * sizeof(int); }
	};


}
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

#include "SFStripProcessor.h"
#include <algorithm>
#include <math.h>
#include <Windows.h>



struct c_unique {
  int current;
  c_unique() {current=0;}
  int operator()() {return current++;}
} UniqueNumber;

SFStripProcessor::SFStripProcessor (float phi, int stripSize) : SF(phi, stripSize) 
{
	m_T.resize(m_TSize); // m_TSize computed by SF
	cout << "SF::TSize = " << m_T.size() << endl;

	m_SortedT.resize(m_T.size());
	m_PackedT.resize(m_T.size());
	
	m_SortKeyFixed.resize(m_T.size());
	generate (m_SortKeyFixed.begin(), m_SortKeyFixed.end(), UniqueNumber);

	m_SortKey.resize(m_T.size());
	// Copy m_SortKeyFixed to m_SortKey instead of regenerate it
	copy(m_SortKeyFixed.begin(), m_SortKeyFixed.end(), m_SortKey.begin());
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

SFStripProcessor::~SFStripProcessor()
{
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// unique_copy Binary predictate
class MyUniqueChecker {
public:
	OutputType* m_Counters;
	bool m_IsFirst;

	MyUniqueChecker(OutputType* counters)
	{
		m_Counters = counters;
		m_IsFirst = true;
	}
	bool operator()(int i, int j) 
	{
		(*m_Counters)[j] ++;

		if (m_IsFirst)
		{
			// First element
			(*m_Counters)[i] ++;
			m_IsFirst = false;
		}

		return i == j;
	}
};

// ---------------------------------------------------
// ---------------------------------------------------

// sort comparator
class MyCompareClass : public MyUniqueChecker {
public:
	MyCompareClass(OutputType* counters)
		: MyUniqueChecker(counters)
	{}

	bool operator()(int i, int j) 
	{
		return (*m_Counters)[i] > (*m_Counters)[j];
	}
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


// unique_copy Binary predictate
class MyUniqueCheckerSoA {
public:
	vector<int>* m_Counters;
	vector<int>* m_DestCounters;

	int m_Accumulator;

	//int m_WriteIndex;
	int m_WriteIndex;
	int m_ReadIndex;

	MyUniqueCheckerSoA(vector<int>* sourceCounters, vector<int>* destCounters)
	{
		m_Counters = sourceCounters;
		m_DestCounters = destCounters;
		m_WriteIndex = 0; // Start from the 2nd element
		m_ReadIndex = -1;
		m_Accumulator = 0;
		//(*m_DestCounters)[0] = (*m_Counters)[0]; // Add automaticly 1 to the first element
	}
	bool operator()(int i, int j) 
	{
		m_ReadIndex ++;

		// Temp print
		//cout << m_ReadIndex << "/" << m_WriteIndex << ") " << i << " --- " << j << " [" << 
		//	(*m_Counters)[m_ReadIndex] << "|" <<
		//	(*m_Counters)[m_ReadIndex + 1] << "|" <<
		//	(*m_DestCounters)[m_ReadIndex + 1] << "]" << endl;
				
		if (i != j)
		{
			//cout << "Write " << m_CurrIndex << endl;
			// Commit the acculated value + the previous value
			(*m_DestCounters)[m_WriteIndex] = 
				(*m_Counters)[m_ReadIndex] + m_Accumulator;
			m_WriteIndex ++;
			m_Accumulator = 0;
					
			return 0;
		}
						
		// Accumulate the occurrences (if not required? DEBUGGING)
		//if ((*m_Counters)[m_ReadIndex + 1] > 0)
		//{
			m_Accumulator += (*m_Counters)[m_ReadIndex];
		//}

		return 1;
	}
};

// sort comparator
class MyCompareClassSoA {
public:
	vector<int> m_SortVec;
	MyCompareClassSoA(vector<int>& sortVec)
	{
		m_SortVec = sortVec;
	}

	bool operator()(int i, int j) 
	{
		return m_SortVec[i] > m_SortVec[j];
	}
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


class SortKeyTransform {
public:
	vector<int> m_SoruceVec;
	SortKeyTransform(vector<int>& sourceVec) : m_SoruceVec(sourceVec)
	{}

	int operator()(int i) 
	{
		//cout << "seq: " << i << " >> " << (*m_SoruceVec)[i] << endl;
		return m_SoruceVec[i];
	}
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


struct incFunc
{
	int m_IncVal;
	incFunc(int incVal)
	{
		m_IncVal = incVal;
	}

    int operator()(int current)
    {
        return current + m_IncVal;
    }
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void SFStripProcessor::fillStrip(StripDataVector<int>* intData, int scOffset, int scSize)
{
	int* subData = &(*intData)[scOffset];

	// Copy subData in the last m_K elements of m_TItems
	copy(subData, subData + scSize, m_T.items().end() - scSize - 1);

	cout << "FILL" << m_T.size() << " " << m_K << " " << scSize << endl;
	
	// Set counters of new elemnents to 1
	fill(m_T.counters().end() - scSize - 1, m_T.counters().end() - 1, 1);
	
	// Last element of T set to (0, 0)
	m_T.set(m_T.size() - 1, 0, 0);

	
}

int SFStripProcessor::getIncDecValue()
{
	return m_T.counters()[m_K - 1];
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void SFStripProcessor::incrementCounters(int value)
{
	transform(m_T.counters().begin(), m_T.counters().begin() + m_K, m_T.counters().begin(), incFunc(value));
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------



void SFStripProcessor::sortStripByItems()
{
	// Copy sequence from 1 to N
	copy(m_SortKeyFixed.begin(), m_SortKeyFixed.end(), m_SortKey.begin());
	// Sort by m_TItems
	sort(m_SortKey.begin(), m_SortKey.end(), MyCompareClassSoA(m_T.items()));

	// Copy currentTItems and currentTCounters to m_TItems and m_TCounters 
	// based on sorting in sequence vector
	transform(m_SortKey.begin(), m_SortKey.end(),
		m_SortedT.items().begin(), SortKeyTransform(m_T.items()));

	transform(m_SortKey.begin(), m_SortKey.end(),
		m_SortedT.counters().begin(), SortKeyTransform(m_T.counters()));
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void SFStripProcessor::reduceStrip()
{
	// Compute unique and count repetitions
	vector<int>::iterator itemsEnd = 
		unique_copy(m_SortedT.items().begin(), m_SortedT.items().end(), 
			m_PackedT.items().begin(), 
			MyUniqueCheckerSoA(&m_SortedT.counters(), &m_PackedT.counters()));
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

		
void SFStripProcessor::sortStripByCounters()
{
	// Copy sequence from 1 to N
	copy(m_SortKeyFixed.begin(), m_SortKeyFixed.end(), m_SortKey.begin());
	// Sort by currentTCoutners
	sort(m_SortKey.begin(), m_SortKey.end(), MyCompareClassSoA(m_PackedT.counters()));

	// Copy currentTItems and currentTCounters to m_TItems and m_TCounters 
	// based on sorting in sequence vector
	transform(m_SortKey.begin(), m_SortKey.end(),
		m_T.items().begin(), SortKeyTransform(m_PackedT.items()));

	transform(m_SortKey.begin(), m_SortKey.end(),
		m_T.counters().begin(), SortKeyTransform(m_PackedT.counters()));
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// Uses Structure of Array (SoA)
/*
void SFStripProcessor::process(StripData* data)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size();
	
	int subStripSize = getStripSize();		// v2
	int subStripCount = stripSize / subStripSize;


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

	
}
*/

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType SFStripProcessor::getOutput(int thresh)
{
	OutputType res;

	//thresh = 0;

	for (int i = 0; i < m_K; i ++)
	{
		int count = m_T.counters()[i];

		if (count >= thresh)
			res.insert(OutputItemPair(m_T.items()[i], count));
	}

	return res;
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void T<Type>::resize(int size)
{
	m_Size = size;
	m_Items.resize(m_Size);
	m_Counters.resize(m_Size);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
string T<Type>::toStream(string caption)
{
	stringstream s;
	s << caption.c_str() << endl;
	for (int i = 0; i < size(); i ++)
	{
		s << i << ") " << items()[i] << ", " << counters()[i] << endl;
	}
	return s.str();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void T<Type>::print() { print("---------"); }

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void T<Type>::print(string caption) 
{
	ofstream fileStream;
	fileStream.open ("example.dat", ios::app);
	//cout << toStream(caption).c_str();
	fileStream << toStream(caption).c_str();
	fileStream.close();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void T<Type>::set(int pos, Type item, Type counter)
{
	items()[pos] = item;
	counters()[pos] = counter;
}

template class T<int>;
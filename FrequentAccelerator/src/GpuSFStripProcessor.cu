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
Created: 30-jun-2011

/ *******************************************************/

#include "GpuSFStripProcessor.h"
#include <algorithm>
#include <numeric>
#include <math.h>
#include <iomanip>
#include <thrust/sort.h>
#include <thrust/unique.h>
# include <thrust/iterator/constant_iterator.h>
#include <thrust/binary_search.h>

#include "vector_types.h"

#include "Utils.h"


struct c_unique {
  int current;
  c_unique() {current=0;}
  int operator()() {return current++;}
} UniqueNumber2;


// DEBUG REDUCE
device_T<int> tempT;

// TEMP for binary search
thrust::device_vector<int> bucket_begin;
thrust::device_vector<int> bucket_end;
thrust::device_vector<int> bucket_size;
thrust::device_vector<int> d_vec_unique;


GpuSFStripProcessor::GpuSFStripProcessor (float phi, int stripSize) : SF(phi, stripSize)
{
	m_T.resize(m_TSize); // Fixed
	cout << "GpuSF::TSize = " << m_T.size() << endl;
	m_EnableNewEntriesAdv = false;

	tempT.resize(m_TSize);

	bucket_begin.resize(m_TSize);
	bucket_end.resize(m_TSize);
	d_vec_unique.resize(m_TSize);
	bucket_size.resize(m_TSize);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

GpuSFStripProcessor::~GpuSFStripProcessor()
{
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


struct incFunc
{
	int m_IncVal;
	incFunc(int incVal)
	{
		m_IncVal = incVal;
	}

    __host__ __device__
    int operator()(int current)
    {
        return current + m_IncVal;
    }
};

struct computeCounter
{
	int* m_Items;
	int* m_Counters;

	computeCounter(int* itemsRef, int* countersRef)
	{
		m_Items = itemsRef;
		m_Counters = countersRef;
	}

    __host__ __device__
    int operator()(int x, int y)
    {
		if (m_Items[y] == 0)
			return 0;

		int diff = x - y - 1;	// Difference ignoring the first 1
		diff += m_Counters[y];	// Add the counter value instead of the first 1
        return diff;
    }
};


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void GpuSFStripProcessor::fillStrip(StripDataVector<int>* intData, int scOffset, int scSize)
{
	// Get the pointer to data
	int* subData = &(*intData)[scOffset];

	thrust::copy(subData, subData + scSize, m_T.items().end() - scSize - 1);

	// Set counters of new elemnents to 1
	thrust::fill(m_T.counters().end() - scSize - 1,	m_T.counters().end() - 1, 1);

	// Last element of T set to (0, 0)
	m_T.set(m_T.size() - 1, 0, 0); 
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int GpuSFStripProcessor::getIncDecValue()
{
	return m_T.counters()[m_K - 1];
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void GpuSFStripProcessor::incrementCounters(int value)
{
	thrust::transform(m_T.counters().begin(), m_T.counters().begin() + m_K, m_T.counters().begin(), incFunc(value));
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void GpuSFStripProcessor::sortStripByItems()
{
	thrust::sort_by_key(m_T.items().begin(), m_T.items().end(), m_T.counters().begin()); // sort order is irrilevant
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


void GpuSFStripProcessor::reduceStrip()
{
	// Binary search instead of reduce_by_key
	// See svn for old versions

	thrust::device_vector<int>::iterator d_vec_unique_end = 
		thrust::unique_copy(m_T.items().begin(), m_T.items().end(), d_vec_unique.begin());

	thrust::counting_iterator<int> search_begin(0);

	thrust::lower_bound(m_T.items().begin(), m_T.items().end(),
		d_vec_unique.begin(), d_vec_unique_end, 
		bucket_begin.begin());

	thrust::upper_bound(m_T.items().begin(), m_T.items().end(),
		d_vec_unique.begin(), d_vec_unique_end, 
		bucket_end.begin());

	thrust::transform(bucket_end.begin(), bucket_end.end(),
		bucket_begin.begin(), 
		bucket_size.begin(), 
		computeCounter(
			thrust::raw_pointer_cast(&m_T.items()[0]),
			thrust::raw_pointer_cast(&m_T.counters()[0])));

	thrust::copy(d_vec_unique.begin(), d_vec_unique_end, m_T.items().begin());
	thrust::copy(bucket_size.begin(), bucket_size.end(), m_T.counters().begin());

	int newUniqueSize = d_vec_unique_end - d_vec_unique.begin();
	int remaining = m_T.size() - newUniqueSize;

	thrust::fill(m_T.items().end() - remaining, m_T.items().end(), 0);
	thrust::fill(m_T.counters().end() - remaining, m_T.counters().end(), 0);
}

void GpuSFStripProcessor::reduceStripOV()
{
	thrust::pair<thrust::device_vector<int>::iterator, 
		thrust::device_vector<int>::iterator> newEnd = 
		thrust::reduce_by_key(
			m_T.items().begin(), m_T.items().end(), m_T.counters().begin(), 
			m_T.items().begin(), m_T.counters().begin());

	// Clean old values
	thrust::fill(newEnd.first, m_T.items().end(), 0);
	thrust::fill(newEnd.second, m_T.counters().end(), 0);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

		
void GpuSFStripProcessor::sortStripByCounters()
{
	thrust::sort_by_key(m_T.counters().begin(), m_T.counters().end(), m_T.items().begin(), thrust::greater<int>());
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// Print information on substrip size without affecting performances measurements
void GpuSFStripProcessor::printSubStripInfo(StripData* data)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int stripSize = (m_StripSize > 0) ? m_StripSize : intData->size();

	int subStripSize = getStripSize();	
	int subStripCount = stripSize / subStripSize;
	int remaining = stripSize - subStripCount * subStripSize;

	cout << "(Gpu) Data: " << stripSize << " m_K: " << m_K << 
		" |T|: " << m_T.size() <<
		" subStrip: " << subStripCount << "x" << subStripSize <<
		"+" << remaining << 
		endl;
}

void GpuSFStripProcessor::printT(string caption, int sizeLimit)
{
	m_T.print(caption, sizeLimit);
}

	
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

/*
// FOR DEBUGGING
void GpuSFStripProcessor::processSubStrip(StripDataVector<int>* intData, int scOffset, int scSize)
{
	cout << "*************************************************************************" << endl;
	cout << "From" << scOffset << " x " << scSize << endl;

	MultiTimer<false> mTimer; 
	mTimer.eventRecord("Start");
	
	// Get the pointer to data
	int* subData = &(*intData)[scOffset];



	thrust::copy(subData, subData + scSize, m_T.items().end() - scSize - 1);

	mTimer.eventRecord("Copy");

	// Set counters of new elemnents to 1
	thrust::fill(m_T.counters().end() - scSize - 1,	m_T.counters().end() - 1, 1);

	// Last element of T set to (0, 0)
	m_T.set(m_T.size() - 1, 0, 0); 

	int minCounter = m_T.counters()[m_K - 1];
	bool enableNewEntryAdv = m_EnableNewEntriesAdv && (minCounter > 0);
	
	int printTsize = 0;

	if (enableNewEntryAdv)
	{
		// First K counters of T decremented by minCounter
		thrust::transform(m_T.counters().begin(), m_T.counters().begin() + m_K, m_T.counters().begin(), incFunc(-minCounter));

		//int * countersRawPointer = thrust::raw_pointer_cast(&m_T.counters()[0]);
		// use ptr in a CUDA C kernel
		//incrementKernel<<<(m_K / 256) + 1, 256>>>(countersRawPointer, m_K, -minCounter);
		
		//m_T.print("DEC: after <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ", printTsize);

		mTimer.eventRecord("Decr");
	}
	

	// Sort by items -> items and counters in m_TItems
	thrust::sort_by_key(m_T.items().begin(), m_T.items().end(), m_T.counters().begin());

	m_T.print("SORT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ", printTsize);

	mTimer.eventRecord("Sort1");

	int position = 2085;

	cout << "BEFORE" << endl;
	cout << "\tItem[" << position << "] = " << m_T.items()[position] << endl;
	cout << "\tCoun[" << position << "] = " << m_T.counters()[position] << endl;


	thrust::device_vector<int>::iterator d_vec_unique_end = 
		thrust::unique_copy(m_T.items().begin(), m_T.items().end(), d_vec_unique.begin());

	thrust::counting_iterator<int> search_begin(0);

	thrust::lower_bound(m_T.items().begin(), m_T.items().end(),
		d_vec_unique.begin(), d_vec_unique_end, 
		bucket_begin.begin());

	thrust::upper_bound(m_T.items().begin(), m_T.items().end(),
		d_vec_unique.begin(), d_vec_unique_end, 
		bucket_end.begin());

	//thrust::fill(bucket_size.begin(), bucket_size.end(), 0);

	cout << "bucket_end: end() - begin() = " << (bucket_end.end() - bucket_end.begin()) << endl;

	thrust::transform(bucket_end.begin(), bucket_end.end(),
		bucket_begin.begin(), 
		bucket_size.begin(), 
		computeCounter(
			thrust::raw_pointer_cast(&m_T.items()[0]),
			thrust::raw_pointer_cast(&m_T.counters()[0])));

	cout << "AFTER1" << endl;
	cout << "\tItem[" << position << "] = " << m_T.items()[position] << endl;
	cout << "\tCoun[" << position << "] = " << m_T.counters()[position] << endl;

	mTimer.eventRecord("Reduce");	
		
	thrust::copy(d_vec_unique.begin(), d_vec_unique_end, m_T.items().begin());
	thrust::copy(bucket_size.begin(), bucket_size.end(), m_T.counters().begin());

	cout << "AFTER2" << endl;
	cout << "\tItem[" << position << "] = " << m_T.items()[position] << endl;
	cout << "\tCoun[" << position << "] = " << m_T.counters()[position] << endl;

	int newUniqueSize = d_vec_unique_end - d_vec_unique.begin();
	int remaining = m_T.size() - newUniqueSize;

	thrust::fill(m_T.items().end() - remaining, m_T.items().end(), 0);
	thrust::fill(m_T.counters().end() - remaining, m_T.counters().end(), 0);

	m_T.print("REDUCE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ", printTsize);

	mTimer.eventRecord("Fill");

	// Sort by counters -> items and counters
	thrust::sort_by_key(m_T.counters().begin(), m_T.counters().end(), m_T.items().begin(), thrust::greater<int>());

	//m_T.print("SORT2 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ", printTsize);

	mTimer.eventRecord("Sort2");
	
	if (enableNewEntryAdv)
	{
		// First K counters of T incremented by minCounter: an advatage to new entries
		thrust::transform(m_T.counters().begin(), m_T.counters().begin() + m_K, m_T.counters().begin(), incFunc(minCounter));

		//int * countersRawPointer = thrust::raw_pointer_cast(&m_T.counters()[0]);
		// use ptr in a CUDA C kernel
		//incrementKernel<<<(m_K / 256) + 1, 256>>>(countersRawPointer, m_K, minCounter);
		
		//m_T.print("INC: after <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ", printTsize);

		mTimer.eventRecord("Incr");
	}

	m_T.print("OUT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	
	mTimer.printTimes();	
}
*/

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType GpuSFStripProcessor::getOutput(int thresh)
{
	OutputType res;

	if (m_T.items().size() < m_K)
	{
		return res;
	}

	thrust::host_vector<int> h_items(m_K);
	thrust::copy(m_T.items().begin(), m_T.items().begin() + m_K, 
		h_items.begin());
	
	thrust::host_vector<int> h_counters(m_K);
	thrust::copy(m_T.counters().begin(), m_T.counters().begin() + m_K, 
		h_counters.begin());
	
	//thresh = 0;

	for (int i = 0; i < m_K; i ++)
	{
		int count = h_counters[i];

		if (count >= thresh)
			res.insert(OutputItemPair(h_items[i], count));
	}

	return res;
}

int GpuSFStripProcessor::getSize()
{
	return m_T.size() * sizeof(int) * 6; // items, counters, temp data for binary search (4 arrays)
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void device_T<Type>::resize(int size)
{
	m_Size = size;
	items().resize(m_Size);
	counters().resize(m_Size);
	//h_items().resize(m_Size);
	//h_counters().resize(m_Size);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
string device_T<Type>::toStream(string caption, int sizeLimit = 0)
{
	stringstream s;
	s << caption.c_str() << endl;

	if (sizeLimit == 0)
		sizeLimit = size();

	thrust::host_vector<int> h_items(sizeLimit);
	thrust::copy(items().begin(), items().begin() + sizeLimit, h_items.begin());
	
	thrust::host_vector<int> h_counters(sizeLimit);
	thrust::copy(counters().begin(), counters().begin() + sizeLimit, h_counters.begin());

	for (int i = 0; i < sizeLimit; i ++)
	{
		s << i << ") " << h_items[i]  << ", " << h_counters[i] << endl;
	}
	return s.str();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void device_T<Type>::print(string caption, int sizeLimit) 
{
	ofstream fileStream;
	fileStream.open ("example_gpu.dat", ios::app);
	//cout << toStream(caption).c_str();
	fileStream << toStream(caption, sizeLimit).c_str();
	fileStream.close();
	//cout << toStream(caption).c_str();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class Type>
void device_T<Type>::set(int pos, Type item, Type counter)
{
	items()[pos] = item;
	counters()[pos] = counter;

	//h_items()[pos] = item;
	//h_counters()[pos] = counter;
}

template class device_T<int>;

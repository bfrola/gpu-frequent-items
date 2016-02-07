#pragma once

#include <iostream>
#include <stdlib.h>

using namespace std;

// ---------------------------------------------------
// ---------------------------------------------------

// unique_copy Binary predictate
class MyUniqueChecker_x {
public:
	map<int, int>* m_Counters;
	bool m_IsFirst;

	MyUniqueChecker_x(map<int, int>* counters)
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
class MyCompareClass_x : public MyUniqueChecker_x {
public:
	MyCompareClass_x(map<int, int>* counters)
		: MyUniqueChecker_x(counters)
	{}

	bool operator()(int i, int j) 
	{
		return (*m_Counters)[i] > (*m_Counters)[j];
	}
};

// ---------------------------------------------------
// ---------------------------------------------------

int make_myrand(void){
	int randVal = 1024 * rand() / RAND_MAX;
	return randVal;
}

// ---------------------------------------------------
// ---------------------------------------------------

void testSorting(const int size)
{
	vector<int> data(size);

	vector<int> items(size);
	vector<int>::const_iterator it;

	map<int, int> counters; 
	
	generate(data.begin(), data.end(), make_myrand);
	fill(items.begin(), items.end(), -1);
	
	DWORD tStart = timeGetTime();
	sort(data.begin(), data.end());
	int tElapsed = timeGetTime() - tStart;
	cout << "Sort: " << tElapsed << "ms" << endl;
	
	vector<int>::iterator itemsEnd = 
		unique_copy(data.begin(), data.end(), items.begin(), 
			MyUniqueChecker_x(&counters));

	sort(items.begin(), itemsEnd, 
		MyCompareClass_x(&counters));

	for (it = items.begin(); it != itemsEnd; ++ it)
	{
		int idx = it - items.begin();
		cout << idx << ") " << 
			*it << ", " <<
			counters[*it] << endl;
		if (idx > 30)
			break;
	}	
	
} // testSorting

// ---------------------------------------------------
// ---------------------------------------------------

void testSortingSoA(const int size)
{
	vector<int> data(size);

	vector<int> items(size);
	vector<int>::const_iterator it;

	//map<int, int> counters; 


}
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

#include "TestThrust.h"

# include <thrust/host_vector.h>
# include <thrust/device_vector.h>
# include <thrust/sort.h>
# include <thrust/unique.h>
# include <thrust/iterator/constant_iterator.h>
#include <thrust/binary_search.h>

#include "Utils.h"

// return a random float2 in [0,1)^2float

struct randGenerator
{
	int multiply;
	randGenerator(int m)
	{
		multiply = m;
	}

	int operator()()
    {
		int val = multiply * rand() / RAND_MAX;
		//cout << val << endl;
        return val;
    }
};

int make_myrand2(void){
	return 1000000 * rand() / RAND_MAX;
}

void testThrust(const int size)
{
	// generate 16M random numbers on the host
	//thrust::host_vector<int> h_vec(1 << 24);
	//const int size = 1 << 20;

	cout << "Size: " << size << endl;

	thrust::host_vector<int> h_vec(size);
	thrust::host_vector<int> h_conuter(size);


	thrust::generate(h_vec.begin(), h_vec.end(), make_myrand2);
	cout << "rand generated" << endl;

	// transfer data to the device
	
	thrust::device_vector<int> d_vec= h_vec;
	thrust :: device_vector <int> d_counter (size);
	thrust::device_vector<int> d_vec_out(size);

	cudaEvent_t tStart, tSort, tReduce, tSearch;
	cudaEventCreate(&tStart);
	cudaEventCreate(&tSort);
	cudaEventCreate(&tReduce);
	cudaEventCreate(&tSearch);

	cudaEventRecord(tStart, 0);
	cudaEventSynchronize(tStart);
	
	// sort data on the device (805 Mkeys/sec on GeForce GTX 480)
	thrust::sort(d_vec.begin(), d_vec.end());

	cudaEventRecord(tSort, 0);
	cudaEventSynchronize(tSort);

	// allocate some random points in the unit square on the host
	thrust::reduce_by_key(d_vec.begin(), d_vec.end(),
		thrust::make_constant_iterator(1),
		d_vec_out.begin(),
		d_counter.begin());

	cudaEventRecord(tReduce, 0);
	cudaEventSynchronize(tReduce);

	
	// bucket_begin[i] indexes the first element of bucket i
	// bucket_end[i] indexes one past the last element of bucket i
	thrust::device_vector<int> bucket_begin(size);
	thrust::device_vector<int> bucket_end(size);
	// used to produce integers in the range [0, w*h)
	thrust::counting_iterator<int> search_begin(0);
	// find the beginning of each bucket's list of points
	thrust::lower_bound(d_vec.begin(), d_vec.end(),
		search_begin, search_begin + size, bucket_begin.begin());
	// find the end of each bucket's list of points
	thrust::upper_bound(d_vec.begin(), d_vec.end(),
		search_begin, search_begin + size, bucket_end.begin());

	// allocate space to hold per-bucket sizes
	thrust::device_vector<unsigned int> bucket_sizes(size);

	// take the difference between bounds to find each bucket’s size
	thrust::transform(
		bucket_end.begin(), bucket_end.end(),
		bucket_begin.begin(),
		bucket_sizes.begin(),
		minus<unsigned int>());
	// bucket_size[i] = bucket_end[i] –bucket_begin[i]
	// note each bucket’s size appears in the output even if it is empty

	cudaEventRecord(tSearch, 0);
	cudaEventSynchronize(tSearch);

	// transfer data back to host
	thrust::copy(d_counter.begin(), d_counter.end(), h_conuter.begin());
	thrust::copy(d_vec.begin(), d_vec.end(), h_vec.begin());
	
	cout << "D2H" << endl;

	float tElapsed;
	cudaEventElapsedTime(&tElapsed, tStart, tSort);
	cout << "sort:\t" << tElapsed << "ms\t(" << (size / tElapsed) << ")" << endl;
	cudaEventElapsedTime(&tElapsed, tSort, tReduce);
	cout << "reduce:\t" << tElapsed << "ms\t(" << (size / tElapsed) << ")" << endl;
	cudaEventElapsedTime(&tElapsed, tReduce, tSearch);
	cout << "Search:\t" << tElapsed << "ms\t(" << (size / tElapsed) << ")" << endl;
	cudaEventElapsedTime(&tElapsed, tStart, tSearch);
	cout << "Total:\t" << tElapsed << "ms\t(" << (size / tElapsed) << ")" << endl;


	//int maxSize = 150;
	//int lastElem = (h_vec.size() < maxSize) ? h_vec.size() : maxSize;
	//for ( int i = 0; i < maxSize ; i ++)
	//	std :: cout << i << " [" << h_vec[i] << ", " << h_conuter[i] << "]" << endl ;
	// H and D are automatically deleted when the function returns

	system("pause");
}

float make_myrandf(void){
	return 1000000 * rand() / RAND_MAX;
}

void testThrustBinarySearch(const int size, const int domainSize)
{
	MultiTimer<true> mTimer; 
	
	cout << "Size: " << size << endl;

	thrust::host_vector<float> h_vec(size);
	thrust::host_vector<float> h_conuter(size);

	thrust::device_vector<int> bucket_begin(size);
	thrust::device_vector<int> bucket_end(size);
	thrust::device_vector<int> d_vec_unique(size);

	thrust::generate(h_vec.begin(), h_vec.end(), randGenerator(domainSize));
	cout << "rand generated" << endl;

	//thrust::sequence(h_conuter.begin(), h_conuter.end());
	//cout << "sequence generated" << endl;

	//mTimer.eventRecord("Rand generation");
	mTimer.eventRecord("Start");

	thrust::device_vector<float> d_vec= h_vec;
	//thrust :: device_vector <float> d_counter = h_conuter;
	thrust::device_vector <float> d_counter(size);
	thrust::fill(d_counter.begin(), d_counter.end(), 1);

	mTimer.eventRecord("Copy H2D");

	//thrust::sort(d_vec.begin(), d_vec.end());
	//thrust::sort_by_key(d_vec.begin(), d_vec.end(), d_counter.begin());
	thrust::stable_sort_by_key(d_vec.begin(), d_vec.end(), d_counter.begin());

	mTimer.eventRecord("Sort by items");

	/*
	thrust::reduce_by_key(d_vec.begin(), d_vec.end(),
		d_counter.begin(),
		d_vec.begin(),
		d_counter.begin());
	*/

	thrust::device_vector<int>::iterator d_vec_unique_end = 
		thrust::unique_copy(d_vec.begin(), d_vec.end(), d_vec_unique.begin());

	thrust::counting_iterator<int> search_begin(0);

	thrust::lower_bound(d_vec.begin(), d_vec.end(),
		d_vec_unique.begin(), d_vec_unique_end, 
		bucket_begin.begin());

	thrust::upper_bound(d_vec.begin(), d_vec.end(),
		d_vec_unique.begin(), d_vec_unique_end, 
		bucket_end.begin());

	thrust::transform(bucket_end.begin(), bucket_end.end(),
		bucket_begin.begin(), 
		d_counter.begin(), 
		minus<int>());

	thrust::copy(d_vec_unique.begin(), d_vec_unique_end, d_vec.begin());

	int newUniqueSize = d_vec_unique_end - d_vec_unique.begin();
	thrust::fill(d_vec_unique.end() - newUniqueSize, d_vec_unique.end(), 0);

	mTimer.eventRecord("Reduce");


	thrust::sort_by_key(d_counter.begin(), d_counter.end(), d_vec.begin(), thrust::greater<int>());

	mTimer.eventRecord("Sort by counters");

	// transfer data back to host
	//thrust::copy(d_counter.begin(), d_counter.end(), h_conuter.begin());
	//thrust::copy(d_vec.begin(), d_vec.end(), h_vec.begin());
	
	//mTimer.eventRecord("Copy D2H");

	mTimer.printTimes();

	cout << endl;

	for (int i = 0; i < 100; i++)
	{
		if (i == d_vec_unique.size())
			break;

		cout << i << ") " << d_vec[i] << " x " << d_counter[i] << endl;
	}

	system("pause");
}
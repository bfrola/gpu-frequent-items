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

#pragma once

#include "GpuFrequent_kernel.h"

__global__ void kernel_Test1(int* data, int dataSize)
{
	int threadId = threadIdx.x + blockIdx.x * blockDim.x;
	
	if (threadId >= dataSize)
		return;

	data[threadId]  = 1;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// 10/05/11
// No optimizations: Very slow 10x slower than optimized cpu
__global__ void kernel_Test2(int* inData, int* outData, int dataSize)
{
	int threadId = threadIdx.x + blockIdx.x * blockDim.x;
	
	int sum = 0;
	for (int i = 0; i < dataSize; i ++)
	{
		sum += inData[i];
	}

	//__syncthreads();

	outData[threadId]  = sum;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Shared memory dynamically allocated 
extern __shared__ int sharedMemory[];

// cre 20/05/11
// mod 23/05/11
template <bool useSharedMemory>
__global__ void kernel_Test3(
	int* inData, // input data
	int2* outTData, // T data
	int dataSize, 
	int dataPerThread, 
	int outTSize)
{
	int threadId = threadIdx.x + blockIdx.x * blockDim.x;

	
	int* sh_inData;

	// Compilation-time condition
	if (useSharedMemory)
	{
		sh_inData = (int *) sharedMemory;

		// Load shared memory
		for (int i = 0; i < dataPerThread; i ++)
		{
			int offset = threadIdx.x * blockDim.x;
			sh_inData[offset + i] = inData[offset + i];
		}
	
		// Share data
		__syncthreads();
	}
	else
	{
		// Just copy the pointer
		sh_inData = inData;
	}
		
	// All threads load data
	// Only tSize threads execute the computation
	if (threadId > outTSize)
		return;

	int sum = 0;
	for (int i = 0; i < dataSize; i ++)
	{
		sum += sh_inData[i];
		//__syncthreads();
	}

	//printf("%d\n", blockIdx.x);
	outTData[threadId]  = make_int2(sum, 0);
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Uses float instead of int
template <bool useSharedMemory>
__global__ void kernel_Test4(
	float* inData, // input data
	int2* outTData, // T data
	int dataSize, 
	int dataPerThread, 
	int outTSize)
{
	int threadId = threadIdx.x + blockIdx.x * blockDim.x;
	
	float* sh_inData;

	// Compilation-time condition
	// Just copy the pointer
	sh_inData = inData;
		
	// All threads load data
	// Only tSize threads execute the computation
	if (threadId > outTSize)
		return;

	float sum = 0;
	for (int i = 0; i < dataSize; i ++)
	{
		sum += sh_inData[i];
	}

	outTData[threadId]  = make_int2((int) sum, 0);
}


// -------------------------------------------------------------
// -------------------------------------------------------------

// n = dataSize kernel calls
__global__ void kernel_Test5(
	int element, // input data
	int2* tData, // T data
	int tSize)
{
	int threadId = threadIdx.x + blockIdx.x * blockDim.x;
	
	if (threadId > tSize)
		return;

	tData[threadId] = make_int2(element, element);
}

// TODO 9: Kernels version#1

// -------------------------------------------------------------
// -------------------------------------------------------------


// First attempt to write the frequent algo
__global__ void kernel_Test6(
	int* inData, // input data
	int inDataSize,  
	int2* tData, // T data
	int tSize)
{
	// UNUSED
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Copy from kernel 3
__global__ void kernel_Test7(
	int* inData, // input data
	int2* outTData, // T data
	int dataSize,  
	int outTSize, 
	int streamIndex)
{
	int threadId = threadIdx.x + blockIdx.x * blockDim.x;

	
	int* sh_inData;

		// Just copy the pointer
	sh_inData = inData;

		
	// All threads load data
	// Only tSize threads execute the computation
	if (threadId > outTSize)
		return;

	int sum = 0;
	for (int i = 0; i < dataSize; i ++)
	{
		sum += sh_inData[i];
		//__syncthreads();
	}

	//printf("%d\n", blockIdx.x);
	outTData[threadId]  = make_int2(sum, 0);
}

// -------------------------------------------------------------
// -------------------------------------------------------------

#define DEBUG_printfs 0

// Copy from kernel 7
// First GPU version of frequent
__global__ void kernel_Test8(
	int* inData, // input data
	int dataSize,  
	int dataOffset,
	int2* T, // T data, specif for this stream
	int TSize,
	int streamIndex)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;

	// All threads load data
	// Only tSize threads execute the computation
	if (idx >= TSize)
		return;

	__shared__ unsigned int counter; // atomicInc
	
	for (int i = dataOffset; i < dataSize + dataOffset; i ++)
	{
		//if (idx == 0)
		//	printf("[%d] --------------------\n", inData[i]);
		//printf("K%d\t %dx%d\n", idx, T[idx].x, T[idx].y);

		int found = (T[idx].x == inData[i]) ? 1 : 0;
		int foundInT = __syncthreads_or(found);

		//printf("K%d\t[%d] f: %d fInT: %d\n", idx, inData[i], found, foundInT);

		if (found)
		{
			T[idx].y ++;
		}

		if (foundInT)
		{
			continue;
		}
		
		// First half

		int isZero = (T[idx].y == 0) ? 1 : 0;
		int zerosInT = __syncthreads_or(isZero);
		
		
		if (isZero)
		{
			counter = 0;
			// Use atomic inc to compute isFirstZero

			unsigned int zeroRank = 0;

			zeroRank = atomicInc(&counter, blockDim.x); 
			// TOO SLOW! => sync the whole device
			// Solution: use reduction on shared mem
			if (zeroRank == 0) // Choose one of them
			{
				T[idx].x = inData[i];
				T[idx].y = 1;
			}
		}

		//printf("K%d\t[%d] z: %d zInT: %d zRank: %d\n", idx, inData[i], isZero, zerosInT, zeroRank);
		
		if (zerosInT)
		{
			continue;
		}

		T[idx].y --;
	} // for

} // Kernel test #8


// -------------------------------------------------------------
// -------------------------------------------------------------

// First version [T max size = blocksize]
// 2 __syncthreads_or, 1 atomic operation
__device__ void dfreq_2605(
	int idx,
	int dataItem, // input data
	int2& Titem) // T data, specif for this stream
{
	int found = (Titem.x == dataItem) ? 1 : 0;
	int foundInT = __syncthreads_or(found);

	if (found)
	{
		Titem.y ++;
	}

	if (foundInT)
	{
		return;
	}
		
	// First half

	int isZero = (Titem.y == 0) ? 1 : 0;
	int zerosInT = __syncthreads_or(isZero);

	__shared__ unsigned int counter;
		
	if (isZero)
	{
		counter = 0;
		// Use atomic inc to compute isFirstZero

		unsigned int zeroRank = 0;
		zeroRank = atomicInc(&counter, blockDim.x); 
		// VERY SLOW! => sync the whole device
		// Solution1: use reduction on shared mem
		// Solution2: use centralized approach
		
		if (zeroRank == 0) // Choose one of them
		{
			Titem.x = dataItem;
			Titem.y = 1;
		}
	}

	if (zerosInT)
	{
		return;
	}

	Titem.y --;
} // dfreq_2605


// ----------------------------------------------------------
// ----------------------------------------------------------

// Second version [T max size = blocksize]
// 2 syncthread, no atomic operations
// MULTIPLE THREADS WRITE THE SAME SH MEM LOCATION (selectedID)
// THIS DOES NOT WORK IN RELEASE MODE
__device__ void dfreq_3005(
	int idx,
	int dataItem, // input data
	int2& Titem) // T data, specif for this stream
{
	__shared__ bool foundInT;
	foundInT = false;
	
	// Sinse element of T are unique, only one thread at time can do this
	if (Titem.x == dataItem)
	{
		Titem.y ++;
		foundInT = true; 
	}

	// Share foundInT
	__syncthreads(); 
	
	// If one the element has been found
	if (foundInT)
	{
		return;
	}

	// ----------------------------------------
	// First half

	// Selectec the id of one of the items where y == 0
	__shared__ int selectedId;

	// By default is negative
	selectedId = -1;
	__syncthreads(); // Solve the false init problem

	// Set the current thread as candidate
	if (Titem.y == 0)
	{
		// Multiple threads write the same location
		// This does not work in RELEASE (no garantee on the result)
		selectedId = idx;
	}

	// Share selectedId
	__syncthreads();

	// Only one threads has been selected
	if (selectedId == idx) 
	{
		Titem.x = dataItem;
		Titem.y = 1;
	}
	
	// There are no zeros
	if (selectedId < 0) 
	{
		Titem.y --;
	}
} // dfreq_3005

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

// Equal to 3005, concorrent selectedId access resoved
__device__ void dfreq_0906(
	int idx,
	int dataItem, // input data
	int2& Titem) // T data, specif for this stream
{
	__shared__ bool foundInT;
	foundInT = false;
	__syncthreads(); // R2006 correct sharing of foundInT

	// Sinse element of T are unique, only one thread at time can do this
	if (Titem.x == dataItem)
	{
		Titem.y ++;
		foundInT = true; 
	}

	// Share foundInT
	__syncthreads(); 
	
	// If one the element has been found
	if (foundInT)
	{
		return;
	}

	// ----------------------------------------
	// First half

	// Select id of one item with Titem.y == 0
	__shared__ int selectedId; 
	selectedId = 0;
	__syncthreads(); // correct sharing of selectedId 

	// Set the current thread as candidate
	if (Titem.y == 0)
	{
		// 0 is NoValue
		selectedId = idx + 1;
	}

	// Share selectedId
	__syncthreads();

	// Only one threads has been selected
	if (selectedId == idx + 1) 
	{
		Titem.x = dataItem;
		Titem.y = 1;
	}
	
	// There are no zeros
	if (selectedId == 0) 
	{
		Titem.y --;
	}
} // dfreq_0906


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

// Copy from kernel 8
__global__ void kernel_Test9(
	int* inData, // input data
	int dataSize,  
	int dataOffset,
	int2* T, // T data, specif for this stream
	int TSize,
	int streamIndex)
{
	int idx = threadIdx.x; // Only blockIdx.x threads

	// Only tSize threads are active
	if (idx >= TSize)
		return;

	// Get the input (current state of T)
	int2 Titem = T[idx];

	for (int i = dataOffset; i < dataSize + dataOffset; i ++)
	{
		
		dfreq_0906(idx, inData[i], Titem);
		//dfreq_3005(idx, inData[i], Titem); // BUG in release mode
		//dfreq_2605(idx, inData[i], Titem); // Slow
		

#define DEBUG_printFrequentSteps 0
#if DEBUG_printFrequentSteps
		// DEBUG printfs
		if (idx == 0)
			printf("[%d] --------------------\n", inData[i]);
		printf("K%d\t %dx%d\n", idx, Titem.x, Titem.y);
#endif

	} // for

	// Write the output
	T[idx] = Titem;

} // Kernel test #9


__global__ void kernel_Test10(
	int* inData, // input data
	int dataSize,  
	int2* T, // T data, specif for this stream
	int TSize, 
	int TSizePow2)
{
	int idx = threadIdx.x; // More than blockIdx.x threads

	// Only tSize active threads 
	if (idx >= TSize)
		return;

	// Get the input (current state of T)
	int outputOffset = blockIdx.x * TSizePow2;
	int2 Titem = T[outputOffset + idx];

	
	// Offset
	int dataOffset = blockIdx.x * dataSize;
	int dataStart = dataOffset;
	int dataEnd = dataOffset + dataSize;

	for (int i = dataStart; i < dataEnd; i ++)
	{
		dfreq_0906(idx, inData[i], Titem); // same of T9

#define DEBUG_printFrequentSteps 0
#if DEBUG_printFrequentSteps
		// DEBUG printfs
		if (idx == 0)
			printf("\n[%d] --------------------\n", inData[i]);
		if (Titem.y > 0)
			printf("(K%d) %dx%d || ", idx, Titem.x, Titem.y);
#endif
	}

	// Write the output
	T[outputOffset + idx] = Titem;

} // Kernel test #10

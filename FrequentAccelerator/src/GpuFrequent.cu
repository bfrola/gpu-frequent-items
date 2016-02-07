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
/* ******************************************************

Author: Bernardino Frola
Created: 19-jan-2011

 / *******************************************************/
#pragma once

#include "GPUFrequent.h"

// Sintax highlighting and Intellisense
#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

// Help
#include "common/book.h"
#include "GPUFrequent_kernel.cu"

#include <stdlib.h>
#include <stdio.h>

using namespace BF;

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

GPUFrequent::GPUFrequent(float phi, int streamCount) :
m_StreamCount(streamCount)
{
	m_RunningTest = 10;

	m_K = (int) ceil (1.0/phi);

	// Smaller power of 2 > m_K
	m_KPow2 = (int)
			pow(2.0, ceil(log((double) m_K) / log(2.0) ));
	
	// ---------------------------------------------------------
	if (m_RunningTest >= 1 && m_RunningTest <= 5)
	{
		m_T.alloc(m_K);
	}
	// ---------------------------------------------------------
	else if (m_RunningTest == 8 || m_RunningTest == 9)
	{
		m_StreamPool = new cudaStream_t[m_StreamCount];
		m_TPool = new GpuArray<int2>[m_StreamCount];

		for (int i = 0; i < m_StreamCount; i ++)
		{
			HANDLE_ERROR( cudaStreamCreate( &m_StreamPool[i] ) );
			m_TPool[i].alloc(m_K);
		}
	}
	// ---------------------------------------------------------
	else if (m_RunningTest == 10)
	{
		m_T.alloc(m_KPow2 * m_StreamCount);
	}
	// ---------------------------------------------------------
	else
	{
		cout << "Invalid runnning test (" << m_RunningTest << ")" << endl;
		return;
	}


}
 

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

GPUFrequent::~GPUFrequent()
{
	// ---------------------------------------------------------
	if (m_RunningTest >= 1 && m_RunningTest <= 5)
	{
		m_T.free();
	}
	// ---------------------------------------------------------
	else if (m_RunningTest == 8 || m_RunningTest == 9)
	{
		for (int i = 0; i < m_StreamCount; i ++)
		{
			HANDLE_ERROR( cudaStreamDestroy( m_StreamPool[i] ) );	
			m_TPool[i].free();
		}
	}
	// ---------------------------------------------------------
	else if (m_RunningTest == 10)
	{
		m_T.free();
	}
	// ---------------------------------------------------------
	else
	{
		cout << "Invalid runnning test (" << m_RunningTest << ")" << endl;
		return;
	}
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void computeTheadsAndBlocks(
	int totalNumThreads, int blockSize, 
	dim3& threadBlock,
	dim3& blockGrid)
{
	threadBlock = dim3(blockSize, 1);
	blockGrid = dim3((int) ceil((double) totalNumThreads / threadBlock.x), 1);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void GPUFrequent::update_Test1to3( StripData* data, int dataSize, int dataOffset)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 

	int* hostData = &(*intData)[dataOffset];
	GpuArray<int> gaData;
	gaData.alloc(dataSize, false, false, false, hostData);
	gaData.copy(GpuArray<int>::HOST_TO_DEVICE);
	

	// Texture binding (works REALLY bad with integer values)
	//CUDA_SAFE_CALL(cudaBindTexture(0, inDataCache,	
	//	gaData.getDevicePtr(), gaData.getSize() * sizeof(int)));
	
	int dataBlockSize = 4096; // blocks of X bytes

	dim3 threads, blocks;
	computeTheadsAndBlocks(m_KPow2, 512, threads, blocks);
	int dataSizePerThread = dataBlockSize / threads.x;

	int runs = ceil((double) dataSize / dataBlockSize);
	cout << "XLaunch: " << runs << " x " << dataBlockSize << endl;
	for (int i = 0; i < runs; i ++)
	{
		
		// TODO 9: Kernel call
		kernel_Test3<false><<<blocks, threads, dataBlockSize>>>(
			gaData.getDevicePtr(), 
			m_T.getDeviceWritePtr(), 
			dataBlockSize, 
			dataSizePerThread, 
			m_K);
	}

	// Unbind texture
	//CUDA_SAFE_CALL(cudaUnbindTexture(inDataCache));
	m_T.copy(GpuArray<int2>::DEVICE_TO_HOST);

	
	// DO NOT FREE hostData
	gaData.free();
}

void GPUFrequent::update_Test5( StripData* data, int dataSize, int dataOffset)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 

	int* hostData = &(*intData)[dataOffset];
	GpuArray<int> gaData;
	gaData.alloc(dataSize, false, false, false, hostData);
	gaData.copy(GpuArray<int>::HOST_TO_DEVICE);

	dim3 threads, blocks;
	computeTheadsAndBlocks(m_KPow2, 512, threads, blocks);

	cudaStream_t stream;
	HANDLE_ERROR( cudaStreamCreate( &stream ) );

	for (int i = 0; i < 500; i ++)
	{
		kernel_Test5<<<blocks, threads, 0, stream>>>(
			1, m_T.getDeviceWritePtr(), m_K);
	}

	HANDLE_ERROR( cudaStreamDestroy( stream ) );	

	gaData.free();
}

void GPUFrequent::update_Test6( StripData* data, int dataSize, int dataOffset)
{
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	
	int* hostData = &(*intData)[dataOffset];
	GpuArray<int> gaData;
	gaData.alloc(dataSize, false, false, false, hostData);
	gaData.copy(GpuArray<int>::HOST_TO_DEVICE);
	
	int dataBlockSize = dataSize; // blocks of X bytes

	dim3 threads, blocks;
	computeTheadsAndBlocks(m_KPow2, 512, threads, blocks);
	
	kernel_Test6<<<blocks, threads>>>(
			gaData.getDevicePtr(), dataSize, 
			m_T.getDeviceWritePtr(), m_K);

	m_T.copy(GpuArray<int2>::DEVICE_TO_HOST);
	
	// DO NOT FREE hostData
	gaData.free();
}

void GPUFrequent::update_Test7_sub1(int streamCount, GpuArray<int> gaData, int dataSize)
{
	cudaEvent_t timerStart, timerStop;
	cudaEventCreate(&timerStart);
	cudaEventCreate(&timerStop);

	cudaEvent_t timerStart2, timerStop2;
	cudaEventCreate(&timerStart2);
	cudaEventCreate(&timerStop2);

	// ------------------------------	

	cudaEventRecord(timerStart, 0);

	cudaStream_t* stream;
	stream = new cudaStream_t[streamCount];

	for (int i = 0; i < streamCount; i ++)
		HANDLE_ERROR( cudaStreamCreate( &stream[i] ) );

	int dataBlockSize = dataSize / streamCount;

	dim3 threads, blocks;
	computeTheadsAndBlocks(m_KPow2, 1024, threads, blocks);

	// ------------------------------
	
	cudaEventRecord(timerStart2, 0);
	
	for (int i = 0; i < streamCount; i ++)
	{
		kernel_Test7<<<blocks, threads, 0, stream[i]>>>(
			gaData.getDevicePtr(), 
			m_T.getDeviceWritePtr(), 
			dataBlockSize, 
			m_K, i);
	}

	// Wait for the end of any stream
	for (int i = 0; i < streamCount; i ++)
		HANDLE_ERROR( cudaStreamSynchronize(stream[i]) );	

	cudaEventRecord(timerStop2, 0);
	cudaEventSynchronize(timerStop2);

	// ------------------------------

	for (int i = 0; i < streamCount; i ++)
		HANDLE_ERROR( cudaStreamDestroy( stream[i] ) );	

	cudaEventRecord(timerStop, 0);
	cudaEventSynchronize(timerStop);

	float timerElapsed;
	cudaEventElapsedTime(&timerElapsed, timerStart, timerStop);
	float speed = dataSize / timerElapsed;

	float timerElapsed2;
	cudaEventElapsedTime(&timerElapsed2, timerStart2, timerStop2);
	float speed2 = dataSize / timerElapsed2;
	//cout << streamCount << " streams, speed: " << speed <<  " (" << timerElapsed << "ms)" << endl;
	cout << streamCount << " streams, speed: " << speed2 <<  " (" << timerElapsed2 << "ms)" << endl;

}

// 25/05/11 Multiple streams
void GPUFrequent::update_Test7( StripData* data, int dataSize, int dataOffset)
{
	cudaEvent_t tStart, tInitStop, tAllocStop, tMemStop, tComputeStop, tFreeStop;
	cudaEventCreate(&tStart);
	cudaEventCreate(&tAllocStop);
	cudaEventCreate(&tMemStop);
	cudaEventCreate(&tComputeStop);
	cudaEventCreate(&tFreeStop);

	// --------------------------------
	cudaEventRecord(tStart, 0);
	// --------------------------------
	
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int* hostData = &(*intData)[dataOffset];
	GpuArray<int> gaData;
	gaData.alloc(dataSize, false, false, false, hostData);

	// --------------------------------
	cudaEventRecord(tAllocStop, 0);
	cudaEventSynchronize(tAllocStop);
	// --------------------------------

	gaData.copy(GpuArray<int>::HOST_TO_DEVICE);

	// --------------------------------
	cudaEventRecord(tMemStop, 0);
	cudaEventSynchronize(tMemStop);
	// --------------------------------

	// Test pool on m_StreamCount
	for (int i = 0; i < 92; i ++)
		update_Test7_sub1(i + 1, gaData, dataSize);
	
	// --------------------------------
	cudaEventRecord(tComputeStop, 0);
	cudaEventSynchronize(tComputeStop);
	// --------------------------------

	gaData.free();

	// --------------------------------
	cudaEventRecord(tFreeStop, 0);
	cudaEventSynchronize(tFreeStop);
	// --------------------------------

#define DEBUG_timings 0

	float elapsed;
#if DEBUG_timings
	cudaEventElapsedTime(&elapsed, tStart, tAllocStop);
	cout << "Alloc: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tAllocStop, tMemStop);
	cout << "Mem: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tMemStop, tComputeStop);
	cout << "Com: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tComputeStop, tFreeStop);
	cout << "Free: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tStart, tFreeStop);
	cout << "Tot: " << elapsed << "ms" << endl;
#endif
	
	//m_T.copy(GpuArray<int2>::DEVICE_TO_HOST);

	
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// 25/05/11 Multiple streams (copy form test7)
// Centralized stream init/destroy
// Updated 26/05
void GPUFrequent::update_Test8_9( StripData* data, int dataSize, int dataOffset)
{
	cudaEvent_t tStart, tInitStop, tAllocStop, tMemStop, tComputeStop, tFreeStop;
	cudaEventCreate(&tStart);
	cudaEventCreate(&tAllocStop);
	cudaEventCreate(&tMemStop);
	cudaEventCreate(&tComputeStop);
	cudaEventCreate(&tFreeStop);
	
	// --------------------------------
	cudaEventRecord(tStart, 0);
	// --------------------------------
	
	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int* hostData = &(*intData)[dataOffset];
	GpuArray<int> gaData;
	gaData.alloc(dataSize, false, false, false, hostData);
	
	// --------------------------------
	cudaEventRecord(tAllocStop, 0);
	cudaEventSynchronize(tAllocStop);
	// --------------------------------
	
	gaData.copy(GpuArray<int>::HOST_TO_DEVICE);
	
	dim3 threads, blocks;
	computeTheadsAndBlocks(m_KPow2, m_KPow2, threads, blocks);
	int dataBlockSize = dataSize / m_StreamCount;
	//cout << "XLaunch: " << threads.x << "x" << blocks.x << " S" << m_StreamCount << endl;
	 
	// --------------------------------
	cudaEventRecord(tMemStop, 0);
	cudaEventSynchronize(tMemStop);
	// --------------------------------
		
	for (int i = 0; i < m_StreamCount; i ++)
	{
		//kernel_Test8<<<blocks, threads, 0, m_StreamPool[i]>>>(
		kernel_Test9<<<blocks, threads, 0, m_StreamPool[i]>>>(
			gaData.getDevicePtr(), dataBlockSize, // Same input data
			i * dataBlockSize, // Data offset
			m_TPool[i].getDeviceWritePtr(), m_K, // many outputs (one for each stream)
			i // stream index
		);
	}
	
	for (int i = 0; i < m_StreamCount; i ++)
		HANDLE_ERROR( cudaStreamSynchronize(m_StreamPool[i]) );
	
	// --------------------------------
	cudaEventRecord(tComputeStop, 0);
	cudaEventSynchronize(tComputeStop);
	// --------------------------------
	
	gaData.free();

	// --------------------------------
	cudaEventRecord(tFreeStop, 0);
	cudaEventSynchronize(tFreeStop);
	// --------------------------------

#define DEBUG_timings 0

	float elapsed;
	
#if DEBUG_timings
	//cudaEventElapsedTime(&elapsed, tStart, tAllocStop);
	//cout << "Alloc: " << elapsed << "ms" << endl;
	//cudaEventElapsedTime(&elapsed, tAllocStop, tMemStop);
	//cout << "Mem: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tMemStop, tComputeStop);
	cout << "Com: " << elapsed << "ms" << endl;
	//cudaEventElapsedTime(&elapsed, tComputeStop, tFreeStop);
	//cout << "Free: " << elapsed << "ms" << endl;
	//cudaEventElapsedTime(&elapsed, tStart, tFreeStop);
	//cout << "Tot: " << elapsed << "ms" << endl;
#endif
} // Test #8

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void GPUFrequent::update_Test10( StripData* data, int dataSize, int dataOffset)
{
	cudaEvent_t tStart, tMemStop, tComputeStop;
	cudaEventCreate(&tStart);
	cudaEventCreate(&tMemStop);
	cudaEventCreate(&tComputeStop);

	StripDataVector<int>* intData = (StripDataVector<int>*) data; 
	int* hostData = &(*intData)[dataOffset];
	GpuArray<int> gaData;
	gaData.alloc(dataSize, false, false, false, hostData);

	// --------------------------------
	cudaEventRecord(tStart, 0);
	cudaEventSynchronize(tStart);
	// --------------------------------
	
	gaData.copy(GpuArray<int>::HOST_TO_DEVICE);

	// --------------------------------
	cudaEventRecord(tMemStop, 0);
	cudaEventSynchronize(tMemStop);
	// --------------------------------

	dim3 threads, blocks;
	//computeTheadsAndBlocks(m_StreamCount * m_KPow2, m_KPow2, threads, blocks);
	threads.x = m_KPow2;
	blocks.x = m_StreamCount;
	
	int dataBlockSize = dataSize / m_StreamCount;
	
	kernel_Test10<<<blocks, threads>>>(
		gaData.getDevicePtr(), 
		dataBlockSize, 
		m_T.getDeviceWritePtr(), 
		m_K,		// Real size of T
		m_KPow2);	// Output offset on m_T

	// --------------------------------
	cudaEventRecord(tComputeStop, 0);
	cudaEventSynchronize(tComputeStop);
	// --------------------------------
	
#define DEBUG_timings 0
#if DEBUG_timings
	float elapsed;
	cudaEventElapsedTime(&elapsed, tStart, tMemStop);
	cout << "Mem: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tMemStop, tComputeStop);
	cout << "Com: " << elapsed << "ms" << endl;
	cudaEventElapsedTime(&elapsed, tStart, tComputeStop);
	cout << "Tot: " << elapsed << "ms" << endl;
#endif

	gaData.free();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


#define DEBUG_INPUT_DATA 0
#define DEBUG_T_DATA 0

// Non device funcs
void GPUFrequent::update( StripData* data, int dataSize, int dataOffset)
{
	// TODO 9: temp limitation
	if (m_K > 1024)
	{
		cout << "GPUFrequent: K = " << m_K << " too large. Processing stopped." << endl;
		return;
	}

#if DEBUG_INPUT_DATA
	int sum = 0;
	for (int i = 0; i < 10; i ++)// TODO 6: Delete this
	{
		sum += hostData[i];
	}
	cout << "Sum: " << sum << endl;

	float sumF = 0.0f;
	for (int i = 0; i < 10; i ++)// TODO 6: Delete this
	{
		sumF += gaDataF.getHostPtr()[i];
	}
	cout << "Sum: " << sumF << endl; // Test OK
#endif

	// ---------
	//update_Test1to3(data, dataSize, dataOffset);
	//update_Test5(data, dataSize, dataOffset);
	//update_Test6(data, dataSize, dataOffset);
	//update_Test8_9(data, dataSize, dataOffset);

	update_Test10(data, dataSize, dataOffset);

	CUDA_SAFE_CALL( cudaThreadSynchronize() );
	
	// ---------
#if DEBUG_T_DATA
	for (int i = 0; i < 3; i ++)// TODO 6: Delete this
	{
		//sum += hostData[i];
		cout << m_T.getHostPtr()[i].x << ", ";
	}
	cout << "..., " << m_T.getHostPtr()[m_T.getSize() - 1].x << endl;
#endif
	// ---------
}

OutputType GPUFrequent::getOutput(int thresh, int streamIndex)
{
	OutputType res;

	
	// -------------------------------------------------
	if (m_RunningTest == 8 || m_RunningTest == 9)
	{
		
		m_TPool[streamIndex].copy(GpuArray<int2>::DEVICE_TO_HOST);

		// DEBUG
		for (int j = 0; j < m_K; j ++)
		{
			//cout << "(" << m_TPool[streamIndex].getHostPtr()[j].x << ", " << m_TPool[streamIndex].getHostPtr()[j].y << ") ";
			if ( m_TPool[streamIndex].getHostPtr()[j].y > 0)
				res.insert(OutputItemPair(
					m_TPool[streamIndex].getHostPtr()[j].x, 
					m_TPool[streamIndex].getHostPtr()[j].y));
		}
	}
	// -------------------------------------------------
	else if (m_RunningTest == 10) 
	{
		// Require 
		int baseIdx = streamIndex * m_KPow2;
		
		//m_T.copy(GpuArray<int2>::DEVICE_TO_HOST, baseIdx, m_K);
		if (streamIndex == 0)
			m_T.copy(GpuArray<int2>::DEVICE_TO_HOST);
		
		for (int j = baseIdx; j < baseIdx + m_KPow2; j ++)
		{
			if ( m_T.getHostPtr()[j].y > 0)
				res.insert(OutputItemPair(
					m_T.getHostPtr()[j].x, 
					m_T.getHostPtr()[j].y));
		}
	}
	
	return res;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

OutputType* GPUFrequent::getOutput(int thresh)
{
	OutputType* res = new OutputType[m_StreamCount];

	for (int i = 0; i < m_StreamCount; i ++)
	{
		res[i] = getOutput(thresh, i);
	}

	return res;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int GPUFrequent::getMemSize()
{
	// Gpu and cpu memory (2X)
	int singleStreamSize = m_KPow2 * sizeof(int2);
	return singleStreamSize * m_StreamCount;
}

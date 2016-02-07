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

/* ******************************************************/

#pragma once

#include <iostream>
#include "driver_types.h"
#include "GpuArray.h"
#include "StripData.h"
#include "FrequentCommonTypes.h"

using namespace std;

namespace BF
{
	
	class GPUFrequent
	{
	public:
		GPUFrequent(float phi, int streamCount = 1);
		~GPUFrequent();

	public:
		// Public funcs
		//void init (float);
		void update (StripData* data, int dataSize, int dataOffset = 0);
		//void destroy (void);

		// Tests
		void update_Test1to3(StripData* data, int dataSize, int dataOffset = 0);
		void update_Test5(StripData* data, int dataSize, int dataOffset = 0);
		void update_Test6(StripData* data, int dataSize, int dataOffset = 0);

		void update_Test7(StripData* data, int dataSize, int dataOffset = 0);
		void update_Test7_sub1(int streamCount, GpuArray<int> gaData, int dataSize);

		void update_Test8_9(StripData* data, int dataSize, int dataOffset = 0); // Streams

		void update_Test10(StripData* data, int dataSize, int dataOffset = 0); // No streams
		
		// Get the output of a single stream
		OutputType getOutput(int thresh, int streamIndex);
		// Get an array of m_StreamCount outputs
		OutputType* getOutput(int thresh);

		int getStreamCount() { return m_StreamCount; }
		int getK() { return m_K; }

		void setActiveTest(int runningTest) { m_RunningTest = runningTest; }

		int getMemSize();
	private:

		int m_RunningTest;

		int m_K;



		// Test 1-5
		GpuArray<int2> m_T;
		int m_KPow2; // Fitst pow of 2 greater than m_k

		// Test 8-9
		int m_StreamCount;
		cudaStream_t* m_StreamPool;
		GpuArray<int2>* m_TPool;

	};
}
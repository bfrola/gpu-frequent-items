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
Created: 27-apr-2011

/ *******************************************************/

#pragma once

#include "FrequentStripProcessor.h"
#include "SSStripProcessor.h"
#include "pthread.h"
#include "FISPFactory.h"

#include "GpuSFStripProcessor.h" // Output gemeratpr

using namespace BF;

namespace BF
{
	
	class ParallelProcessArguments 
	{
	public:
		float m_Phi;
		StripData* m_Data;

		int m_StripSize;
		int m_StripOffset;

		OutputType m_FrequentOutput;
		FIStripProcessor* m_SpFreq;

	};

	// Generic parallel implementation of FIStripProcessor
	class ParFIStripProcessor : public FIStripProcessor
	{
		// Constructor-destructor
	public:
		ParFIStripProcessor (float phi, int poolSize, FISPFactory* fispFactory);
		~ParFIStripProcessor();

		// Fields
	protected:
		int m_PoolSize;

		pthread_t* m_ThreadPool;
		ParallelProcessArguments** m_ThreadArguments;

		// Methods
	public:
		void initialize();

		// Override the method of FrequentStripProcessor
		void process(StripData* data);
		void destroy();
		
		// "Forward" abstract method
		virtual OutputType getOutput(int thresh) = 0;
		int getSize();
		int getK();
	};

	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------

	class OutuputComposer : public GpuSFStripProcessor
	{

	public:
		OutuputComposer(float phi, int stripSize);
	
		// -------------------------------------------------------------------------

		void fillT(ParallelProcessArguments* threadArgs[], int poolSize, int thresh);
	
		// -------------------------------------------------------------------------

		void processSubStrip(StripDataVector<int>* data, int scOffset, int scSize);
	
		// -------------------------------------------------------------------------

	private:
		int m_MinCounter;
	};
	
	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------

	// Specialization of ParFIStripProcessor
	class ParFreqStripProcessor : public ParFIStripProcessor
	{
	public:
		ParFreqStripProcessor (float phi, int poolSize);

	public:
		/// Reduce two OutputType into a single OutputType item 
		static OutputType reductionOperator(OutputType res1, OutputType res2, int k);		

		// Override methods of FrequentStripProcessor
		OutputType getOutput(int thresh);

	private:
		int m_MinCounter;
	};

	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------

	// Specialization of ParFIStripProcessor
	class ParSSStripProcessor : public ParFIStripProcessor
	{
	public:
		ParSSStripProcessor (float phi, int poolSize);
		~ParSSStripProcessor();

	public:
		/// Reduce two OutputType into a single OutputType item 
		static OutputType reductionOperator(OutputType res1, OutputType res2, int k);		

		// Override methods of FrequentStripProcessor
		OutputType getOutput(int thresh);
		
	private:
		OutuputComposer* m_OutputComposer;
	};
	
	
}
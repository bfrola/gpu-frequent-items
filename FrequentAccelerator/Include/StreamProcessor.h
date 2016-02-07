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
/* ******************************************************

Author: Bernardino Frola
Created: 30-jan-2011

/ *******************************************************/

#include <stdlib.h>

// Interproject dependecies
#include "CommonConstants.h"
#include "StripProcessor.h"

// Auxiliary libraries
#include "pthread.h"
#include "windows.h" 
// We use windows.h for:
//  - Sleep (simulation purposes)
//  - Threads (asynchronous calls)

using namespace std;

const int THREAD_POOL_SIZE = 2;

using namespace BF;

namespace BF
{
	// Thread/Handler arguments
	class AsyncStripProcessorHandlerArguments 
	{
	public:
		StripProcessor* stripProcessor;
		int handlerIndex;
	};

	// TODO: 6 Add handler class, containing HandleStripProcessingArguments reference

	template <class DataType>
	class AsyncStripProcessorHandler
	{
		typedef DataType* StripType;
		enum CurrentStateType
		{
			Idle,
			Running
		};

	protected: // Fields
		CurrentStateType m_CurrentState;
		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------

	public: // public methods
		BF::AsyncStripProcessorHandler::CurrentStateType CurrentState() const { return m_CurrentState; }
		void CurrentState(BF::AsyncStripProcessorHandler::CurrentStateType val) { m_CurrentState = val; }

		void launchAsyncProcessing(); // TODO: 6 create this

	};

	
	// StreamProcessor class
	template <class DataType>
	class StreamProcessor
	{
		// Strip type definition
		typedef DataType* StripType;

	protected: // Fields
		// int m_StripSize; // Stored in StripProcessor
		int m_CurrentIndex;
		

		int m_ReadStrip;
		int m_WriteStrip;

		// Strip analyzer
		StripProcessor* m_StripProcessor;
	
		// TODO: 7 Implement buffer/Use multiple strip processors
		StripType m_Strip[THREAD_POOL_SIZE];

		// Processing Handlers (threads)
		pthread_t m_ProcessingHandler[THREAD_POOL_SIZE];
		int m_CurrentHandlerIndex;
		

		
		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------

	public: // public methods
		StreamProcessor (StripProcessor* stripProcessor);
		~StreamProcessor();

		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------
		// Getters-Setters

		StripType getReadStrip () { return m_Strip [m_ReadStrip]; }
		StripType getWriteStrip () { return m_Strip [m_WriteStrip]; }

		int getCurrentIndex() const { return m_CurrentIndex; }
		void setCurrentIndex(int val) { m_CurrentIndex = val; }

		int getCurrentHandlerIndex() const { return m_CurrentHandlerIndex; }
		void setCurrentHandlerIndex(int val) { m_CurrentHandlerIndex = val; }

		int getStripSize() { return 2000; }// TODO 6: TEMP


		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------

		/**
		* 
		* 
		*/
		void addElement (DataType element);
	
		/**
		* 
		* 
		*/
		void handleStripProcessing();

		/**
		* 
		* 
		*/
		void printCurrentIndexGraph();
	
	}; // StreamProcessor class

} // BF namespace



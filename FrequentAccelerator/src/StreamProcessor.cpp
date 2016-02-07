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

#include "StreamProcessor.h"

using namespace BF;

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// Handler function --> pthread_create
template<class DataType>
void* handleStripProcessingAsync(void* threadArgs)
{
	// Gather arguments
	AsyncStripProcessorHandlerArguments* handlerArgs = 
		(AsyncStripProcessorHandlerArguments*) threadArgs;
		
	
		
	// Execute the strip processing asynchronously
	handlerArgs->stripProcessor->process(NULL);

	cout << endl << " ******** Processing complete" << endl;

	pthread_exit((void*) threadArgs);
	return 0;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class DataType>
StreamProcessor<DataType>::StreamProcessor (StripProcessor* stripProcessor) : 
	m_StripProcessor (stripProcessor), 
	m_CurrentIndex (0), 
	m_ReadStrip (0), 
	m_WriteStrip (0),
	m_CurrentHandlerIndex (0)
{
	assert (m_StripProcessor != NULL);
	
	// TODO: 6 generalize
	m_Strip[0] = (DataType*) malloc (1000 * sizeof(DataType)); // TODO 6: TEMP
	//m_Strip[1] = (DataType*) malloc (m_StripProcessor->getStripSize() * sizeof(DataType)); 

	m_StripProcessor->initialize();

			
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class DataType>
StreamProcessor<DataType>::~StreamProcessor()
{
	cout << "Waiting for all threads to complete...";
	// Wait for all threads to complete
	for (int i = 0; i < THREAD_POOL_SIZE; i ++) {
		pthread_join(m_ProcessingHandler[i], NULL);
	}
	cout << "Ok" << endl;

	free (m_Strip[0]);
	m_StripProcessor->destroy();
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// TODO: 2 Add support to composite element
template <class DataType>
void StreamProcessor<DataType>::addElement(DataType element)

{
	getWriteStrip()[m_CurrentIndex] = element;

	//printCurrentIndexGraph(); // Debug purposes
			
	// TODO *OK: 1 Delete this
	//Sleep (ELEMENT_TRANSMISSION_DELAY); // Simulation/Debug purposes 
			
	m_CurrentIndex ++;
	if (m_CurrentIndex == 2000) // TODO 6: TEMP
	{		
		handleStripProcessing();

		// TODO: 6 Switch buffers (??)
		m_CurrentIndex = 0;
	}

} // addElement

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class DataType>
void StreamProcessor<DataType>::handleStripProcessing()
{
	int currentHandler = m_CurrentHandlerIndex;

	// Setup thread arguments
	AsyncStripProcessorHandlerArguments* handlerArgs =
		new AsyncStripProcessorHandlerArguments();

	handlerArgs->handlerIndex = m_CurrentHandlerIndex;
	handlerArgs->stripProcessor = m_StripProcessor;

	// Wait for this thread to (mi so rincojonito)
	//if (m_ProcessingHandler[currentHandler].x == 0)
	//	pthread_join(m_ProcessingHandler[currentHandler], NULL);

	cout << "Launched: " << currentHandler << endl;
	// Create and launch the thread
	pthread_create(&m_ProcessingHandler[currentHandler], 
		NULL, handleStripProcessingAsync<DataType>, handlerArgs);

	// Increment and wrap the handler index
	m_CurrentHandlerIndex ++;
	if (m_CurrentHandlerIndex == THREAD_POOL_SIZE)
	{
		m_CurrentHandlerIndex = 0;
	}

}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

template <class DataType>
void StreamProcessor<DataType>::printCurrentIndexGraph()
{
	const int printInverseFrequency = 100; // Min: 1; 

	if (m_CurrentIndex % printInverseFrequency != 0)
		return;

	// Graphical representation of the input buffer
	for (int i = 0; i < m_CurrentIndex / printInverseFrequency; i ++)
		// for (int j = 0; j < printInverseFrequency; j ++)
			cout << ".";

	cout << endl;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// Available implementations
// Avoid link errors
template class StreamProcessor<int>;
template class StreamProcessor<float>;
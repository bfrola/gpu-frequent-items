/* *****************************************************

Author: Bernardino Frola
Created: 27-apr-2011

/ *******************************************************/

#pragma once

#include "StripProcessor.h"

using namespace BF;

namespace BF
{
	/*
	 *	This implementation of strip processor does nothing
	 */
	template <class DataType>
	class DummyStripProcessor : public StripProcessor<DataType>
	{
		// Constructor-destructor
	public:
		DummyStripProcessor (int stripSize);
		~DummyStripProcessor();

		//////////////////////////////////////////////////////////////////////////
		// Fields
	protected:

		//////////////////////////////////////////////////////////////////////////
		// Methods
	public:
		void initialize();
		void process();
		void destroy();
	};
}
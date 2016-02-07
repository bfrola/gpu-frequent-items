/* *****************************************************

Author: Bernardino Frola
Created: 27-apr-2011

/ *******************************************************/

#pragma once

#include "StripProcessor.h"
#include "frequent.h"
#include "lossycount.h"
#include "FrequentCommonTypes.h"

using namespace BF;

namespace BF
{
	class FrequentStripProcessor : public StripProcessor
	{
		// Constructor-destructor
	public:
		FrequentStripProcessor (float phi);
		~FrequentStripProcessor();

		// Fields
	protected:
		//freq_type* m_Freq;//TEMP
		LCU_type* m_Freq;
		float m_Phi;
	

		// Methods
	public:
		void initialize();
		virtual void process(StripData* data);
		void destroy();

	public:
		//virtual OutputType getOutput(int thresh) { return Freq_Output(m_Freq, thresh); }//TEMP
		virtual OutputType getOutput(int thresh) { return LCU_Output(m_Freq, thresh); }
		//virtual int getSize() { return Freq_Size(m_Freq); }//TEMP
		virtual int getSize() { return LCU_Size(m_Freq); }

		//freq_type* Freq() const { return m_Freq; }//TEMP
		LCU_type* Freq() const { return m_Freq; }
		//void Freq(freq_type* val) { m_Freq = val; }//TEMP
		void Freq(LCU_type* val) { m_Freq = val; }
	};
}
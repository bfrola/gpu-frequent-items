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
/* ******************************************************

Author: Bernardino Frola
Created: 15-feb-2011

/ *******************************************************/

#pragma once

#include <iostream>
#include <assert.h>

#include "CommonConstants.h"

//#include "windows.h" // Sleep needed (simulation purposes)


#include "StripData.h"
// Temp CPU-Frequent dependet


// TODO: 6 extend this class->setup GPU strip processor

using namespace BF;
using namespace std;

namespace BF
{
	class StripProcessor
	{
	protected: //Fields

		int m_StripSize; // Specific size of the strip (different from the real size of data)
		int m_StripOffset; // Offeset element used as starting point for processing

	public: // Get/Set (extension can override this methods)
		virtual int StripSize() { return m_StripSize; }
		virtual void StripSize(int stripSize) { m_StripSize = stripSize; }
		virtual int StripOffset() { return m_StripOffset; }
		virtual void StripOffset(int stripOffset) { m_StripOffset = stripOffset; }
		
		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------

	public: // Methods
		StripProcessor ();
		~StripProcessor();
		
		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------
		// Getters-Setters

		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------

		/**
		 * 
		 */
		virtual void initialize() = 0;
		
		/**
		 * 
		 */
		virtual void process(StripData* data) = 0;
		
		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------

		virtual void destroy() = 0;
	}; // StripProcessor class

} // BF namespace
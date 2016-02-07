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
/* *****************************************************

Author: Bernardino Frola
Created: 09-sept-2011

// Abstract factory design pattern 
// Create new instances of specialiations of FIStripProcessor
// Current implementations: 
//  - FrequentSPFactory
//  - SSSPFactory

/ *******************************************************/

#include "FIStripProcessor.h"
#include "FrequentStripProcessor.h"
#include "SSStripProcessor.h"

using namespace BF;

namespace BF
{
	
	// --------------------------------------------------------
	// --------------------------------------------------------

	class FISPFactory
	{
	public:
		virtual FIStripProcessor* getNewSPInstance(float phi) = 0;
	};
	
	// --------------------------------------------------------
	// --------------------------------------------------------

	class FrequentSPFactory : public FISPFactory
	{
	public:
		FIStripProcessor* getNewSPInstance(float phi) { return new FrequentStripProcessor(phi); }
	};

	// --------------------------------------------------------
	// --------------------------------------------------------

	class SSSPFactory : public FISPFactory
	{
	public:
		FIStripProcessor* getNewSPInstance(float phi) { return new SSStripProcessor(phi); }
	};
}

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
Created: 15-feb-2011

/ *******************************************************/


#pragma once

namespace BF
{

	// Debug constant values
	const int ELEMENT_TRANSMISSION_DELAY = 0; // Millisecs

	// ELEMENT_PROCESSING_DELAY = ELEMENT_TRANSMISSION_DELAY 
	//    ==> Processing time = Transmission time 

	// const int ELEMENT_PROCESSING_DELAY = ELEMENT_TRANSMISSION_DELAY * 10; // Millisecs
	const int ELEMENT_PROCESSING_DELAY = ELEMENT_TRANSMISSION_DELAY; // Millisecs


	// -------------------------------------------------

	// Print/Debug
	const int PRINT_INV_FREQUENCY = 10; // Min: 1; 

} // BF namespace
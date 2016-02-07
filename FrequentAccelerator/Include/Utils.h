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
/* *****************************************************

Author: Bernardino Frola
Created: 26-july-2011

/ *******************************************************/

#include <fstream>
#include <iomanip>
#include <sstream>

#pragma once

using namespace std;

namespace BF
{
	// Utility: Handles multiples cudaEvent_t
	template <bool enabled> // Compile time condition
	class MultiTimer 
	{
		vector<pair<cudaEvent_t, string>> m_Timer;
	public:

		// Setup and record a new 
		void eventRecord(string label) 
		{
			if (!enabled)
				return;

			cudaEvent_t timer;
			cudaEventCreate(&timer);
			m_Timer.push_back(pair<cudaEvent_t, string>(timer, label));
			cudaEventRecord(timer, 0);
			cudaEventSynchronize(timer);
		}

		// Print times and percentages
		void printTimes()
		{
			if (!enabled)
				return;

			float tolalValue;
			cudaEvent_t tStart = m_Timer[0].first;
		
			// Compute total
			for(int i = 1; i < m_Timer.size(); i ++)
			{
				float tElapsed;
				cudaEventElapsedTime(&tElapsed, m_Timer[i - 1].first, m_Timer[i].first);
				tolalValue += tElapsed;
			}
		
			// Print values
			for(int i = 1; i < m_Timer.size(); i ++)
			{
				float tElapsed;
				cudaEventElapsedTime(&tElapsed, m_Timer[i - 1].first, m_Timer[i].first);
				cout << 
					fixed << setprecision(3) << 
					m_Timer[i].second << ": " << tElapsed << "ms" <<
					fixed << setprecision(0) << 
					" (" << (100 * tElapsed / tolalValue) << "%)" <<
					endl;
			}
		}
	};

	// -------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------

	// Read items from a dataset on file
	template <class itemType>
	void readDataset(std::string datasetPath, std::vector<itemType>& data)
	{
		itemType inItem;

		std::ifstream inFile(datasetPath.c_str());

		if (!inFile.is_open())
		{
			cout << "Unable to open '" << datasetPath.c_str() << "'" << endl;
			return;
		}

		cout << "File successfully opened '" << datasetPath.c_str() << "'" << endl;
		

		// Add to the vector
		while (inFile >> inItem) 
		{

			if (inItem > 0) // Use zero as missing value
				data.push_back(inItem);
			
		}

		inFile.close();
	}
}

using namespace BF;
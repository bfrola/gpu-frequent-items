/* ******************************************************
/* ******************************************************

Frequent Items on GPU.
Sourceforge project: https://sourceforge.net/projects/figpu/

	This work is licensed under the Creative Commons
	Attribution-NonCommercial License. To view a copy of this license,
	visit http://creativecommons.org/licenses/by-nc/1.0/ or send a letter
	to Creative Commons, 559 Nathan Abbott Way, Stanford, California
	94305, USA.

	Algorithms and sources have been introduced and analysed in the following two scientific papers: 
	*** U. Erra and B. Frola, In Proceedings of The International Conference on Computational Science (ICCS), Omaha, Nebraska, USA, June 4-6, 2012.
		http://www.sciencedirect.com/science/article/pii/S1877050912001317
	*** G. Cormode, M. Hadjieleftheriou, Finding the frequent items in streams of data, Commun. ACM 52 (2009) 97–105
		http://dl.acm.org/citation.cfm?id=1454225
	Please, cite them in your publication/work/project. Thank you.

	Content of the "Frequent Items" Visual Studio Project was downloaded from:
	http://www2.research.att.com/~marioh/frequent-items/index.html

   ******************************************************/
/* ******************************************************

Original author: G. Cormode 2002, 2003,2005

This version
Author: Bernardino Frola
Created: 03-may-2011

/ *******************************************************/

#pragma once

#include "HelperFrequent.h"

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void CheckOutput(std::map<uint32_t, uint32_t>& res, uint32_t thresh, size_t hh, Stats& S, const std::vector<uint32_t>& exact)
{
	if (res.empty())
	{
		S.F.insert(0.0);
		S.F2.insert(0.0);
		S.P.insert(100.0);
		S.dP += 100.0;

		if (hh == 0)
		{
			S.R.insert(100.0);
			S.dR += 100.0;
		}
		else
			S.R.insert(0.0);

		return;
	}

	size_t correct = 0;
	size_t claimed = res.size();
	size_t falsepositives = 0;
	double e = 0.0, e2 = 0.0;

	std::map<uint32_t, uint32_t>::iterator it;
	for (it = res.begin(); it != res.end(); ++it)
	{
		if (exact[it->first] >= thresh)
		{
			++correct;
			uint32_t ex = exact[it->first];
			double diff = (ex > it->second) ? ex - it->second : it->second - ex;
			e += diff / ex;
		}
		else
		{
			++falsepositives;
			uint32_t ex = exact[it->first];
			double diff = (ex > it->second) ? ex - it->second : it->second - ex;
			e2 += diff / ex;
		}
	}

	if (correct != 0)
	{
		e /= correct;
		S.F.insert(e);
		S.dF += e;
	}
	else
		S.F.insert(0.0);

	if (falsepositives != 0)
	{
		e2 /= falsepositives;
		S.F2.insert(e2);
		S.dF2 += e2;
	}
	else
		S.F2.insert(0.0);

	double r = 100.0;
	if (hh != 0) r = 100.0 *((double) correct) / ((double) hh);

	double p = 100.0 *((double) correct) / ((double) claimed);

	S.R.insert(r);
	S.dR += r;
	S.P.insert(p);
	S.dP += p;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

size_t RunExact(uint32_t thresh, std::vector<uint32_t>& exact)
{
	size_t hh = 0;

	for (size_t i = 0; i < exact.size(); ++i)
		if (exact[i] >= thresh) ++hh;

	return hh;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void Exact_Update(std::map<uint32_t, uint32_t>& res, int item)
{
	res[item] = res[item] + 1;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

// Just extract from exactRes values with second greater than thresh
std::map<uint32_t, uint32_t> Exact_Output(std::map<uint32_t, uint32_t>& exactRes, int thresh)
{
	std::map<uint32_t, uint32_t> res;

	std::map<uint32_t, uint32_t>::iterator it;
	for (it = exactRes.begin(); it != exactRes.end(); ++it)
	{
		if (it->second >= thresh)
		{
			res.insert(std::pair<uint32_t, uint32_t>(it->first, it->second));
		}
	}

	return res;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

// Do resA and resB contain the same elements?
// Used for quality test
bool areEqualRes(
	std::map<uint32_t, uint32_t> resA,
	std::map<uint32_t, uint32_t> resB)
{
	if (resA.size()
		!= resB.size())
		return false;

	std::map<uint32_t, uint32_t>::iterator it;
	
	// 1. Does resB contain all the element in resA?
	for (it = resA.begin(); it != resA.end(); ++it)
	{
		//if (it->second != resB[it->first])
		if (resB[it->first] == 0)
			return false;
	} 

	// 2. Does resA contain all the element in resB?
	for (it = resB.begin(); it != resB.end(); ++it)
	{
		//if (it->second != resA[it->first])
		if (resA[it->first] == 0)
			return false;
	} 

	// If 1 and 2 are true, the function returns TRUE
	return true;
}

// --------------------------------------------------------------
// --------------------------------------------------------------

bool areOlnyExactEqualRes(
	std::map<uint32_t, uint32_t> resA,
	std::map<uint32_t, uint32_t> resB,
	std::vector<uint32_t> exact,
	int thresh)
{
	std::map<uint32_t, uint32_t>::iterator it;

	// 1. Does resB contain all the element in resA?
	for (it = resA.begin(); it != resA.end(); ++it)
	{
		//cout << "DIFF [" << (exact[it->first] >= thresh) << "]: " << it->first << " " << 
		//	it->second << " "  << resB[it->first] << endl;

		if (exact[it->first] >= thresh && 
			//it->second != resB[it->first])
			resB[it->first] == 0)
			return false;
	} 

	// 2. Does resA contain all the element in resB?
	for (it = resB.begin(); it != resB.end(); ++it)
	{
		if (exact[it->first] >= thresh && 
			//it->second != resA[it->first])
			resA[it->first] == 0)
			return false;
	} 
	
	// If 1 and 2 are true, the function returns TRUE
	return true;
}

// --------------------------------------------------------------
// --------------------------------------------------------------

// TODO 9: complete this
// The same of areOlnyExactEqualRes, returns a matching quality percentage [0...1]
float falseNegativeRate(
	std::map<uint32_t, uint32_t> resA,
	std::map<uint32_t, uint32_t> resB,
	std::vector<uint32_t> exact,
	int thresh)
{
	std::map<uint32_t, uint32_t>::iterator it;

	if (resA.size() == 0)
	{
		cout << "resA.size() is zero!" << endl;
		return 0;
	}

	int falseNegatives = 0;
	// 1. Does resB contain all the element in resA?
	for (it = resA.begin(); it != resA.end(); ++it)
	{
		if (exact[it->first] < thresh)
			continue;

		if (resB[it->first] == 0)
		{
			falseNegatives ++;
			cout << it->first << ", F: " << resA[it->first] << ", Ex: " << exact[it->first] << endl;
			continue;
		}

		//cout << "[" << it->second << ", " << resB[it->first] << "] ";
	} 

	cout << endl << falseNegatives << " on " << resA.size() << " items " << endl;
	
	return falseNegatives * 100 / resA.size();
}

float counterErrroRate(
	std::map<uint32_t, uint32_t> resA,
	std::map<uint32_t, uint32_t> resB,
	std::vector<uint32_t> exact,
	int thresh)
{
	int counterErrors = 0;
	int exacts = 0;

	for (OutputType::iterator it = resA.begin(); it != resA.end(); ++it)
	{
		if (exact[it->first] < thresh)
			continue;
		
		exacts ++;

		if (it->second != resB[it->first])
			counterErrors ++;
	}

	return counterErrors * 100 / exacts;
}



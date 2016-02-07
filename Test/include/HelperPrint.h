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
#include "HelperQuality.h"

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void PrintStartCheckPoint( std::string str )
{
#ifdef ENABLE_PrintCheckPoint
	std::cout << str << "...";
#endif
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void PrintCheckPoint( std::string str )
{
#ifdef ENABLE_PrintCheckPoint
	std::cout << "OK" << std::endl;
	std::cout << str << "...";
#endif
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void PrintEndCheckPoint( void )
{
#ifdef ENABLE_PrintCheckPoint
	std::cout << "OK" << std::endl;
#endif
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void PrintOutput(
	string title, size_t size, const Stats& S, size_t u32NumberOfPackets, 
	ofstream* outFileStream = NULL)
{
	double p5th = -1.0, p95th = -1.0; 
	double r5th = -1.0, r95th = -1.0, r50th = -1.0;
	double f5th = -1.0, f95th = -1.0;
	double f25th = -1.0, f295th = -1.0;
	size_t i5, i95, i50;
	std::multiset<double>::const_iterator it;

	if (! S.P.empty())
	{
		it = S.P.begin();
		i5 = static_cast<int>(S.P.size() * 0.05);
		for (size_t i = 0; i < i5; ++i) ++it;
		p5th = *it;
		i95 = static_cast<int>(S.P.size() * 0.95);
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		p95th = *it;
	}
	
	if (! S.R.empty())
	{
		it = S.R.begin();
		i5 = static_cast<int>(S.R.size() * 0.05);
		for (size_t i = 0; i < i5; ++i) ++it;
		r5th = *it;

		i50 = static_cast<int>(S.R.size() * 0.50);
		for (size_t i = 0; i < (i50 - i5); ++i) ++it;
		r50th = *it;

		i95 = static_cast<int>(S.R.size() * 0.95);
		for (size_t i = 0; i < (i95 - i50); ++i) ++it;
		r95th = *it;
	}

	if (! S.F.empty())
	{
		it = S.F.begin();
		i5 = static_cast<int>(S.F.size() * 0.05);
		for (size_t i = 0; i < i5; ++i) ++it;
		f5th = *it;
		i95 = static_cast<int>(S.F.size() * 0.95);
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		f95th = *it;
	}

	if (! S.F2.empty())
	{
		it = S.F2.begin();
		i5 = static_cast<int>(S.F2.size() * 0.05);
		for (size_t i = 0; i < i5; ++i) ++it;
		f25th = *it;
		i95 = static_cast<int>(S.F2.size() * 0.95);
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		f295th = *it;
	}

	int packatesPerMs = (S.dU > 0) ? u32NumberOfPackets / S.dU : 500000;

	cout << title << "\t" <<
		fixed << setprecision(1) <<
		packatesPerMs/1000 << "K\t" <<
		S.dU << "\t" <<
		S.dQ << "\t" <<
		(size / 1024) << "K\t" <<
		S.dR / S.R.size() << "\t" <<
		r5th << "\t" <<
		//r50th << "\t" <<
		//r95th << "\t" <<
		S.dP / S.P.size() << "\t" <<
		//p5th << "\t" <<
		//p95th << "\t" <<
		fixed << setprecision(4) <<
		S.dF / S.F.size() << "\t" <<
		//f5th << "\t" <<
		//f95th << "\t" <<
		//S.dF2 / S.F2.size() << "\t" <<
		//f25th << "\t" <<
		//f295th << "\t" <<
		endl;

	if (outFileStream == NULL)
		return;

	*outFileStream << title << "\t" <<
		packatesPerMs << "\t" <<
		S.dU << "\t" <<
		S.dQ << "\t" <<
		size << "\t" <<
		S.dR / S.R.size() << "\t" <<
		r5th << "\t" <<
		r95th << "\t" <<
		S.dP / S.P.size() << "\t" <<
		p5th << "\t" <<
		p95th << "\t" <<
		S.dF / S.F.size() << "\t" <<
		f5th << "\t" <<
		f95th << "\t" <<
		//S.dF2 / S.F2.size() << "\t" <<
		//f25th << "\t" <<
		//f295th << "\t" <<
		endl;	

	
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void PrintOutputHeader(ofstream* outFileStream = NULL)
{
	stringstream outStream;
	cout << endl << 
		"Method" << "\t" <<
		"Upd/ms" << "\t" <<
		"Upd(ms)" << "\t" <<
		"Out(ms)" << "\t" <<
		"Space" << "\t" 
		"Rme" << "\t" <<
		"R5" << "\t" <<
		//"R50" << "\t" <<
		//"R95" << "\t" <<
		"P" << "\t" <<
		//"P5" << "\t" <<
		//"P95" << "\t" <<
		"ARE" << "\t" <<
		//"ARE5th" << "\t" <<
		//"ARE95th" << "\t" <<
		//"FP" << "\t" <<
		//"FP5th" << "\t" <<
		//"FP95th" << "\t" <<
		endl;
		
	if (outFileStream == NULL)
		return;

	// Write the same to file
	//ofstream fileStream;
	//fileStream.open ("example.dat", ios::app);
	*outFileStream <<  
		"Updates_per_ms" << "\t" <<
		"Update_time" << "\t" <<
		"Output_time" << "\t" <<
		"Bytes" << "\t" 
		"Recall" << "\t" <<
		"Recall 5th" << "\t" <<
		"Recall 95th" << "\t" <<
		"Precision" << "\t" <<
		"Precision 5th" << "\t" <<
		"Precision 95th" << "\t" <<
		"ARE" << "\t" <<
		"ARE 5th" << "\t" <<
		"ARE 95th" << "\t" << 
		"---" << endl;
	//fileStream.close();*/
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void PrintRes(string title, std::map<uint32_t, uint32_t>& res)
{
	cout << "------------------------" << endl << title << "(" << res.size() << "): ";
	std::map<uint32_t, uint32_t>::iterator it;
	for (it = res.begin(); it != res.end(); ++it)
	{
		cout << 
			it->first << " [x" << 
			it->second << "] ";
	}
	cout << endl;
}

bool comp_hungarian_strings(pair<uint32_t, uint32_t> x, pair<uint32_t, uint32_t>  y)
{
	return x.second > y.second;
}

void PrintRes2File(string title, std::map<uint32_t, uint32_t>& res, string fileName)
{
	ofstream fileStream;
	fileStream.open (fileName.c_str(), ios::app);
	
	fileStream << "------------------ " << title << "(" << res.size() << "): " << endl;

	std::vector<pair<uint32_t, uint32_t>> sortedRes(res.begin(), res.end());
	sort(sortedRes.begin(), sortedRes.end(), comp_hungarian_strings);
	
	std::vector<pair<uint32_t, uint32_t>>::iterator it = sortedRes.begin();
	for (int i = 0; i < res.size(); i ++)
	{
		fileStream << i << ") " << it->first  << ", " << it->second << endl;
		it ++;
	}
	fileStream.close();
}

void printComparison(double oldVal, double newVal)
{
	if (newVal == oldVal)
	{
		std::cout << "Same perfs" << std::endl;
		return;
	}

	/*if (newVal == 0)
	{
		std::cout << "+INF faster" << std::endl;
		return;
	}

	if (oldVal == 0)
	{
		std::cout << "+INF slower" << std::endl;
		return;
	}*/

	if (oldVal > newVal)
	{
		std::cout << oldVal / newVal << "x faster" << std::endl;
		return;
	}

	std::cout << newVal / oldVal << "x SLOWER" << std::endl;
}

// Improvements of a new approach comparet to an ond one
void PrintImprov(string title, const Stats& SOld, const Stats& SNew)
{
	//double diff = S1.dU - S2.dU;

	std::cout << title << std::endl << "\tUpdate: ";
	printComparison(SOld.dU, SNew.dU);
	//diff = S1.dQ - S2.dQ;
	std::cout << "\tOutput: ";
	printComparison(SOld.dQ, SNew.dQ);

}

void CondPrintRes(string title, std::map<uint32_t, uint32_t>& res)
{
#if ENABLE_PrintRes // Moved to Test[...]
	PrintRes(title, res);
#endif
}


// Are resA and resB equal? similar?
// resA: base
// resB: new result
void PrintResComparison(
	string title,
	std::map<uint32_t, uint32_t> resBase,
	std::map<uint32_t, uint32_t> resNew,
	std::vector<uint32_t> exact,
	int thresh)
{
	cout << "Comparing results base:current " << resBase.size() << ":" << resNew.size() << endl;
	if (resBase.size() == 0)
	{
		cout << "resBase is empty!" << endl;
		return;
	}

	if (resNew.size() == 0)
	{
		cout << "resNew is empty!" << endl;
		return;
	}

	cout << "\t" << title << " fPRate: " << 
		falseNegativeRate(resBase, resNew, exact, thresh) << "%" << endl;
	cout << "\t" << title << " fNRate: " << 
		falseNegativeRate(resNew, resBase, exact, thresh) << "%" << endl;
	//cout << title << " cE:     " << 
	//	counterErrroRate(resBase, resNew, exact, thresh) << "%" << endl;
	//cout << title << " cE:     " << 
	//	counterErrroRate(resNew, resBase, exact, thresh) << "%" << endl;
}

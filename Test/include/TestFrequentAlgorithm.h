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
	Please, do cite them in your publication/work/project. Thank you.

	Content of the "Frequent Items" Visual Studio Project was downloaded from:
	http://www2.research.att.com/~marioh/frequent-items/index.html

/* ******************************************************
/* ******************************************************

Original author: G. Cormode 2002, 2003,2005

This version
Author: Bernardino Frola
Created: 02-may-2011

Change log:
07-sept-2011: added spSS 
08-sept-2011: added parSS
09-sept-2011: rem spSS

Usage example:  
Test.exe -np 32000000 -r 2 -phi 0.0001 -parps 8 -stripSizeExp 24 -z 1.0 -session test261112 

	Generates 32M data items
	Runs 2 output sample
	Phi is 0.0001
	8 CPU threads
	Strip size is 2^24 (~16M items)
	Skew is 1.0
	The output will be stored in folder <VS Project>/Test/test261112/

/ *******************************************************/

#pragma once

#pragma warning(disable : 4018) //< signed/unsigned
#pragma warning(disable : 4146) //unary minus operator
#pragma warning(disable : 4244) //double to float
#pragma warning(disable : 4267) //+= conversion from size_t

#include "HelperDataGenerator.h"
#include "HelperPrint.h"
#include "HelperQuality.h"

#include "FrequentStripProcessor.h"
#include "SSStripProcessor.h"
#include "GpuFreqStripProcessor.h"
#include "SFStripProcessor.h"
#include "GpuSFStripProcessor.h"

#include <algorithm>
#include "Utils.h"

#include <fstream>

void usage()
{
	std::cerr
		<< "Usage: graham\n"
		<< "  -np   number of packets\n"
		<< "  -r    number of runs\n"
		<< "  -phi  phi\n"
		<< "  -d    depth\n"
		<< "  -g    granularity\n"
#ifdef PCAP
		<< "  -pf   pcap file name\n"
		<< "  -f    pcap filter\n"
#else
		<< "  -z    skew\n"
#endif
		<< std::endl;
}


int HandleArguments( 
	int argc, char ** argv, 
	size_t &stNumberOfPackets, size_t &stRuns, uint32_t &u32Depth, uint32_t &u32Granularity,
	double &dPhi, std::string &sPcapFile, std::string &sFilter, double &dSkew, 
	bool& pauseAfterEx,
	int& parPoolSize, 
	string& testSessionName,
	int& stripSizeExp,
	string& datasetPath) 
{
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-np") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing number of packets." << std::endl;
				return -1;
			}
			stNumberOfPackets = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-r") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing number of runs." << std::endl;
				return -1;
			}
			stRuns = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-d") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing depth." << std::endl;
				return -1;
			}
			u32Depth = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-g") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing granularity." << std::endl;
				return -1;
			}
			u32Granularity = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-phi") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing phi." << std::endl;
				return -1;
			}
			dPhi = atof(argv[i]);
		}
#ifdef PCAP
		else if (strcmp(argv[i], "-pf") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing pcap file name." << std::endl;
				return -1;
			}
			sPcapFile = std::string(argv[i]);
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing pcap filter." << std::endl;
				return -1;
			}
			sFilter = std::string(argv[i]);
		}
#else
		else if (strcmp(argv[i], "-z") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing skew parameter." << std::endl;
				return -1;
			}
			dSkew = atof(argv[i]);
		}
#endif
		else if (strcmp(argv[i], "-parps") == 0) // Parallel pool size
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing parps parameter." << std::endl;
				return -1;
			}
			parPoolSize = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-session") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing fout parameter." << std::endl;
				return -1;
			}
			testSessionName = string(argv[i]);
		}
		else if (strcmp(argv[i], "-stripSizeExp") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing stripSizeExp parameter." << std::endl;
				return -1;
			}
			stripSizeExp = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-dataset") == 0)
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "Missing dataset parameter." << std::endl;
				return -1;
			}
			datasetPath = string(argv[i]);
		}
		else if (strcmp(argv[i], "-pae") == 0)
		{
			// No need of i++ (no value for this parameter)
			pauseAfterEx = true;
		}
		else
		{
			usage();
			return -1;
		}
	}
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

/******************************************************************/

int testFrequentAlgorithm(int argc, char **argv) 
{
	size_t stNumberOfPackets = 1000000;
	size_t stRuns = 1; 

	double dPhi = 0.01;

	uint32_t u32Depth = 4;
	uint32_t u32Granularity = 8;
	std::string sFilter = "";
	std::string sPcapFile = "";

	int parPoolSize = 2;
	int stripSizeExp = 0;
	string datasetPath = "";

#ifdef PCAP
	sFilter = "ip and (tcp or udp)";
	sPcapFile = "login.pcap";
#else
	double dSkew = 1.0;
#endif

	bool pauseAfterEx = false;
	string testSessionName;
	int handleArgsResult = HandleArguments(argc, argv, stNumberOfPackets, stRuns, u32Depth, 
		u32Granularity, dPhi, sPcapFile, sFilter, dSkew, pauseAfterEx,
		parPoolSize, testSessionName, stripSizeExp, datasetPath);

	if (handleArgsResult < 0)
		return -1;

	if (testSessionName.length() == 0)
		testSessionName = "generic";

	// Output file name
	cout << "Test session (output directory): " << testSessionName << endl;
	
	// -----------------------------------------------------
	// Data

	PrintCheckPoint( "Data generation" );

	std::vector<uint32_t> data;
	// How many different elements = domain size
	uint32_t u32DomainSize = 1048575;
	
	if (datasetPath.length() == 0)
	{
		// Use generated data
		bool useFakeData = false;
		if (useFakeData)
			GenerateFakeData(sPcapFile, sFilter, 
				u32DomainSize, dSkew, stNumberOfPackets, data);
		else
			GenerateData(sPcapFile, sFilter, 
			u32DomainSize, dSkew, stNumberOfPackets, data);
	}
	else
	{
		// Use data from dataset on file
		BF::readDataset<uint32_t>(datasetPath, data);
		u32DomainSize = *max_element(data.begin(), data.end());
		stNumberOfPackets = data.size();
	}

	cout << "Domain size: " << u32DomainSize << endl; 
	std::vector<uint32_t> exact(u32DomainSize + 1, 0);

	// -----------------------------------------------------
	// -----------------------------------------------------

	PrintStartCheckPoint( "Set up..." );

	cout << "Configuration: " << 
		"N = " << stNumberOfPackets << ", " << 
		"Runs = " << stRuns  << ", " << 
		"phi = " << dPhi  << ", " <<
		"CPUpar = " << parPoolSize   << ", " << 
		"skewness (z) = " << dSkew << endl;
	
	uint32_t u32Width = 2.0 / dPhi;

	// -----------------------------------------------------
	// -----------------------------------------------------

	PrintCheckPoint( "Initialization" );

	Stats			// Stats collector
		SExact,		// Exact
		SLCU,		// Space-Saving serial
		SParSSL,	// Space-Saving parallel
		SGpuSF;		// Sort-based GPU parallel

	// Space-Saving serial
	LCU_type* lcu = LCU_Init(dPhi); // Yes, this is correct, U here stands for "unary"
	
	// Space-Saving parallel
	FIStripProcessor* spParSSL = new ParSSStripProcessor(dPhi, parPoolSize); // TEMP was parPoolSize

	int stripSize;
	// Default value of stripSize
	if (stripSizeExp == 0)
		stripSize = stNumberOfPackets / stRuns;
	else
		stripSize = (int) pow(2.0, stripSizeExp);

	// Sort-based GPU parallel
	GpuSFStripProcessor* spGpuSF = new GpuSFStripProcessor(dPhi, stripSize);

	// -----------------------------------------------------
	// -----------------------------------------------------

	PrintCheckPoint( "Copy data to strip data" );

	StripDataVector<int>* stripData = new StripDataVector<int>();
	stripData->resize(stNumberOfPackets);
	copy(data.begin(), data.end(), stripData->begin()); // Values OK

	PrintEndCheckPoint ();

	bool printData = false;
	if (printData)
	{
		cout << "\n";
		// Print data
		for (int i = 0; i < stNumberOfPackets; ++i)
		{
			cout << data[i] << ", ";
		}
		cout << "\n";
	}


	// ---------------------------------------------------------------
	// ---------------------------------------------------------------

	size_t stRunSize = data.size() / stRuns;
	size_t stStreamPos = 0;
	uint64_t nsecs;

	std::map<uint32_t, uint32_t> exactRes;
	uint32_t thresh; // Threshold used to compute outputs (thresh = N * phi)

	if (stRuns > 20)
		cout << "WARNING: Intrarun prints disabled (stRuns too high)" << endl;

	// Sample results stRuns times
	for (size_t run = 1; run <= stRuns; ++run)
	{
		PrintStartCheckPoint( "Update" );

		// Exact count of elements
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			exact[data[i]]++;
		}

#if ENABLE_ComputeExactRes
		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			Exact_Update(exactRes, data[i]);
		}
		SExact.dU += StopTheClock(nsecs);
#endif
		
		// ------------------------------------------
		// Space-Saving serial

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			LCU_Update(lcu, data[i]);
		}
		SLCU.dU += StopTheClock(nsecs);
		
		// ------------------------------------------
		// Space-Saving parallel

		StartTheClock(nsecs);
		spParSSL->StripOffset(stStreamPos);
		spParSSL->StripSize(stRunSize);
		spParSSL->process(stripData);
		SParSSL.dU += StopTheClock(nsecs);

		// ------------------------------------------
		// Sort-based GPU parallel

		StartTheClock(nsecs);
		spGpuSF->StripOffset(stStreamPos);
		spGpuSF->StripSize(stRunSize);
		spGpuSF->process(stripData);
		SGpuSF.dU += StopTheClock(nsecs);
		
		// ------------------------------------------

		if (run == 1) // Print this just the first iteration
			spGpuSF->printSubStripInfo(stripData);

		// --------------------------------------------------------------------
		// --------------------------------------------------------------------

		// Number of elements so far: floor(dPhi * run * stRunSize) 
		// Elements with frequency greater than thresh should be in T
		thresh = static_cast<uint32_t>(floor(dPhi * run * stRunSize));

		// Show elements with counter >= 1
		if (thresh == 0) thresh = 1;
		// Compute the number of elements in exact greater than thresh
		size_t hh = RunExact(thresh, exact);
		
		PrintEndCheckPoint( );

		if (stRuns < 21)
			std::cout << "Run: " << run << ", Exact: " << hh << 
				" Threshold: " << thresh << std::endl;


		std::map<uint32_t, uint32_t> res;

#if ENABLE_ComputeExactRes

		// -----------------------------------
		// Exact 

		StartTheClock(nsecs);
		res = Exact_Output(exactRes, thresh);
		SExact.dQ += StopTheClock(nsecs);

		PrintRes("Exact", res);
		CheckOutput(res, thresh, hh, SExact, exact);
#endif

		// -----------------------------------
		// Space-Saving serial

		StartTheClock(nsecs);
		res = LCU_Output(lcu,thresh);
		SLCU.dQ += StopTheClock(nsecs);
		PrintRes2File("LCU", res, "LCU_out.dat");
		CheckOutput(res, thresh, hh, SLCU, exact);

		// -----------------------------------
		// Space-Saving parallel

		StartTheClock(nsecs);
		res = spParSSL->getOutput(thresh);
		SParSSL.dQ += StopTheClock(nsecs);
		CondPrintRes("SParSSL  ", res);
		CheckOutput(res, thresh, hh, SParSSL, exact);

		// -----------------------------------
		// Sort-based GPU parallel

		StartTheClock(nsecs);
		res = spGpuSF->getOutput(thresh); 
		SGpuSF.dQ += StopTheClock(nsecs);
		CondPrintRes("GPUSF  ", res);
		PrintRes2File("GpuSF", res, "GpuSF_out.dat");
		CheckOutput(res, thresh, hh, SGpuSF, exact);

		// -----------------------------------
		// -----------------------------------

		stStreamPos += stRunSize;

	} // for processing
	
	cout << "----------------------------------" << endl;

	stringstream mkdirCmd;
	mkdirCmd << "mkdir FreqReports\\" << testSessionName;
	system(mkdirCmd.str().c_str());
	stringstream fileName;

	int phiPrecision = 0;

	double fVal = dPhi;
	while (abs(fVal - ceil(fVal)) > 0.0001)
	{
		fVal *= 10;
		phiPrecision ++;
	}

	fileName << "FreqReports\\" << testSessionName << "\\n" << stNumberOfPackets << "r" << stRuns << 
		"phi" << fixed << setprecision(phiPrecision) << dPhi <<
		"parcpu" << parPoolSize; 
		
	fileName.unsetf(ios::fixed);
	string safeDatasetPath = datasetPath;
	safeDatasetPath.erase (std::remove(safeDatasetPath.begin(), safeDatasetPath.end(), '/'), safeDatasetPath.end());
	fileName << "dataset" << safeDatasetPath <<  ".dat"; 

	cout << "FILE: " << fileName.str() << endl;
	
	// Write output to file
	ofstream fileStream;
	fileStream.open (fileName.str().c_str(), ios::trunc);

	// --------------------------------------------------------------------
	// --------------------------------------------------------------------
	// Results

	PrintOutputHeader(&fileStream);

#if ENABLE_ComputeExactRes
	PrintOutput("Exact", sizeof(exactRes), SExact, stNumberOfPackets);
#endif
	
	PrintOutput("SSL", LCU_Size(lcu), SLCU, stNumberOfPackets, &fileStream);

	PrintOutput("ParSSL",			
		spParSSL->getSize(), 
		SParSSL, stNumberOfPackets, &fileStream); 

	PrintOutput("GPUSB", 
		spGpuSF->getSize(), 
		SGpuSF, stNumberOfPackets, &fileStream); 

	cout << endl;

	
	fileStream.close();

	PrintImprov("SSL->ParSSL", SLCU, SParSSL);
	PrintImprov("SSL->GpuSF", SLCU, SGpuSF);
	PrintImprov("ParSSL->GpuSF", SParSSL, SGpuSF);

	// --------------------------------------------------------------------
	// --------------------------------------------------------------------
	// Clean

	// Exiting 
	LCU_Destroy(lcu);

	delete spParSSL;
	delete spGpuSF;

	// --------------------------------------------------------------------
	// --------------------------------------------------------------------

	printf("\n");

	if (pauseAfterEx)
	{
		system("pause");
	}

	return 0;
}


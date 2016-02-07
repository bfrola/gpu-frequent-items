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

void GenerateData( 
	std::string &sPcapFile, std::string &sFilter, 
	uint32_t u32DomainSize, double dSkew, 
	size_t stNumberOfPackets, std::vector<uint32_t> &data ) 
{

#ifdef PCAP
	//Open the capture file
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];

	std::cout << sPcapFile.c_str();
	system( "pause" );

	if ((fp = pcap_open_offline(sPcapFile.c_str(), errbuf)) == 0)
	{
		std::cerr << "Unable to open file." << std::endl;
		exit(1);
	}

	//compile the filter
	struct bpf_program fcode;
	if (pcap_compile(fp, &fcode, const_cast<char*>(sFilter.c_str()), 1, 0xffffff) < 0 )
	{
		std::cerr << "Unable to compile the packet filter. Check the syntax" << std::endl;
		exit(1);
	}

	//set the filter
	if (pcap_setfilter(fp, &fcode) < 0)
	{
		std::cerr << "Error setting the filter" << std::endl;
		exit(1);
	}

	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int res;
#endif

	prng_type * prng;
	prng=prng_Init(44545,2);
	int64_t a = (int64_t) (prng_int(prng)% MOD);
	int64_t b = (int64_t) (prng_int(prng)% MOD);
	prng_Destroy(prng);

#ifndef PCAP
	Tools::Random r = Tools::Random(0xF4A54B);
	Tools::PRGZipf zipf = Tools::PRGZipf(0, u32DomainSize, dSkew, &r);
#endif

	size_t stCount = 0;
#ifdef PCAP
	while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0 && stCount < stNumberOfPackets)
#else
	std::cout << "Data gen: ";
	data.resize(stNumberOfPackets);
	for (int i = 0; i < stNumberOfPackets; ++i)
#endif
	{
		++stCount;
		//if (stCount % 500000 == 0) 
		//	cout << ".";

		if (stCount == stNumberOfPackets * 0.25)
			std::cout << "25%...";
		else if (stCount == stNumberOfPackets * 0.5)
			std::cout << "50%...";
		else if (stCount == stNumberOfPackets * 0.75)
			std::cout << "75%...";

#ifdef PCAP
		ip_header *ih;
		udp_header *uh;
		tcp_header *th;
		u_int ip_len;
		u_short sport,dport;

		//retireve the position of the ip header
		ih = (ip_header *) (pkt_data + 14); //length of ethernet header
		ip_len = (ih->ver_ihl & 0xf) * 4;

		if (ih->proto == 6)
		{
			th = (tcp_header *) ((u_char*)ih + ip_len);
			sport = ntohs( th->sport );
			dport = ntohs( th->dport );
		}
		else if (ih->proto == 17)
		{
			uh = (udp_header *) ((u_char*)ih + ip_len);
			sport = ntohs( uh->sport );
			dport = ntohs( uh->dport );
		}

		uint32_t v;
		memcpy(&v, &(ih->daddr), 4);
#endif
#ifndef PCAP
		uint32_t v = zipf.nextLong();
#endif

		uint32_t value = hash31(a, b, v) & u32DomainSize;
		//data.push_back(value);
		data[i] = value;
	} // for

	std::cout << "100%" << endl;
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

void GenerateFakeData( 
	std::string &sPcapFile, std::string &sFilter, 
	uint32_t u32DomainSize, double dSkew, 
	size_t stNumberOfPackets, std::vector<uint32_t> &data ) 
{
	for (int i = 0; i < stNumberOfPackets; ++i)
	{
		//int val = 1 + i % 10;
		/*if (i<stNumberOfPackets / 2)
			data.push_back(i % 20);
		else if (i<stNumberOfPackets * 5 / 6)
			data.push_back(2);
		else
			data.push_back(3);*/

		data.push_back(rand() * 100 / RAND_MAX);

		//int val = i % 10;
		//data.push_back(val);
		//data.push_back(100);
	}

}

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

#include "RepositoryFlag.h"

// you need to include libraries wpcap.lib ws2_32.lib to compile using PCAP.

#include "prng.h"
#include "cgt.h"
#include "lossycount.h"
#include "frequent.h"
#include "qdigest.h"
#include "ccfc.h"
#include "countmin.h"
#include "gk4.h"


#ifdef __GNUC__
#include <sys/time.h>
#include <time.h>
#endif

/******************************************************************/

#ifdef PCAP
#include <pcap.h>

#ifdef __GNUC__
#include <arpa/inet.h>
#endif

typedef struct ip_address{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header{
    u_char  ver_ihl;        // Version (4 bits) + Internet header length (4 bits)
    u_char  tos;            // Type of service 
    u_short tlen;           // Total length 
    u_short identification; // Identification
    u_short flags_fo;       // Flags (3 bits) + Fragment offset (13 bits)
    u_char  ttl;            // Time to live
    u_char  proto;          // Protocol
    u_short crc;            // Header checksum
    ip_address  saddr;      // Source address
    ip_address  daddr;      // Destination address
    u_int   op_pad;         // Option + Padding
}ip_header;

/* UDP header*/
typedef struct udp_header{
    u_short sport;          // Source port
    u_short dport;          // Destination port
    u_short len;            // Datagram length
    u_short crc;            // Checksum
}udp_header;

typedef struct tcp_header{
	u_short sport;
	u_short dport;
}tcp_header;
#else
#ifdef _MSC_VER
#include <windows.h>
#endif
#endif

class Stats
{
public:
	Stats() : dU(0.0), dQ(0.0), dP(0.0), dR(0.0), dF(0.0), dF2(0.0) {}

	double dU;  // Tempo di esecuzioe
	double dQ;  // Tempo di esecuzione raccolta risultati
	
	double dP;  
	double dR;  
	double dF;  
	double dF2; 
	std::multiset<double> P;
	std::multiset<double> R;
	std::multiset<double> F;
	std::multiset<double> F2;
};

inline void StartTheClock(uint64_t& s)
{
#ifdef _MSC_VER
	FILETIME ft;
    LARGE_INTEGER li;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	s = (uint64_t) (li.QuadPart / 10000);
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	s = (1000 * tv.tv_sec) + (tv.tv_usec / 1000);
#endif
}

// returns milliseconds.
inline uint64_t StopTheClock(uint64_t s)
{
#ifdef _MSC_VER
    FILETIME ft;
    LARGE_INTEGER li;
    uint64_t t;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	t = (uint64_t) (li.QuadPart / 10000);
	return t - s;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (1000 * tv.tv_sec) + (tv.tv_usec / 1000) - s;
#endif
}

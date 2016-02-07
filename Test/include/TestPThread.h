#pragma once

#include <iostream>
#include <stdlib.h>

#include "pthread.h"

using namespace std;

// 100% CPU usage example (just a few seconds)
inline void* hello(void* id) {
	printf("%d: Hello world!\n", *((int*) id));
	
	cout << "Start" << endl;
	for (int i = 0; i < 10; i ++)
	{
		//Sleep (200);
		double a = 1.0, b = 2.0, tot = 0.0;
		for (int j = 0; j < 50000000; j ++)
			tot = a * b * j;
		cout << *((int*) id) << endl;
	}
	cout << "Endl" << endl;
	return 0;
}

// -------------------------------------------------

void testPosixThreads()
{
	const int COUNT = 5;
	int i;

	pthread_t thread[COUNT];
	int ids[COUNT];

	for (i = 0; i < COUNT; i++) {
		ids[i] = i;
		int retval = pthread_create(&thread[i], NULL, hello, &ids[i]);
		if (retval) {
			perror("pthread_create failed");
			return;
		}
	}

	for (i = 0; i < COUNT; i++) 
		pthread_join(thread[i], NULL);

}
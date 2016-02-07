void PrintOutput(char* title, size_t size, const Stats& S, size_t u32NumberOfPackets)
{
	double p5th = -1.0, p95th = -1.0, r5th = -1.0, r95th = -1.0, f5th = -1.0, f95th = -1.0, f25th = -1.0, f295th = -1.0;
	size_t i5, i95;
	std::multiset<double>::const_iterator it;

	if (! S.P.empty())
	{
		it = S.P.begin();
		i5 = S.P.size() * 0.05;
		for (size_t i = 0; i < i5; ++i) ++it;
		p5th = *it;
		i95 = S.P.size() * 0.95;
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		p95th = *it;
	}

	if (! S.R.empty())
	{
		it = S.R.begin();
		i5 = S.R.size() * 0.05;
		for (size_t i = 0; i < i5; ++i) ++it;
		r5th = *it;
		i95 = S.R.size() * 0.95;
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		r95th = *it;
	}

	if (! S.F.empty())
	{
		it = S.F.begin();
		i5 = S.F.size() * 0.05;
		for (size_t i = 0; i < i5; ++i) ++it;
		f5th = *it;
		i95 = S.F.size() * 0.95;
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		f95th = *it;
	}

	if (! S.F2.empty())
	{
		it = S.F2.begin();
		i5 = S.F2.size() * 0.05;
		for (size_t i = 0; i < i5; ++i) ++it;
		f25th = *it;
		i95 = S.F2.size() * 0.95;
		for (size_t i = 0; i < (i95 - i5); ++i) ++it;
		f295th = *it;
	}

	std::cout << title << "\t" <<
		u32NumberOfPackets / S.dU << "\t" <<
		size << std::endl;
	
	/*
	printf("%s\t%1.2f\t\t%d\t\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\n",
		title, 
		u32NumberOfPackets / S.dU, 
		size,
		S.dR / S.R.size(), r5th, r95th,
		S.dP / S.P.size(), p5th, p95th,
		S.dF / S.F.size(), f5th, f95th,
		S.dF2 / S.F2.size(), f25th, f295th
	);
	*/
	
	/*
	printf("%s\t%1.2f\t\t%d\t\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\t%1.2f\n",
		title, 
		u32NumberOfPackets / S.dU, 
		size,
		S.dR / S.R.size(), r5th, r95th,
		S.dP / S.P.size(), p5th, p95th,
		S.dF / S.F.size(), f5th, f95th,
		S.dF2 / S.F2.size(), f25th, f295th
	);
	*/
}
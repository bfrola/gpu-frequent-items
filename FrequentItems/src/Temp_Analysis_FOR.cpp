for (size_t run = 1; run <= stRuns; ++run)
	{
		// Conteggia in modo esatto gli elementi
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			exact[data[i]]++;
		}

		// Frequent
		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			Freq_Update(freq, data[i]);
		}
		SFreq.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			QD_Insert(qd, data[i], 1);
		}
		SQD.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			if (data[i]>0)
				CMH_Update(cmh,data[i],1);      
			else
				CMH_Update(cmh,-data[i],-1);
		}
		SCMH.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			if (data[i]>0)
			  CGT_Update(cgt,data[i],1);      
			else
			CGT_Update(cgt,-data[i],-1);
		}
		SCGT.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			LC_Update(lc,data[i]);
		}
		SLC.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			LCD_Update(lcd,data[i]);
		}
		SLCD.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			LCL_Update(lcl,data[i],1);
		}
		SLCL.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			LCU_Update(lcu, data[i]);
		}
		SLCU.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			if (data[i]>0)
				CCFC_Update(ccfc,data[i],1);      
			else
				CCFC_Update(ccfc,-data[i],-1);
		}
		SCCFC.dU += StopTheClock(nsecs);

		StartTheClock(nsecs);
		for (size_t i = stStreamPos; i < stStreamPos + stRunSize; ++i)
		{
			gk4.insert(data[i]);
		}
		SGK4.dU += StopTheClock(nsecs);

		uint32_t thresh = static_cast<uint32_t>(floor(dPhi * run * stRunSize));
		if (thresh == 0) thresh = 1;

		size_t hh = RunExact(thresh, exact);
		// hh contiene il numero di elementi di exact maggiori di thresh

		std::cerr << "Run: " << run << ", Exact: " << hh << std::endl;

		std::map<uint32_t, uint32_t> res;

		StartTheClock(nsecs);
		res = Freq_Output(freq,thresh);
		SFreq.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SFreq, exact);

		StartTheClock(nsecs);
		QD_Compress(qd);
		SQD.dU += StopTheClock(nsecs);
		StartTheClock(nsecs);
		res = QD_FindHH(qd, thresh);
		SQD.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SQD, exact);

		StartTheClock(nsecs);
		res = CMH_FindHH(cmh,thresh);
		SCMH.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SCMH, exact);

		StartTheClock(nsecs);
		res = CGT_Output(cgt,thresh);
		SCGT.dQ+= StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SCGT, exact);

		StartTheClock(nsecs);
		res = LC_Output(lc,thresh);
		SLC.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SLC, exact);
		  
		StartTheClock(nsecs);
		res = LCD_Output(lcd,thresh);
		SLCD.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SLCD, exact);
		  
		StartTheClock(nsecs);
		res = LCL_Output(lcl,thresh);
		SLCL.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SLCL, exact);

		StartTheClock(nsecs);
		res = LCU_Output(lcu,thresh);
		SLCU.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SLCU, exact);

		StartTheClock(nsecs);
		res = CCFC_Output(ccfc,thresh);
		SCCFC.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SCCFC, exact);

		StartTheClock(nsecs);
		res = gk4.getHH(thresh);
		SGK4.dQ += StopTheClock(nsecs);
		CheckOutput(res, thresh, hh, SGK4, exact);

		stStreamPos += stRunSize;
	}
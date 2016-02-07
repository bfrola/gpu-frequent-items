void CheckOutput(
std::map<uint32_t, uint32_t>& res, 
uint32_t thresh, size_t hh, Stats& S, const std::vector<uint32_t>& exact)
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
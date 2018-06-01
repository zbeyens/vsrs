#pragma once

#include "SystemIncludes.h"

class Clock
{
public:
	Clock();

	/*!
		Start the clock
	*/
	void setStartTime();
	/*!
		Print the time from the start of the clock 
	*/
	void setEndTime();
	void setTotalTime();

private:
	clock_t start, finish, first;
};

Clock::Clock()
{
	first = start = clock();
}

inline void Clock::setStartTime()
{
	finish = clock();
	cout << "Initialization: " << (double)(finish - start) / CLOCKS_PER_SEC << " sec" << endl;
	start = finish;
}

inline void Clock::setEndTime()
{
	finish = clock();
	cout << "->End (" << (double)(finish - start) / CLOCKS_PER_SEC << " sec)" << endl;
	start = finish;
}

inline void Clock::setTotalTime()
{
	finish = clock();
	cout << "Total: " << ((double)(finish - first)) / ((double)CLOCKS_PER_SEC) << " sec" << endl;
}

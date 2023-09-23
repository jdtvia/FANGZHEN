#include "SimNode.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

#include <vector>

#define ulong unsigned long

#include "SimSchedule.h"

int main(int argc, char** argv)
{
	//ofstream trace(".\\traces\\trace.txt",ios::app);

	const int    allNum = 2;
	const double Pd = 1.0;
	const double Per = 0.0;
	const ulong  sim_time = 1e6;
	double ans = 0;

	for (int i = 0; i < 5; i++) {
		double tmp = sim_main(allNum, Pd, Per, sim_time);
		ans += tmp;
		printf("%.9lf\n ", tmp);
	}

	ans /= 5;
	printf("%.9lf ", ans);
	//trace.close();
	return EXIT_SUCCESS;
}


#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = test_IntelTBB_3();
	if (ret == 0) fprintf(stderr, "test success\n");
	else fprintf(stderr, "test fail\n");

	return 0;
}


#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = test_FaceDetect_HOG();

	if (ret == 0) fprintf(stdout, "====== test success ======\n");
	else fprintf(stderr, "###### test fail ######\n");
}

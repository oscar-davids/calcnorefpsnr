#include <stdio.h>
#include <stdio.h>
#include "norefpsnr.h"

int main(int argc, char **argv)
{
	printf("Welcome to test norefpsnr!\n");

	if (argc != 2) {
		printf("command example: ./dlltest test.h264\n");
		exit(0);
	}

	float fpsnr = calc_norefpsnr(argv[1]);
	printf("%s psnr: %lf\n", argv[1], fpsnr);

	return 0;
}
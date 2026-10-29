#include <stdio.h>
#include <string.h>

#include "wrappers.h"

int main(int argc, char **argv)
{
	long result;

	if (argc != 2) {
		fprintf(stderr, "usage: %s --start or --stop\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "--start") == 0)
		result = start_elevator();
	else if (strcmp(argv[1], "--stop") == 0)
		result = stop_elevator();
	else {
		fprintf(stderr, "usage: %s --start or --stop\n", argv[0]);
		return 1;
	}

	printf("returned %ld\n", result);
	return result < 0 ? 1 : 0;
}

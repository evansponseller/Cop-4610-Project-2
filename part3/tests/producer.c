#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "wrappers.h"

static int random_value(int minimum, int maximum)
{
	return minimum + rand() % (maximum - minimum + 1);
}

int main(int argc, char **argv)
{
	int count;
	int index;
	int start_floor;
	int destination_floor;
	int type;
	long result;

	if (argc != 2 || sscanf(argv[1], "%d", &count) != 1 || count < 0) {
		fprintf(stderr, "usage: %s number_of_pets\n", argv[0]);
		return 1;
	}

	srand((unsigned int)time(NULL));
	for (index = 0; index < count; index++) {
		type = random_value(0, 3);
		start_floor = random_value(1, 5);
		do {
			destination_floor = random_value(1, 5);
		} while (destination_floor == start_floor);

		result = issue_request(start_floor, destination_floor, type);
		printf("issue %d %d %d returned %ld\n", start_floor,
		       destination_floor, type, result);
	}

	return 0;
}

#include <stdio.h>

#include "wrappers.h"

int main(void)
{
	printf("start_elevator returned %ld\n", start_elevator());
	printf("issue_request returned %ld\n", issue_request(1, 2, 0));
	printf("stop_elevator returned %ld\n", stop_elevator());
	return 0;
}

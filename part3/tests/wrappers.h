#ifndef ELEVATOR_WRAPPERS_H
#define ELEVATOR_WRAPPERS_H

#define _GNU_SOURCE
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_START_ELEVATOR 548
#define __NR_ISSUE_REQUEST 549
#define __NR_STOP_ELEVATOR 550

static inline long start_elevator(void)
{
	return syscall(__NR_START_ELEVATOR);
}

static inline long issue_request(int start_floor, int destination_floor,
					int type)
{
	return syscall(__NR_ISSUE_REQUEST, start_floor, destination_floor, type);
}

static inline long stop_elevator(void)
{
	return syscall(__NR_STOP_ELEVATOR);
}

#endif

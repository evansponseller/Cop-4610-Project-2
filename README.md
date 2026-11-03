# Cop-4610-Project-2

This project consists of three parts: system-call tracing, a timer kernel module, and a pet elevator kernel module. The work involves tracing system calls with `strace`, implementing kernel modules and proc entries, adding system calls to the Linux kernel, and testing the elevator with the provided user-space programs.

## Group Members

1. **Cody Cintron**
2. **Jaime Flandes**
3. **Evan Sponseller**

## Division of Labor

### Part 1: System-Call Tracing

- **Responsibilities**: Create the empty and `part1` C programs, add exactly five system calls to `part1`, and generate the `strace` output files.
- **Assigned to**: Evan Sponseller and Cody Cintron

### Part 2: Timer Kernel Module

- **Responsibilities**: Implement the `my_timer` kernel module using `ktime_get_real_ts64()`. Create and remove `/proc/timer` when the module is loaded and unloaded, and report the current time and elapsed time on each read.
- **Assigned to**: Evan Sponseller and Jaime Flandes

### Part 3a: Adding System Calls

- **Responsibilities**: Add `start_elevator()`, `issue_request()`, and `stop_elevator()` as system calls numbered 548, 549, and 550. Update the required Linux kernel files.
- **Assigned to**: Cody Cintron and Jaime Flandes

### Part 3b: Kernel Compilation

- **Responsibilities**: Configure, compile, and install the modified Linux 6.16.x kernel, then verify the installed kernel version.
- **Assigned to**: Evan Sponseller and Cody Cintron

### Part 3c: Threads

- **Responsibilities**: Implement and use a kernel thread (`kthread`) to control elevator movement and process pet requests.
- **Assigned to**: Evan Sponseller and Jaime Flandes

### Part 3d: Linked List

- **Responsibilities**: Use linked lists to manage pets waiting on each floor and pets currently inside the elevator.
- **Assigned to**: Cody Cintron and Jaime Flandes

### Part 3e: Mutexes

- **Responsibilities**: Use a mutex to protect shared data accessed by floor and elevator operations.
- **Assigned to**: Evan Sponseller and Cody Cintron

### Part 3f: Scheduling Algorithm

- **Responsibilities**: Implement the elevator scheduling algorithm, including pet loading and unloading, floor movement, capacity and weight limits, and shutdown behavior.
- **Assigned to**: Evan Sponseller and Jaime Flandes

## File Listing

The following is an expected project layout based on the assignment requirements. Update it if the starter code or your final implementation uses different filenames or directories.

```text
shell/
├── part1/
│   ├── Makefile
│   ├── empty.c
│   ├── empty.trace
│   ├── part1.c
│   └── part1.trace
│
├── part2/
│   ├── src/
│   │   └── my_timer.c
│   └── Makefile
│
├── part3/
│   ├── src/
│   │   ├── syscalls.c
│   ├── tests/
│   │   └── test_syscalls.c
│   ├── syscalls/
│   │   ├── syscalls.c
│   │   └── Makefile
│   ├── elevator.c
│   ├── producer.c
│   ├── consumer.c
│   └── Makefile
├── .gitignore
├── Makefile
└── README.md
```

The modified Linux kernel source tree also contains the system-call changes listed in the **Adding System Calls** section. It may be maintained separately from this project directory.

## How to Compile and Run

### Requirements

- **Compiler**: GCC
- **Build System**: Make
- **Tracing utility**: `strace`
- **Kernel tools**: Linux kernel headers/source, kernel build tools, and permission to load kernel modules
- **Environment**: A compatible Linux system running the modified kernel for Part 3

### Part 1: Compile and Trace

From the Part 1 directory, run:

```bash
gcc -o empty empty.c
strace -o empty.trace ./empty
gcc -o part1 part1.c
strace -o part1.trace ./part1
```

### Part 2: Build and Run the Timer Module

From the directory containing the timer module's Makefile, build it:

```bash
make
```

Load and inspect it:

```bash
sudo insmod my_timer.ko
cat /proc/timer
```

Remove it when finished:

```bash
sudo rmmod my_timer
```

### Part 3: Build and Run the Elevator

First boot into the modified kernel and verify it with `uname -r`. Build the system-call test and elevator components using the Makefiles provided with the project.

Test the system calls:

```bash
cd part3/tests
make
sudo insmod syscalls.ko
./test_syscalls
```

Load the elevator module and start the elevator using the provided consumer program:

```bash
sudo insmod elevator.ko
./consumer --start
```

Create pet requests and monitor the proc entry:

```bash
./producer [number_of_pets]
watch -n 1 cat /proc/elevator
```

Stop the elevator and unload the module after it has finished servicing pets:

```bash
./consumer --stop
sudo rmmod elevator
```

## Development Log


### Cody Cintron

| Date | Work Completed / Notes |
| --- | --- |
| YYYY-MM-DD | Record work completed on Part 1, Part 3a, Part 3b, Part 3d, and Part 3e as applicable. |

### Jaime Flandes

| Date | Work Completed / Notes |
| --- | --- |
| YYYY-MM-DD | Record work completed on Part 2, Part 3a, Part 3c, Part 3d, and Part 3f as applicable. |

### Evan Sponseller

| Date | Work Completed / Notes |
| --- | --- |
| YYYY-MM-DD | Record work completed on Part 1, Part 2, Part 3b, Part 3c, Part 3e, and Part 3f as applicable. |

## Meetings

| Date | Attendees | Topics Discussed | Outcomes / Decisions |
| --- | --- | --- | --- |
| YYYY-MM-DD | Cody, Jaime, Evan | Discussed task assignments, project requirements, and implementation plans for system-call tracing, the timer module, and the elevator. | Confirmed the division of labor and identified the next development tasks. |


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

The repository is organized as follows:

```text
Cop-4610-Project-2-main/
├── .gitignore
├── Makefile
├── README.md
│
├── part1/
│   ├── Makefile
│   ├── empty.c
│   ├── empty.trace
│   ├── part1.c
│   └── part1.trace
│
├── part2/
│   ├── Makefile
│   └── src/
│       └── my_timer.c
│
└── part3/
    ├── Makefile
    ├── README.md
    ├── syscalls.c
    ├── src/
    │   ├── elevator.c
    │   └── local_stubs.c
    └── tests/
        ├── elevator-test/
        │   ├── Makefile
        │   ├── README.md
        │   ├── consumer.c
        │   ├── producer.c
        │   └── wrappers.h
        └── system-calls-test/
            ├── Makefile
            ├── README.md
            ├── syscheck.c
            ├── test-syscalls.c
            └── test-syscalls.h
```

The Linux kernel source tree modified for Part 3 is maintained separately from this repository. The `part3/syscalls.c` file is copied into that kernel tree as part of the system-call integration process. See `part3/README.md` for the required kernel changes.

## How to Compile and Run

### Requirements

- A Linux environment with GCC and GNU Make.
- `strace` for Part 1.
- Linux kernel headers/build files matching the kernel you are building modules for (normally available under `/lib/modules/$(uname -r)/build`).
- Root privileges (`sudo`) to load and unload kernel modules.
- For the full Part 3 system-call functionality, the separately modified Linux kernel must be compiled, installed, and booted.

Run the commands below from the repository's root directory unless another directory is specified.

### Part 1: System-Call Tracing

Build both programs:

```bash
make -C part1
```

Run each program under `strace` and save its trace output:

```bash
cd part1
strace -o empty.trace ./empty
strace -o part1.trace ./part1
cd ..
```

Alternatively, regenerate both trace files through the Part 1 Makefile:

```bash
make -C part1 trace
```

### Part 2: Timer Kernel Module

Build the timer module against the currently running kernel:

```bash
make -C part2
```

Load the module and read its proc entry:

```bash
sudo insmod part2/my_timer.ko
cat /proc/timer
```

When finished, unload the module:

```bash
sudo rmmod my_timer
```

To remove generated build files:

```bash
make -C part2 clean
```

### Part 3: Elevator Kernel Module and System Calls

**Important:** Part 3 depends on kernel integration work outside this repository.

The three system calls (`start_elevator`, `issue_request`, and `stop_elevator`) must first be added to the Linux kernel source, and that modified kernel must be built, installed, and booted. The exact kernel integration steps and syscall numbers are documented in `part3/README.md`.

Confirm that the modified kernel is running:

```bash
uname -r
```

The module Makefile builds against the kernel currently running on the machine.

On the modified kernel, build the elevator module without the local stub module:

```bash
make -C part3 LOCAL_STUBS=0
```

On a stock kernel, the default build includes `local_stubs.ko`. Those stubs allow the elevator module to load for module-level testing, but they do not add the system calls to the kernel and do not provide the full syscall functionality:

```bash
make -C part3
```

#### Build the User-Space Test Programs

Build the elevator producer and consumer programs:

```bash
make -C part3/tests/elevator-test
```

Build the system-call test program:

```bash
cd part3/tests/system-calls-test
gcc test-syscalls.c -o test-syscalls
cd ../../..
```

#### Check the System Calls (Modified Kernel)

With the modified kernel booted and the elevator module unloaded, build and load the `syscheck` module. Then run the system-call test program:

```bash
cd part3/tests/system-calls-test
make syscheck.ko
sudo insmod syscheck.ko
./test-syscalls
sudo rmmod syscheck
cd ../../..
```

The `syscheck` module supplies temporary stub implementations so the system calls can be invoked for this check. It is a test helper, not the elevator implementation.

#### Run the Elevator

On the modified kernel, load the elevator module:

```bash
sudo insmod part3/elevator.ko
```

On a stock kernel used only for module-loading tests, load the local stubs first, followed by the elevator module:

```bash
sudo insmod part3/local_stubs.ko
sudo insmod part3/elevator.ko
```

Start the elevator, submit ten requests, and monitor its status:

```bash
cd part3/tests/elevator-test
./consumer --start
./producer 10
watch -n 1 cat /proc/elevator
```

When the elevator has finished servicing requests, stop it:

```bash
./consumer --stop
cd ../../..
```

Unload modules in reverse order.

On the modified kernel:

```bash
sudo rmmod elevator
```

If using the stock-kernel stub setup, unload both modules:

```bash
sudo rmmod elevator
sudo rmmod local_stubs
```

The producer selects floors 1 through 6. The elevator implementation may reject requests involving floor 6; consult `part3/README.md` for this implementation-specific behavior.

To clean generated files, run:

```bash
make clean
```

## Development Log
Each member's primary contributions are recorded below.

### Cody Cintron

| Date | Work Completed / Notes |
| --- | --- |
| 2026-10-28 | Completed Part 2 and started on Part 3a, 3b, 3d, and 3e. |
| 2026-10-29 | Completed Part 3a, 3b, 3d, and 3e.  |
| 2026-11-02 | Ran testing and did bug-fixing. |

### Jaime Flandes

| Date | Work Completed / Notes |
| --- | --- |
| 2026-10-28 | Assisted completing Part 2 |
| 2026-10-29 | Assisted completing Part 3a, 3c, 3d, and 3f. |
| 2026-11-02 | Finished writing the README.md |

### Evan Sponseller

| Date | Work Completed / Notes |
| --- | --- |
| 2026-10-28 | Completed Part 1 while making the GitHub Repository for everyone else to work on. |
| 2026-10-29 | Assisted completing Part 3b, 3e, 3c, and 3f. |
| 2026-11-02 | Ran testing and did bug-fixing. |

## Meetings

| Date | Attendees | Topics Discussed | Outcomes / Decisions |
| --- | --- | --- | --- |
| 2026-10-13 | Cody, Jaime, Evan | Discussed task assignments, project requirements, and implementation plans for system-call tracing, the timer module, and the elevator. | Confirmed the division of labor and identified the next development tasks each person would do from home. |

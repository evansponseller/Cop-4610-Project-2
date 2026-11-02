# COP4610 Project 2: System Calls, Timer, and Pet Elevator

A Linux systems programming project involving system-call tracing, a timer kernel module, and a pet elevator kernel module. The project includes modifying and compiling the Linux kernel, implementing system calls, and using proc entries to report kernel-module status.

## Group Members

- **Cody Cintron**: [crc24c@fsu.edu](mailto:crc24c@fsu.edu)
- **Jaime Flandes**: [jmf23p@fsu.edu](mailto:jmf23p@fsu.edu)
- **Evan Sponseller**: [ets24a@fsu.edu](mailto:ets24a@fsu.edu)

## Division of Labor

### Part 1: System-Call Tracing

- **Responsibilities**: Create the empty and part1 C programs, add exactly five system calls to part1, and generate the corresponding `strace` output files.
- **Assigned to**: To be determined

### Part 2: Timer Kernel Module

- **Responsibilities**: Implement the `my_timer` kernel module using `ktime_get_real_ts64()`. Create and remove `/proc/timer` when the module is loaded and unloaded, and report the current time and elapsed time on each read.
- **Assigned to**: To be determined

### Part 3: Elevator Module

- **Responsibilities**: Implement the pet elevator kernel module, including the elevator system calls, kernel modification and installation, pet-request handling, elevator scheduling, and the `/proc/elevator` status interface.
- **Assigned to**: To be determined

### Part 3, Step 1: System Calls and Kernel Modification

- **Responsibilities**: Add `start_elevator()`, `issue_request()`, and `stop_elevator()` as system calls numbered 548, 549, and 550. Modify the required kernel files.
- **Assigned to**: To be determined

### Part 3, Step 2: Kernel Compilation and Installation

- **Responsibilities**: Configure, compile, and install the modified Linux 6.16.x kernel, then verify the installed version.
- **Assigned to**: To be determined

### Part 3, Step 3: System-Call Testing

- **Responsibilities**: Build and run the provided system-call tests to verify that all three system calls are installed and callable.
- **Assigned to**: To be determined

### Part 3, Steps 4–6: Elevator Implementation and Testing

- **Responsibilities**: Implement elevator and floor queues using linked lists, control elevator movement with a kernel thread, protect shared data with a mutex, allocate pet requests dynamically, expose elevator information through `/proc/elevator`, and test using the provided producer and consumer programs.
- **Assigned to**: To be determined

## Project Requirements

### Part 1: System-Call Tracing

Create an empty C program named `empty`, then copy it to create `part1`. Add exactly five system calls to `part1`.

Compile and trace both programs:

```bash
gcc -o empty empty.c
strace -o empty.trace ./empty
gcc -o part1 part1.c
strace -o part1.trace ./part1
```

The `part1` trace should contain five more system calls than the empty-program trace. Keep the program minimal and avoid unnecessary function calls so that the `strace` output is easy to inspect.

**Files to submit:**

- `empty.c`
- `empty.trace`
- `part1.c`
- `part1.trace`

### Part 2: Timer Kernel Module

Create a kernel module named `my_timer` that uses `ktime_get_real_ts64()` to retrieve and store the current time as seconds and nanoseconds since the Unix Epoch.

When loaded, the module must create `/proc/timer`. When unloaded, it must remove that proc entry. Each read of `/proc/timer` must display the current time and, when a previous read exists, the elapsed time since that previous call.

Load the module:

```bash
sudo insmod my_timer.ko
```

Check that it is loaded:

```bash
lsmod | grep my_timer
```

Read the proc entry:

```bash
cat /proc/timer
```

Remove the module:

```bash
sudo rmmod my_timer
```

### Part 3: Pet Elevator Kernel Module

Implement a kernel module named `elevator` that schedules pet requests in a five-floor building.

#### Elevator Rules

- The elevator can carry a maximum of **5 pets** and a maximum total weight of **50 lbs**.
- Each pet is randomly selected with equal likelihood as a Chihuahua, Pug, Pughuahua, or Dachshund.
- Pets select a starting floor and destination floor.
- Pets board in first-in, first-out (FIFO) order.
- Pet weights:
  - Chihuahua: **3 lbs**
  - Pug: **14 lbs**
  - Pughuahua: **10 lbs**
  - Dachshund: **16 lbs**
- Pets waiting on floors must remain queued until serviced.
- Once a pet boards, it can only disembark at its destination.
- Moving between floors takes **2.0 seconds**.
- Loading and unloading pets takes **1.0 second**.
- Floor 1 is the lobby and minimum floor; floor 5 is the maximum floor.
- Pets may arrive at any time, and each floor must support an arbitrary number of waiting pets.

#### Step 1: Add System Calls

Download the latest stable Linux 6.16.x kernel from [kernel.org](https://www.kernel.org/). Move the kernel source into `/usr/src/` and create a symbolic link in your home directory so it can be modified from a writable location.

Add the following system calls:

| System-call number | Function |
| --- | --- |
| 548 | `start_elevator()` |
| 549 | `issue_request()` |
| 550 | `stop_elevator()` |

Required function prototypes and behavior:

```c
int start_elevator(void);
int issue_request(int start_floor, int destination_floor, int type);
int stop_elevator(void);
```

- **`start_elevator()`** activates the elevator. It returns `1` if the elevator is already active, `0` on a successful start, or a negative error value if initialization fails (including `-ENOMEM` if memory allocation fails). Initial state: `IDLE`, floor `1`, load `0`.
- **`issue_request()`** creates a pet request. Pet types are `0 = Chihuahua`, `1 = Pug`, `2 = Pughuahua`, and `3 = Dachshund`. It returns `1` for an invalid request (such as an out-of-range floor or invalid type) and `0` otherwise.
- **`stop_elevator()`** begins deactivation. It stops accepting new requests but must deliver all pets already on the elevator before becoming `OFFLINE`. It returns `1` if deactivation is already in progress and `0` otherwise.

Create these files:

- `[kernel_version]/syscalls/syscalls.c`
- `[kernel_version]/syscalls/Makefile`

Modify these files:

- `[kernel_version]/arch/x86/syscalls/syscall_64.tbl`
- `[kernel_version]/include/linux/syscalls.h`
- `[kernel_version]/Makefile`

#### Step 2: Compile and Install the Kernel

Disable the required certificates as described in the provided slides, then configure and compile the modified kernel.

```bash
make menuconfig
make -j$(nproc)
sudo make modules_install
sudo make install
sudo reboot
```

After rebooting, verify the installed kernel:

```bash
uname -r
```

**Note:** Kernel compilation and installation can take a significant amount of time. Complete this step early.

#### Step 3: Test System Calls

Use the provided tests in `part3/tests/` to verify the new system calls:

```bash
make
sudo insmod syscalls.ko
./test_syscalls
```

The test program should report whether the system calls are installed.

#### Step 4: Implement the Elevator

Implement the `elevator` kernel module. It must be loadable with `insmod` and removable with `rmmod`.

The implementation must use:

- Linked lists to manage pets waiting on each floor and pets inside the elevator.
- A kernel thread (`kthread`) to control elevator movement.
- A mutex to protect shared data accessed by floor and elevator operations.
- `kmalloc` to allocate memory dynamically for pet requests.

#### Step 5: Create the Proc File

The module must create `/proc/elevator` and print the following information with appropriate labels:

- Elevator movement state:
  - `OFFLINE`: module is installed, but the elevator is not running (initial state).
  - `IDLE`: elevator is stopped on a floor because there are no more pets to service.
  - `LOADING`: elevator is stopped on a floor to load and unload pets.
  - `UP`: elevator is moving from a lower floor to a higher floor.
  - `DOWN`: elevator is moving from a higher floor to a lower floor.
- Current floor.
- Current elevator load in pounds.
- List of pets currently in the elevator.
- Total number of pets.
- Total number of pets waiting.
- Total number of pets serviced.

For each floor, print:

- An indicator showing whether the elevator is currently on that floor.
- Number of waiting pets.
- Each waiting pet's type and destination floor, represented by two characters.

Pet abbreviations:

| Character | Pet type |
| --- | --- |
| `C` | Chihuahua |
| `P` | Pug |
| `H` | Pughuahua |
| `D` | Dachshund |

Example output format:

```text
Elevator state: LOADING
Current floor: 4
Current load: 30 lbs
Elevator status: C5 D2 P4 H1
[ ] Floor 5: 1 H3
[ ] Floor 4: 0
[*] Floor 3: 2 P5 C1
[ ] Floor 2: 1 D4
[ ] Floor 1: 0
Number of pets: 4
Number of pets waiting: 4
Number of pets serviced: 27
```

#### Step 6: Test the Elevator

Use the provided user-space programs:

- **`producer.c`** creates pets and issues requests:
  ```bash
  ./producer [number_of_pets]
  ```
- **`consumer.c`** starts or stops the elevator:
  ```bash
  ./consumer --start
  ./consumer --stop
  ```

To monitor the elevator, use:

```bash
watch -n [secs] cat /proc/elevator
```

The `producer.c` and `consumer.c` programs will be provided.

## Expected Files

### Part 1

- `empty.c`
- `empty.trace`
- `part1.c`
- `part1.trace`

### Part 2

- Source file(s) for the `my_timer` kernel module.
- A Makefile to build the module, if required by the starter project.

### Part 3

- Modified Linux kernel source files for the three system calls.
- `syscalls/syscalls.c`
- `syscalls/Makefile`
- Source file(s) for the `elevator` kernel module.
- Any required Makefiles and user-space test files.

## Development Log

Each member's primary contributions should be recorded below.

### Cody Cintron

| Date | Work Completed / Notes |
| --- | --- |
| YYYY-MM-DD | Add completed tasks and contributions. |

### Jaime Flandes

| Date | Work Completed / Notes |
| --- | --- |
| YYYY-MM-DD | Add completed tasks and contributions. |

### Evan Sponseller

| Date | Work Completed / Notes |
| --- | --- |
| --- | --- |

## Meetings

Document in-person meetings, their purpose, and what was discussed.

| Date | Attendees | Topics Discussed | Outcomes / Decisions |
| --- | --- | --- | --- |
| YYYY-MM-DD | Cody, Jaime, Evan | Add meeting topics. | Add outcomes and decisions. |

## Considerations

The project is divided into three parts: system-call tracing, a timer kernel module, and a pet elevator kernel module. The elevator portion requires changes to the Linux kernel as well as coordination between system calls, kernel-thread scheduling, shared data structures, and proc-file reporting. Keep the implementation modular and document each member's contributions and testing as development progresses.

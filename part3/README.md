# part 3

## kernel integration

create a `syscalls` directory in the linux source tree and copy `syscalls.c`
into it

add a `Makefile` to the same `syscalls` directory containing

```text
obj-y += syscalls.o
```

add `syscalls/` to the top level kernel makefile

add the three entries below to `arch/x86/entry/syscalls/syscall_64.tbl`

```text
548 common start_elevator sys_start_elevator
549 common issue_request sys_issue_request
550 common stop_elevator sys_stop_elevator
```

add the three declarations below to `include/linux/syscalls.h`

```c
asmlinkage long sys_start_elevator(void);
asmlinkage long sys_issue_request(int start_floor, int destination_floor, int type);
asmlinkage long sys_stop_elevator(void);
```

## build

on the custom kernel build only the elevator module with

```text
make LOCAL_STUBS=0
```

on a stock kernel the default build also produces `local_stubs.ko`, which
exports the three stub pointers so the elevator module can be loaded without
the syscalls

```text
make
```

build the user programs with

```text
cd tests
make
```

## runtime

on the custom kernel load only the elevator module

```text
sudo insmod elevator.ko
```

on a stock kernel load the local pointer module first

```text
sudo insmod local_stubs.ko
sudo insmod elevator.ko
```

the local pointer module only validates module loading and does not add syscall
table entries

the normal test sequence is

```text
./consumer --start
./producer 10
watch -n 1 cat /proc/elevator
./consumer --stop
```

remove modules in reverse order

```text
sudo rmmod elevator
sudo rmmod local_stubs
```

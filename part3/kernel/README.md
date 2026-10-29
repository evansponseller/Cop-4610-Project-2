# part 3 kernel integration

copy `syscalls.c` into the linux source tree under `syscalls/syscalls.c`

copy `Makefile` into the same `syscalls` directory

add `syscalls/` to the top level kernel makefile

add the three entries below to `arch/x86/entry/syscalls/syscall_64.tbl`

```text
548 common start_elevator sys_start_elevator
549 common issue_request sys_issue_request
550 common stop_elevator sys_stop_elevator
```

add the three declarations to `include/linux/syscalls.h`

build the elevator module with `make LOCAL_STUBS=0` after the kernel exports the
three stub pointers

the default module build uses `local_stubs.c` only to validate the elevator
module against a stock kernel header tree

```c
asmlinkage long sys_start_elevator(void);
asmlinkage long sys_issue_request(int start_floor, int destination_floor, int type);
asmlinkage long sys_stop_elevator(void);
```

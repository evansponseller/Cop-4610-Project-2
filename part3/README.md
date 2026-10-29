# part 3

## build

build the elevator module and local pointer exports with

```text
make
```

build the user programs with

```text
cd tests
make
```

the default module build uses `local_stubs.ko` so the elevator can be checked
against the available kernel headers

the integrated build uses

```text
make LOCAL_STUBS=0
```

after the three syscall pointers have been added to the target kernel

## runtime

load the local pointer module followed by the elevator module

```text
sudo insmod local_stubs.ko
sudo insmod elevator.ko
```

the local pointer module only validates module loading and does not add syscall
table entries

with the integrated kernel use the kernel syscall implementation in `kernel`
and load only `elevator.ko`

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

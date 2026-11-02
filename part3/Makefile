obj-m += elevator.o
elevator-objs := src/elevator.o

# set LOCAL_STUBS=0 on the custom kernel, which already exports the stub pointers
LOCAL_STUBS ?= 1

ifeq ($(LOCAL_STUBS),1)
obj-m += local_stubs.o
local_stubs-objs := src/local_stubs.o
endif

ccflags-y := -g -Wno-error

KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean

.PHONY: all clean

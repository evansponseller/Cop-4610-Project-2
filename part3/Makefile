obj-m += elevator.o

LOCAL_STUBS ?= 1

ifeq ($(LOCAL_STUBS),1)
obj-m += local_stubs.o
endif

ccflags-y := -g -Wno-error

KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean

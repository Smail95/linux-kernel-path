#KERNELDIR_PNL ?= /lib/modules/$(shell uname -r)/build
KERNELDIR_PNL = ${HOME}/linux/linux-4.19.27
KERNELDIR5 = ${HOME}/linux/linux-5.0.6
PWD := $(shell pwd)

obj-m += detection-xarray.o

CFLAGS_prdebug.o := -DDEBUG
all:
	echo $(info xxx)
	make -C $(KERNELDIR5) M=$(PWD) modules

test:
	make -C test/

clean:
	make -C $(KERNELDIR5) M=$(PWD) clean


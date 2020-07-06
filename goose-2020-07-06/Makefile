CFLAGS = -std=gnu99

obj-m += goose.o

.PHONY: all
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

.PHONY: clean
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

.PHONY: test
test:
	sudo dmesg -C
	sudo insmod goose.ko
	@echo "You should see a goose..."
	sudo cat /dev/goose
	sudo rmmod goose.ko
	@echo "dmesg:"
	@dmesg


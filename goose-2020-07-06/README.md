# Linux kernel module providing an ascii goose

Module creates /dev/goose file providing an ascii goose.

Use ```make all``` to build and ```make test``` to test the module. Example:
```commandline
ilya@orangepizero:~/rtsoft2020/goose-2020-07-06$ make all
make -C /lib/modules/5.4.45-sunxi/build M=/home/ilya/rtsoft2020/goose-2020-07-06 modules
make[1]: Entering directory '/usr/src/linux-headers-5.4.45-sunxi'
  CC [M]  /home/ilya/rtsoft2020/goose-2020-07-06/goose.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC [M]  /home/ilya/rtsoft2020/goose-2020-07-06/goose.mod.o
  LD [M]  /home/ilya/rtsoft2020/goose-2020-07-06/goose.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.4.45-sunxi'
ilya@orangepizero:~/rtsoft2020/goose-2020-07-06$ make test
sudo dmesg -C
[sudo] password for ilya: 
sudo insmod goose.ko
You should see a goose...
sudo cat /dev/goose
     ▄▀▀▀▄
▄███▀ *   ▌
    ▌     ▐
    ▐     ▐
    ▌     ▐▄▄
    ▌    ▄▀##▀▀▀▀▄
   ▐    ▐########▀▀▄
   ▐    ▐▄##########▀▄
    ▀▄    ▀▄##########▀▄
      ▀▄▄▄▄▄█▄▄▄▄▄▄▄▄▄▄▄▀▄
           ▌▌ ▌▌
           ▌▌ ▌▌
         ▄▄▌▌▄▌▌
sudo rmmod goose.ko
dmesg:
[708627.101605] Goose device initialized
[708627.197163] Goose device unregistered
ilya@orangepizero:~/rtsoft2020/goose-2020-07-06$ make clean
make -C /lib/modules/5.4.45-sunxi/build M=/home/ilya/rtsoft2020/goose-2020-07-06 clean
make[1]: Entering directory '/usr/src/linux-headers-5.4.45-sunxi'
  CLEAN   /home/ilya/rtsoft2020/goose-2020-07-06/Module.symvers
make[1]: Leaving directory '/usr/src/linux-headers-5.4.45-sunxi'
ilya@orangepizero:~/rtsoft2020/goose-2020-07-06$ 
```
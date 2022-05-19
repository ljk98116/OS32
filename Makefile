INCLUDE = libs

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin -fno-stack-protector -fno-pie -no-pie -I $(INCLUDE)
S_FLAGS = -m32 -gdwarf-2 -Wa,-divide
LD_FLAGS = -T tools/kernel.ld -m elf_i386 -nostdlib

C_FILES = src/init\
		src/libs\
		src/bootloader\
		src/console\
		src/test\
		src/debug

S_FILES = src/bootloader

C_SOURCES = $(shell find $(C_FILES) -name "*.c")
S_SOURCES = $(shell find $(S_FILES) -name "*.S")

C_OBJS = $(patsubst src/%.c,obj/%.o,$(C_SOURCES))
S_OBJS = $(patsubst src/%.S,obj/%.o,$(S_SOURCES))

C_DIRS = $(patsubst src%,obj%,$(C_FILES))
S_DIRS = $(patsubst src%,obj%,$(S_FILES))

dirs:
	mkdir -p $(C_DIRS) $(S_DIRS)

obj:dirs
	$(foreach n,$(C_SOURCES),$(shell gcc $(C_FLAGS) $(n) -o obj/$(patsubst src/%.c,%.o,$(n))))
	$(foreach n,$(S_SOURCES),$(shell gcc $(C_FLAGS) $(n) -o obj/$(patsubst src/%.S,%.o,$(n))))

kernel:obj
	ld $(LD_FLAGS) $(C_OBJS) $(S_OBJS) -o $@

update_img:kernel
	sudo losetup /dev/loop29 os.img #将test.img绑定到回环设备29上
	sudo mount /dev/loop29 mnt/ #将回环设备29挂载到对应目录
	sudo cp kernel mnt/kernel
	sudo umount mnt/	#解除挂载
	sudo losetup -d /dev/loop29 #释放回环设备29

.PHONY:operate_img
operate_img:
	sudo losetup /dev/loop29 os.img #将test.img绑定到回环设备29上
	sudo mount /dev/loop29 mnt/ #将回环设备29挂载到对应目录

.PHONY:exit_img
exit_img:
	sudo umount mnt/	#解除挂载
	sudo losetup -d /dev/loop29 #释放回环设备29

.PHONY:all
all:update_img

.PHONY:clean
clean:
	rm -rf bin obj kernel

.PHONY:QEMU
qemu:
	qemu-system-i386 -hda os.img
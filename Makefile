K=kernel
B=build

CROSS = riscv64-unknown-elf-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS = -Wall -O2 -nostdlib -march=rv64g -mabi=lp64 -Iinclude -mcmodel=medany -fno-builtin-printf -fno-builtin-putc
CFLAGS += -fno-common -nostdlib
CFLAGS += -fno-builtin-strncpy -fno-builtin-strncmp -fno-builtin-strlen -fno-builtin-memset
CFLAGS += -fno-builtin-memmove -fno-builtin-memcmp -fno-builtin-log -fno-builtin-bzero
CFLAGS += -fno-builtin-strchr -fno-builtin-exit -fno-builtin-malloc -fno-builtin-putc
CFLAGS += -fno-builtin-free
CFLAGS += -fno-builtin-memcpy -Wno-main
CFLAGS += -fno-builtin-printf -fno-builtin-fprintf -fno-builtin-vprintf

LDFLAGS = -T $K/kernel.ld

OBJS = $(B)/entry.o $(B)/main.o $(B)/uart.o $(B)/console.o $(B)/printf.o

# 默认目标
all: $(B)/kernel.elf $(B)/kernel.bin

# 创建 build 目录
$(B):
	mkdir -p $(B)

# 编译各个目标文件到 build 目录
$(B)/entry.o: $K/entry.S | $(B)
	$(CC) $(CFLAGS) -c $K/entry.S -o $@

$(B)/main.o: $K/main.c | $(B)
	$(CC) $(CFLAGS) -c $K/main.c -o $@

$(B)/uart.o: $K/uart.c | $(B)
	$(CC) $(CFLAGS) -c $K/uart.c -o $@

$(B)/console.o: $K/console.c | $(B)
	$(CC) $(CFLAGS) -c $K/console.c -o $@

$(B)/printf.o: $K/printf.c | $(B)
	$(CC) $(CFLAGS) -c $K/printf.c -o $@

# 链接生成 elf 文件
$(B)/kernel.elf: $(OBJS) $K/kernel.ld | $(B)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# 生成 bin 文件
$(B)/kernel.bin: $(B)/kernel.elf | $(B)
	$(OBJCOPY) -O binary $< $@

# 清理
clean:
	rm -rf $(B)/*.o $(B)/kernel.elf $(B)/kernel.bin

.PHONY: all clean
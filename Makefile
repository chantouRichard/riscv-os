K = kernel
B = build

CROSS = riscv64-unknown-elf-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS = -Wall -O0 -nostdlib -march=rv64g -mabi=lp64 -Iinclude -mcmodel=medany -g
CFLAGS += -fno-builtin-printf -fno-builtin-putc -fno-common -Wno-main
CFLAGS += -fno-builtin-strncpy -fno-builtin-strncmp -fno-builtin-strlen
CFLAGS += -fno-builtin-memset -fno-builtin-memmove -fno-builtin-memcmp
CFLAGS += -fno-builtin-log -fno-builtin-bzero -fno-builtin-strchr
CFLAGS += -fno-builtin-exit -fno-builtin-malloc -fno-builtin-free
CFLAGS += -fno-builtin-fprintf -fno-builtin-vprintf

LDFLAGS = -T $(K)/kernel.ld

# 查找 kernel 下所有 C 文件
SRC = $(shell find $(K) -name "*.c")
# 把对应的 .o 文件放在 build/ 下
OBJS = $(patsubst $(K)/%.c,$(B)/%.o,$(SRC))
OBJS := $(OBJS:.c=.o)

# 查找所有 .S 文件
ASM_SRC = $(shell find $(K) -name "*.S")
ASM_OBJS = $(patsubst $(K)/%.S,$(B)/%.o,$(ASM_SRC))

# 默认目标
all: $(B)/kernel.elf $(B)/kernel.bin

# 创建 build 目录
$(B):
	mkdir -p $(B)

# 规则：编译 C 文件
$(B)/%.o: $(K)/%.c | $(B)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 规则：编译汇编文件
$(B)/%.o: $(K)/%.S | $(B)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接生成 elf
$(B)/kernel.elf: $(OBJS) $(ASM_OBJS) $(K)/kernel.ld | $(B)
	$(LD) $(LDFLAGS) -o $@ $(ASM_OBJS) $(OBJS)

# 生成 bin
$(B)/kernel.bin: $(B)/kernel.elf | $(B)
	$(OBJCOPY) -O binary $< $@

# 清理
clean:
	rm -rf $(B)

.PHONY: all clean

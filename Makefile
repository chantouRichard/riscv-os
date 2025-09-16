K=kernel
U=user
B=build

CROSS = riscv64-unknown-elf-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS = -Wall -O2 -nostdlib -march=rv64g -mabi=lp64 -Iinclude -mcmodel=medany
LDFLAGS = -T $K/kernel.ld

OBJS = entry.o main.o uart.o

all: kernel.elf kernel.bin

entry.o: $K/entry.S
	$(CC) $(CFLAGS) -c $K/entry.S

main.o: $K/main.c
	$(CC) $(CFLAGS) -c $K/main.c

uart.o: $K/uart.c
	$(CC) $(CFLAGS) -c $K/uart.c

kernel.elf: $(OBJS) $K/kernel.ld
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJS)

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

clean:
	rm -f *.o kernel.elf kernel.bin
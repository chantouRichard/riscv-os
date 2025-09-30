#!/bin/bash
set -e  # 遇到错误立即退出

# Step 1: 清理
echo "[*] Cleaning build..."
make clean

# Step 2: 编译
echo "[*] Building kernel..."
make

# Step 3: 运行 QEMU
echo "[*] 开始运行我的操作系统..."
echo "    (按 Ctrl+A X 退出)"
echo ""
qemu-system-riscv64 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel build/kernel.bin

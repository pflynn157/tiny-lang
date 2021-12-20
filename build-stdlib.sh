#!/bin/bash

CFLAGS="-nostdlib -c -Wno-builtin-declaration-mismatch"
CORELIB="lib/base/corelib"
STDLIB="lib/base/stdlib"

echo "Building core library..."

as $CORELIB/syscall_x86.asm -o build/syscall_x86.o
cc $CORELIB/sys.c -o build/sys.o $CFLAGS
cc $CORELIB/io.c -o build/io.o $CFLAGS
cc $CORELIB/str.c -o build/str.o $CFLAGS

ar rcs build/liborka_corelib.a \
    build/syscall_x86.o \
    build/sys.o \
    build/io.o \
    build/str.o

as $CORELIB/x64_start.asm -o build/occ_start.o

echo "Building standard library..."

mkdir -p build/stdlib

cc $STDLIB/io.c -o build/stdlib/io.o $CFLAGS -fPIC

ld \
    -o build/stdlib/liborka.so \
    -shared -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
    -Lbuild -lorka_corelib \
    build/stdlib/io.o

echo "Done"


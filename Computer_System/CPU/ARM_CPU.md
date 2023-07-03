## ARM CPU

The ARM architecture is a Reduced Instruction Set Computer (RISC) architecture.

A load/store architecture, where data-processing operations only operate on register contents, not directly on memory contents.

ARM cores support both big and little endian modes, but are most commonly used in, and typically default to "little-endian" mode.

The ARMv8 architecture supports: 

+ A 64-bit Execution state, AArch64.

+ A 32-bit Execution state, AArch32, that is compatible with previous versions of the ARM architecture.

AArch64 is the 64-bit Execution state, meaning addresses are held in 64-bit registers, 

and instructions in the base instruction set can use 64-bit registers for their processing. 

AArch64 state supports the A64 instruction set.

AArch32 is the 32-bit Execution state, meaning addresses are held in 32-bit registers, 

and instructions in the base instruction sets use 32-bit registers for their processing.

AArch32 state supports the T32 and A32 instruction sets.

AArch64 (the 64-bit Execution state) provides: 

+ 64-bit general-purpose registers

+ 64-bit program counter (PC), stack pointers (SPs), and exception link registers (ELRs)

+ 128-bit registers for SIMD vector and scalar floating-point support

In ARMv8 the possible instruction sets depend on the Execution state. 

AArch64 state supports only a single instruction set, called A64. 

This is a fixed-length instruction set that uses 32-bit instruction encodings.

AArch32 state supports the following instruction sets: 

+ A32 : This is a fixed-length instruction set that uses 32-bit instruction encodings.

+ T32 : This is a variable-length instruction set that uses both 16-bit and 32-bit instruction encodings.


## ARM v8 Architecture Data Types

* Supported Data Types :

   + Byte (1 byte =  8 bits)

   + Halfword (2 bytes = 16 bits)

   + Word (4 bytes = 32 bits)

   + Doubleword (8 bytes = 64 bits)

   + Quadword (16 bytes = 128 bits)

* Floating-point Data Types :

   + Half precision floating-point: 2 bytes

   + BFloat16: 2 bytes

   + Single precision floating-point: 4 bytes

   + Double precision floating-point: 8 bytes


## References:

[ARM Architecture Reference Manual](https://developer.arm.com/documentation/)

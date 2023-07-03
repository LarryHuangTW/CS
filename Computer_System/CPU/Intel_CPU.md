## Intel CPU

**IA-32**: Intel Architecture 32-bit (also called i386) is the 32-bit version of the x86 instruction set architecture which designed by Intel. 

IA-32 architecture is the instruction set architecture and programming environment for Intel's 32-bit microprocessors.

**IA-64**: Intel (Itanium) Architecture 64-bit is the instruction set architecture (ISA) of the Itanium family of 64-bit Intel microprocessors.

**x86**: a family of instruction set architectures of variable instruction length, primarily CISC design adopted by Intel and AMD. 

**x86-64**: the 64-bit version of the x86 instruction set (also known as x64, AMD64 and Intel 64).

**Intel 64**: Intel 64 architecture is the instruction set architecture and programming environment which is the superset of Intel's 32-bit and 64-bit architectures. It is compatible with the IA-32 architecture.


Intel 64 and IA-32 processors are "little endian" machines; this means the bytes of a word are numbered starting from the least significant byte.

machine instruction: instruction opcode + operands (source operand , destination operand)

number of operands: 0 ~ 3

When two operands are present in an arithmetic or logical instruction, the right operand is the source and the left operand is the destination.

L1 cache: instruction cache (I-Cache) and data cache (D-Cache)

L2 cache: static RAM

A 32-bit address bus that supports up to 4-GBytes of physical memory

Bus Unit, translation engine (instruction fetch unit, decode units / decoders), Arithmetic Logic Units (ALUs), Float-Point Unit (FPU), Out-Of-Order Execution Core, Retirement Unit ...

In 64-bit mode:

   + Effective address size: 32, 64 (default)

   + Effective operand size: 16, 32 (default), 64

RAX , EAX , AX , AL + AH

## Intel CPU Data Types

* Fundamental Data Types:

   + Byte (1 byte = 8 bits)

   + Word (2 bytes = 16 bits)

   + Doubleword (4 bytes = 32 bits)

   + Quadword (8 bytes = 64 bits)

   + Double Quadword (16 bytes = 128 bits)

   + Words, doublewords, and quadwords do not need to be aligned in memory on natural boundaries.

> The natural boundaries for words, double words, and quadwords are even-numbered addresses, 
> addresses evenly divisible by four, and addresses evenly divisible by eight, respectively.
> However, to improve the performance of programs, data structures should be aligned on natural boundaries whenever possible.
> The reason for this is that the processor requires two memory accesses (memory bus cycle) to make an unaligned memory access; 
> aligned accesses require only one memory access.

* Numeric Data Types:

   + some instructions support additional interpretations of fundamental data types to allow operations to be performed on numeric data types.

   + Integers:

      - 2 (Unsigned , Signed) x 4 (Byte , Word , Doubleword , Quadword) = 8 types of integers

      - Unsigned Integers: Ordinals

      - Signed Integers: 2's complement encoding with the sign bit is located in the highest bit

   + Floating-Point:

      - Half precision floating-point: 2 bytes

      - Single precision floating-point: 4 bytes

      - Double precision floating-point: 8 bytes

      - Double Extended precision floating-point: 10 bytes

      - The data formats correspond directly to IEEE Standard 754 for binary floating-point arithmetic.

      - are operated on by x87 FPU

* Pointer Data Types:

   + non-64-bit modes:

      - Near Pointer: is a 32-bit offset (also called an effective address) within a segment (register)

      - Far  Pointer: is a logical address, consisting of a 16-bit segment selector and a 32-bit offset

   + 64-bit mode:

      - Near Pointer: 64 bits

      - Far  Pointer: is a 16-bit segment selector and a 64-bit offset

* Bit Field Data Type: It is a contiguous sequence of bits. It can begin at any bit position of any byte in memory and can contain up to 32 bits

* String Data Types:

   + are continuous sequence of bits, bytes, words, or doublewords.

   + a bit  string can contain up to 4 G bits

   + a byte string can contain up to 4 GBytes

* Packed SIMD Data Types:

   + SIMD : Single Instruction Multiple Data

   + a set of 64-bit and 128-bit packed data type for use in SIMD operations

* BCD Integers: Binary-coded decimal integers (BCD integers) are unsigned 4-bit integers with valid values ranging from 0 to 9.

[^1]: [Intel 64 and IA-32 Architectures Software Developer's Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

[^2]: Align and Organize Data for Better Performance (2011)

[^3]: [Data Alignment to Assist Vectorization](https://software.intel.com/content/www/us/en/develop/articles/data-alignment-to-assist-vectorization.html)

[^4]: [Modern CPU Architecture Part 1 – Key Concepts](https://www.youtube.com/watch?v=vgPFzblBh7w)

[^5]: [Modern CPU Architecture Part 2 – Microarchitecture Deep Dive](https://www.youtube.com/watch?v=o_WXTRS2qTY)

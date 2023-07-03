## AMD CPU

The x86 and AMD64 architectures address memory using "little-endian" byte-ordering. 

Multibyte values are stored with their least-significant byte at the lowest byte address.

Instructions and data are stored in memory in "little-endian" byte order. 

Little-endian ordering places the least-significant byte of the instruction or data item at the lowest memory address and the most-significant byte at the highest memory address.

* In long mode:

   + Address size : 16, 32, 64 (default)

   + Operand size : 16, 32 (default), 64

Application software can override the default operand size by using an operand-size instruction prefix.

The increased width of general-purpose registers from 32 bits to 64 bits allows compilers to substantially improve software performance.

Compilers have more flexibility in using registers to hold variables.

Compilers can also minimize memory traffic and thus boost performance by localizing work within the GPRs.

Virtual memory consists of the entire address space available to programs. 

It is a large linear-address space that is translated by a combination of hardware and operating-system software to a smaller physical-address space, 

parts of which are located in memory and parts on disk or other external storage media.

Physical memory is the installed memory (excluding cache memory) in a particular computer system that can be accessed through the processor's bus interface. 

The maximum size of the physical memory space is determined by the number of address bits on the bus interface.

Memory management is not visible to application programs. 

It is handled by the operating system and processor hardware using segmentation and/or paging.

Instructions are byte data types. They are read from memory one byte at a time, starting with the least-significant byte (lowest address).

In memory, the REX prefix byte would be stored at the lowest address, and the first immediate byte of operand would be stored at the highest instruction address.

Pointers are variables that contain addresses rather than data. 

They are used by instructions to reference memory. Instructions access data using near and far pointers. 

Stack pointers locate the current stack.

Near pointers contain only an effective address, which is used as an offset into the current segment. 

Far  pointers contain both an effective address and a segment selector that specifies one of several segments.

In 64-bit mode, the AMD64 architecture supports only the flat-memory model in which there is only one data segment, 

so the effective address is used as the virtual (linear) address and far pointers are not needed.

In most instructions that take two operands, the first (left-most) operand is both a source operand and the destination operand. 

The second (right-most) operand serves only as a source.

Instructions can have one or more prefixes that modify default instruction functions or operand properties.


## Data Alignment

A data access is aligned if its address is a multiple of its operand size, in bytes.

1. Byte accesses are always aligned. Bytes are the smallest addressable parts of memory.

2. Word (two-byte) accesses are aligned if their address is a multiple of 2.

3. Doubleword (four-byte) accesses are aligned if their address is a multiple of 4.

4. Quadword  (eight-byte) accesses are aligned if their address is a multiple of 8.

The AMD64 architecture does not impose data-alignment requirements for accessing data in memory.

However, depending on the location of the misaligned operand with respect to the width of the data bus and other aspects of the hardware implementation 

(such as store-to-load forwarding mechanisms), a misaligned memory access can require more bus cycles than an aligned access. 

For maximum performance, avoid misaligned memory accesses.


## AMD CPU Data Types

* Fundamental Data Types:

   + bit (1 bit)

   + nibble (4 bit)

   + Byte (1 byte = 8 bits)

   + Word (2 bytes = 16 bits)

   + Doubleword (4 bytes = 32 bits)

   + Quadword (8 bytes = 64 bits)

   + Double Quadword / Octword (16 bytes = 128 bits)

   + Double Octword (32 bytes = 256 bits)

* Integers:

   + 2 (Unsigned, Signed) x 5 (Byte, Word, Doubleword, Quadword, Octword) = 10 types of integers

   + Signed Integers are represented in 2's complement format and the sign bit occupies the most significant bit

* Floating-point Data Types:

   + Half precision floating-point: 2 bytes

   + Single precision floating-point: 4 bytes

   + Double precision floating-point: 8 bytes

* Binary coded decimal (BCD) digits

* Packed BCD

* Strings:

   + continuous sequence of bits, bytes, words, doublewords, or quadwords.

   + Character Strings (composed of bytes or words)

   + Doubleword and Quadword

* Packed Integers / Integer Vectors

* Packed Single / Double precision floating-point / Floating-point Vectors


## References:

1. [AMD64 Architecture Programmer's Manual](https://www.amd.com/en/support/tech-docs/amd64-architecture-programmers-manual-volumes-1-5)

2. [Microsoft: Alignment](https://docs.microsoft.com/en-us/cpp/cpp/alignment-cpp-declarations?view=vs-2019)

3. [WiKi: Data structure alignment](https://en.wikipedia.org/wiki/Data_structure_alignment)

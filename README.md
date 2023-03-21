# RISC-V ISA Simulator 

By Adithya Rajesh | Sanket Patil | Viraj Pashte | Zoheb Mohammed Anis Mir

####################################################################################################################################################################

RISC-V is an open-source, modular Instruction Set Architecture (ISA) that is gaining significant attention in the computing industry. It is designed to be simple, elegant, and extensible, allowing for customization and specialization for different applications. The RISC-V 32I base integer instruction set architecture is the most fundamental version of the RISC-V ISA, providing a basic set of instructions for implementing a general-purpose computer. This instruction set includes 32 registers, basic arithmetic, logical, and memory instructions, and control transfer instructions. The simplicity and flexibility of the RISC-V 32I make it an ideal choice for embedded systems, microcontrollers, and other low-power devices.

In addition to the base integer instruction set architecture (32I), RISC-V also includes M and F type instructions. The M extension adds integer multiplication and division operations, while the F extension adds support for single-precision floating-point operations. These extensions are optional and can be added to the base integer instruction set as needed for specific applications. The M and F type instructions expand the capabilities of RISC-V, making it suitable for a wider range of applications, including scientific computing, graphics processing, and signal processing. The modularity of the RISC-V ISA allows for custom instruction sets to be designed and added, which can provide significant performance gains for specific tasks.

####################################################################################################################################################################

--------------------------------------------------------------------------------------------------------------------------------------------------------------------

Modes:

Normal Mode: 

It is the PC and hexadecimal value of each instruction as it's fetched along with the contents (in hexadecimal) of each register after the instruction's execution.

Silent Mode:

It prints the PC of the final instruction and hexadecimal value of each register by the end of the simulation.

Debug Mode:

Provides the ability to single step and print contents of register, memory, display current instruction and move forward one by one. 

------------------------------------------------------------------------------------------------------------------------------------------------------------------

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Execution commands:

rvgcc -S -fpic -march=rv32g -mabi=ilp32 prog.c 
(here we use the rv32g to indicate general. This will run the M and F instructions and compile the assembly using correct instructions. If you want to run only the I instructions, use rv32i )

rvas -ahld prog.s -o prog.o

rvobjdump -d prog.o | grep -o ^[[:blank:]]*[[:xdigit:]]*:[[:blank:]][[:xdigit:]]*' > prog.mem 

g++ riscv_simul.cpp -o riscv_simul

./riscv_simul <input file> <program counter> <stack address> <mode> <step enable>

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

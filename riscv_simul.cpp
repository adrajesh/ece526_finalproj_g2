#include <bits/stdc++.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include <string.h>
#include <cctype>
#include <cstring>
#include <csignal>
#include "immediate_field.cpp"
#include <cmath>

using namespace std;

#define PC_LOC 0
#define STACK_ADDR 65536
#define I_FILENAME "program.mem"

// Global vars
bool DebugMode;
bool NormalMode;
bool SingleStep;
int pc; 				// program counter
int sp;					// stack address
int file;
uint8_t memory_array[65536] = {};		// Memory in bytes
uint32_t x[32] = {}; 					// r0 is zero
float f[32] = {};						//floating point registers

int i;

// Initialize regs

uint32_t mem_acc(int mem_p, int b, int sign) {
	uint32_t mem_cont;
	if (b == 4) {
		mem_cont = memory_array[mem_p] | memory_array[mem_p + 1] << 8 | memory_array[mem_p + 2] << 16 | memory_array[mem_p + 3] << 24;
	}
	else if (b == 2) {
		mem_cont = memory_array[mem_p] | memory_array[mem_p + 1] << 8;
		if (sign) {
			if (mem_cont >> 15)
				mem_cont = mem_cont | 0xFFFF0000;
		}
	}
	else {
		mem_cont = memory_array[mem_p];
		if (sign) {
			if (mem_cont >> 7)
				mem_cont = mem_cont | 0xFFFFFF00;
		}
	}
	return mem_cont;
}

uint32_t mem_wr(int mem_p, int b, uint32_t mem_cont) {
	if (b == 4) {
		memory_array[mem_p] = mem_cont & 0xFF;
		memory_array[mem_p + 1] = (mem_cont >> 8) & 0xFF;
		memory_array[mem_p + 2] = (mem_cont >> 16) & 0xFF;
		memory_array[mem_p + 3] = (mem_cont >> 24) & 0xFF;
	}
	else if (b == 2) {
		memory_array[mem_p] = mem_cont & 0xFF;
		memory_array[mem_p + 1] = (mem_cont >> 8) & 0xFF;
	}
	else {
		memory_array[mem_p] = mem_cont & 0xFF;
	}
	return mem_cont;
}

// Prints the usage of the program.
void usage()
{
	std::cerr << endl << "Usage: riscv_simul <input file> <program counter> <stack address> <mode> <step enable>" <<endl<<endl;
	std::cerr << "input file \t\t- default is program.mem. Provide filename to read a specific file" << endl;
	std::cerr << "program counter \t- default is 0" << endl;
	std::cerr << "stck address \t\t- default is 65536" << endl;
	std::cerr << "mode \t\t\t- default is silent mode" << endl;
	std::cerr << "\t\t\t  pass 'normal' to enable normal mode " << endl;							
	std::cerr << "\t\t\t  pass 'debug' to enable debug mode"<< endl;
	std::cerr << "step execution enable \t- default is diabled. '1' to enable"<<endl<<endl;
	std::cerr << "Example - \'riscv_simul abcd.mem 4 65536 debug 1\'" << endl;
	exit(1);
}

// To check if file exists and whether to proceed or not.
void check_file(int check) {
	if (check) {
		cout << "File exists" << endl;
	}
	else {
		cout << "File does not exist. Please check the input mem file." << endl;
		usage();
	}
}

// Parse for trace file.
void parse_line(std::string access, unsigned int arr[]) {
	// What we want to parse
	unsigned int instruction;
	unsigned int trace_pc;
	// Parse from the string we read from the file
	sscanf(access.c_str(), "%x: %x", &trace_pc, &instruction);
	arr[0] = trace_pc;
	arr[1] = instruction;
}

void print_regs() {
	// Print all contents of 32 register
	cout << endl;
	int count = 0;
	for (int i = 0; i < 32; i++) {
		if (x[i]==0)
			cout << "x[" << std::dec << i << "]" << " - " << std::hex << std::setw(8) << setfill(' ') << x[i] << " \t ";
		else 
			cout << "x[" << std::dec << i << "]" << " - " << std::hex << std::setw(8) << setfill('0') << x[i] << " \t ";
		count++;
		if (count % 8 == 0) {
			cout << endl;
		}
	}
	cout << endl;
}

void print_floatregs() {
	// Print all contents of 32 floating register
	cout << endl;
	int count = 0;
	for (int i = 0; i < 32; i++) {
		if (x[i]==0)
			cout << "f[" << std::dec << i << "]" << " - " << std::hex << std::setw(8) << setfill(' ') << f[i] << " \t ";
		else
			cout << "f[" << std::dec << i << "]" << " - " << std::hex << std::setw(8) << setfill('0') << f[i] << " \t ";
		count++;
		if (count % 8 == 0) {
			cout << endl;
		}
	}
	cout << endl;
}

void print_mem() {
	std::ofstream outfile("memory_array.txt");
// For printing the contents of memory into a file - memory_array.txt
	for (int i = 65535; i >= 0; i--) {
		outfile << std::hex << i << " : " << std::setw(2) << setfill('0') << static_cast<int>(memory_array[i]) << endl;
	}
	outfile.close();
}

int main(int argc, char* argv[]) {
	std::string line;
	std::ifstream infile;
//	std::ofstream outfile("memory_array.txt");
	unsigned int trace[2];
	uint32_t instr, curr_instr;
	uint32_t file_pc;
	uint32_t opcode, funct3, funct7, funct2, rd, rs1, rs2, rs3;
	int32_t II, SI, BI, UI, JI, FI; 						// Immediate fields 
	bool MSBimmediate;
	uint32_t memory_loc;
	int bytes;
	uint32_t f_byte[4];
	int program_space;
	int t;
	uint64_t md;

	switch (argc) {
	case 1: infile.open(I_FILENAME);			// No Arguments provided. Read program.mem, pc 0, sa 65535, verbose disabled
		check_file(infile.is_open());
		pc = PC_LOC;
		sp = STACK_ADDR;
		DebugMode = 0;
		NormalMode = 0;
		SingleStep = 0;
		break;
	case 2: infile.open(argv[1]);				// 1 Arguments provided. Read xyz.mem, pc 0, sa 65535, verbose disabled
		check_file(infile.is_open());
		pc = PC_LOC;
		sp = STACK_ADDR;
		DebugMode = 0;
		NormalMode = 0;
		SingleStep = 0;
		break;
	case 3: infile.open(argv[1]);				// 2 Arguments provided. Read xyz.mem, pc x, sa 65535, verbose disabled
		check_file(infile.is_open());
		sscanf(argv[2], "%x", &pc);
		//pc = (int) atoi(argv[2]);			// To change from char* to int
		sp = STACK_ADDR;
		DebugMode = 0;
		NormalMode = 0;
		SingleStep = 0;
		break;
	case 4: infile.open(argv[1]);				// 3 Arguments provided. Read xyz.mem, pc x, sa y, verbose disabled
		check_file(infile.is_open());
		//pc = (int) atoi(argv[2]);
		sscanf(argv[2], "%x", &pc);
		sp = (int)atoi(argv[3]);
		DebugMode = 0;
		NormalMode = 0;
		SingleStep = 0;
		break;
	case 5: infile.open(argv[1]);				// 4 Arguments provided. Read xyz.mem, pc x, sa y, mode
		check_file(infile.is_open());
		//pc = (int) atoi(argv[2]);
		sscanf(argv[2], "%x", &pc);
		sp = (int)atoi(argv[3]);
		SingleStep = 0;
		if (strcmp(argv[4], "debug") == 0) {
			cout << "Debug Mode enabled" << endl;
			DebugMode = 1;
		}
		else if (strcmp(argv[4], "normal") == 0) {
			cout << "Normal Mode enabled" << endl;
			NormalMode = 1;
		}
		else {
			cout << "Incorrect mode specified" << endl;
			usage();
		}
		break;
	case 6: infile.open(argv[1]);				// 5 Arguments provided. Read xyz.mem, pc x, sa y, mode, single step
		check_file(infile.is_open());
		//pc = (int) atoi(argv[2]);
		sscanf(argv[2], "%x", &pc);
		sp = (int)atoi(argv[3]);
		if (strcmp(argv[4], "debug") == 0) {
			cout << "Debug Mode enabled" << endl;
			DebugMode = 1;
		}
		else if (strcmp(argv[4], "normal") == 0) {
			cout << "Normal Mode enabled" << endl;
			NormalMode = 1;
		}
		else {
			cout << "Incorrect mode specified" << endl;
			usage();
		}
		if ((bool)atoi(argv[5])) {
			cout << "Single step execution enabled" << endl;
			SingleStep = 1;
		}	
		break;
	default: usage();
	}
	cout << endl << "******************WELCOME TO RISC-V SIMULATOR******************" << endl;
	if(DebugMode) {
	cout << "Program Counter - " << std::hex << pc << endl;
	cout << "Stack Address - " << sp << endl;
	cout << "Debug mode - " << DebugMode << endl;
	cout << "Normal mode - " << NormalMode << endl;
	cout << "Step execution mode - " << SingleStep << endl;
	}
	x[2] = sp;
	
	// Main Program begins here!

	while (std::getline(infile, line)) {
		parse_line(line, trace);
		file_pc = trace[0];
		instr = trace[1];
		memory_array[file_pc] = instr & 0xFF;
		memory_array[file_pc + 1] = (instr >> 8) & 0xFF;
		memory_array[file_pc + 2] = (instr >> 16) & 0xFF;
		memory_array[file_pc + 3] = (instr >> 24) & 0xFF;
		
		if(DebugMode){
		cout << endl << "pc: " << std::hex << std::uppercase << file_pc << " instr: " << std::hex << instr;
		}
		program_space = file_pc + 4;
	}
	infile.close();

	if(DebugMode) {
	cout << endl << "End of file reading and program saved to memory" << endl;									// Comment later
	}
	if(SingleStep) 
		print_mem();

	while (pc < program_space)
	{
		curr_instr = memory_array[pc] | (memory_array[pc + 1] << 8) | (memory_array[pc + 2] << 16) | (memory_array[pc + 3] << 24);
		cout << "pc: " << std::hex << std::uppercase << pc << " instr: " << std::hex << curr_instr << endl;
		if (curr_instr == 0x00000000) {								// Check for all 0 instr
			cout << "Ending simulation !!! (all 0 instr)";
			break;
		}
		if ((curr_instr == 0x00008067) && (x[1] == 0)) {			// Check for jr ra where ra is 0
			cout << "Ending simulation !!! (jr ra 0)";
			break;
		}
		// Store everything regardless of instr type 
		opcode = curr_instr & (0x7F);								// bits [6:0]
		funct3 = (curr_instr & (0x7000)) >> 12;						// bits [14:12]
		funct7 = (curr_instr & (0xFE000000)) >> 25;					// bits [31:25]
		rd = (curr_instr & (0xF80)) >> 7;							// bits [11:7]
		rs1 = (curr_instr & (0xF8000)) >> 15;						// bits [19:15]
		rs2 = (curr_instr & (0x1F00000)) >> 20;						// bits [24:20]
		funct2 = (curr_instr & (0x06000000)) >> 25;                                        // bits [26:25]
		rs3 = (curr_instr & (0xF8000000)) >> 27;
		// We have to calculate immediate fields also - refer page 16/17

		x[0] = 0;
		// Enable this in debug mode
		if(DebugMode) { 
		cout << std::uppercase << std::hex << "Opcode: " << opcode << " funct3: " << funct3 << " funct7: " << funct7 << endl;
		cout << std::uppercase << std::hex << "rd: " << rd << " rs1: " << rs1 << " rs2: " << rs2 << endl;
		}
		
		switch (opcode) {
		case 0x33: if(DebugMode) cout << "R-type Instruction" << endl;
			switch (funct7) {
			case 0x00:
				switch (funct3) {
				case 0x00: 	if(DebugMode | NormalMode) cout << "ADD detected" << endl;			// ADD detected (R-type)
					x[rd] = x[rs1] + x[rs2];
					pc = pc + 4;
					break;

				case 0x01: 	if(DebugMode | NormalMode) cout << "SLL detected" << endl;			// SLL detected (R-type)
					x[rd] = (uint32_t(x[rs1]) << (x[rs2] & 0x1F));
					pc = pc + 4;
					break;

				case 0x02:	if(DebugMode | NormalMode) cout << "SLT detected" << endl;			// SLT detected (R-type)
					if (int32_t(x[rs1]) < int32_t(x[rs2]))
						x[rd] = 1;
					else
						x[rd] = 0;
					pc = pc + 4;
					break;

				case 0x03: if(DebugMode | NormalMode) cout << "SLTU detected" << endl;		// SLTU detected (R-type)
					if ((uint32_t(x[rs1])) < (uint32_t(x[rs2])))
						x[rd] = 1;
					else
						x[rd] = 0;
					pc = pc + 4;
					break;

				case 0x04: if(DebugMode | NormalMode) cout << "XOR detected" << endl;			// XOR detected (R-type)
					x[rd] = (x[rs1] ^ x[rs2]);
					pc = pc + 4;
					break;

				case 0x05: if(DebugMode | NormalMode) cout << "SRL detected" << endl;			// SRL detected (R-type)
					x[rd] = ((uint32_t(x[rs1])) >> (x[rs2] & 0x1F));
					pc = pc + 4;
					break;

				case 0x06: if(DebugMode | NormalMode) cout << "OR detected" << endl;			// OR detected (R-type)
					x[rd] = (x[rs1] | x[rs2]);
					pc = pc + 4;
					break;

				case 0x07: if(DebugMode | NormalMode) cout << "AND detected" << endl;			// AND detected (R-type)
					x[rd] = (x[rs1] & x[rs2]);
					pc = pc + 4;
					break;
				}
				break;

				//RISC-V 32I- M Standard Extension for Integer Multiplication and Division (Extra Credit)
			case 0x01:
				switch (funct3) {
				case 0x00: if(DebugMode | NormalMode) cout << "MUL detected" << endl; 						//MUL detected (R-type RV32M)
					x[rd] = (int32_t(x[rs1])) * (int32_t(x[rs2]));
					break;

				case 0x01: if(DebugMode | NormalMode) cout << "MULH detected" << endl; 						//MULH detected (R-type RV32M)
					md = ((int32_t(x[rs1])) * (int32_t(x[rs2])));
					x[rd] = md >> 32;
					break;

				case 0x02: if(DebugMode | NormalMode) cout << "MULHSU detected" << endl; 						//MULHSU detected
					md = ((int32_t(x[rs1])) * (uint32_t(x[rs2])));
					x[rd] = md >> 32;
					break;

				case 0x03: if(DebugMode | NormalMode) cout << "MULHU detected" << endl; 						//MULHU detected
					md = ((uint32_t(x[rs1])) * (uint32_t(x[rs2])));
					x[rd] = md >> 32;

					break;

				case 0x04: if(DebugMode | NormalMode) cout << "DIV detected" << endl;  						//DIV detected
					x[rd] = (x[rs1]) / (int32_t(x[rs1]));
					break;

				case 0x05: if(DebugMode | NormalMode) cout << "DIVU detected" << endl; 						//DIVU detected
					x[rd] = (x[rs1]) / (uint32_t(x[rs1]));
					break;

				case 0x06: if(DebugMode | NormalMode) cout << "REM detected" << endl;						//REM detected
					x[rd] = (x[rs1]) % (x[rs2]);
					break;

				case 0x07: if(DebugMode | NormalMode) cout << "REMU detected" << endl;						//REMU detected
					x[rd] = (uint32_t(x[rs1])) % (uint32_t(x[rs2]));
					break;
				}
				break;


				//RISC-V 32I- M Standard Extension for Integer Multiplication and Division (Extra Credit)
			case 0x20:
				switch (funct3) {
				case 0x00: if(DebugMode | NormalMode) cout << "SUB detected" << endl;			// SUB detected (R-type)
					x[rd] = x[rs1] - x[rs2];
					pc = pc + 4;
					break;

				case 0x05: if(DebugMode | NormalMode) cout << "SRA detected" << endl;			// SRA detected (R-type)
					x[rd] = (int32_t(x[rs1]) >> (x[rs2] & 0x1F));
					pc = pc + 4;
					break;
				}
				break;
			}
			break;

		case 0x03: if(DebugMode) cout << "I-type Instruction" << endl;
			// LB, LH, LW, LBU, LHU
			II = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00: if(DebugMode | NormalMode) cout << "LB detected" << endl;				// LB detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 1, 1));
				pc = pc + 4;
				break;

			case 0x01: if(DebugMode | NormalMode) cout << "LH detected" << endl;				// LH detected (I-type)
				if ((II + x[rs1]) % 2 == 0) {
					x[rd] = (mem_acc((II + x[rs1]), 2, 1));
					pc = pc + 4;
				}
				else {
					cout << "Unaligned mem reference" << endl;
					x[1] = 0;
					pc = program_space - 4;
				}
				break;

			case 0x02: if(DebugMode | NormalMode) cout << "LW detected" << endl;				// LW detected (I-type)
				if ((II + x[rs1]) % 4 == 0) {
					x[rd] = (mem_acc((II + x[rs1]), 4, 1));
					pc = pc + 4;
				}
				else {
					cout << "Unaligned mem reference" << endl;
					x[1] = 0;
					pc = program_space - 4;
				}
				break;

			case 0x04: if(DebugMode | NormalMode) cout << "LBU detected" << endl;				// LBU detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 1, 0));
				pc = pc + 4;
				break;

			case 0x05: if(DebugMode | NormalMode) cout << "LHU detected" << endl;				// LHU detected (I-type)
				if ((II + x[rs1]) % 2 == 0) {
					x[rd] = (mem_acc((II + x[rs1]), 2, 0));
					pc = pc + 4;
				}
				else {
					cout << "Unaligned mem reference" << endl;
					x[1] = 0;
					pc = program_space - 4;
				}
				break;
			}
			break;

		case 0x13: if(DebugMode) cout << "I-type Instruction" << endl;
			// ADDI, SLTI, SLTIU, XORI, ORI, ANDI 
			II = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00: if(DebugMode | NormalMode) cout << "ADDI detected" << endl;			// ADDI detected (I-type)
				x[rd] = x[rs1] + II;
				pc = pc + 4;
				break;

			case 0x02: if(DebugMode | NormalMode) cout << "SLTI detected" << endl;				// SLTI detected (I-type)
				if (int32_t(x[rs1]) < II)
					x[rd] = 1;
				else
					x[rd] = 0;
				pc = pc + 4;
				break;

			case 0x03: if(DebugMode | NormalMode) cout << "SLTIU detected" << endl;				// SLTIU detected (I-type)
				if (uint32_t(x[rs1]) < uint32_t(II))
					x[rd] = 1;
				else
					x[rd] = 0;
				pc = pc + 4;
				break;

			case 0x04: if(DebugMode | NormalMode) cout << "XORI detected" << endl;				// XORI detected (I-type)
				x[rd] = x[rs1] ^ II;
				pc = pc + 4;
				break;

			case 0x06: if(DebugMode | NormalMode) cout << "ORI detected" << endl;				// ORI detected (I-type)
				x[rd] = x[rs1] | II;
				pc = pc + 4;
				break;

			case 0x07: if(DebugMode | NormalMode) cout << "ANDI detected" << endl;				// ANDI detected (I-type)
				x[rd] = x[rs1] & II;
				pc = pc + 4;
				break;

			case 0x01: if(DebugMode | NormalMode) cout << "SLLI detected" << endl;             		//SLLI detected (I-type)
				x[rd] = x[rs1] << rs2;
				pc = pc + 4;
				break;

			case 0x05:
				switch (funct7) {
				case 0x00: if(DebugMode | NormalMode) cout << "SRLI detected" << endl;         		//SRLI detected (I-type)
					x[rd] = uint32_t(x[rs1]) >> rs2;
					pc = pc + 4;
					break;

				case 0x20: if(DebugMode | NormalMode) cout << "SRAI detected" << endl;         		//SRLI detected (I-type)
					x[rd] = int32_t(x[rs1]) >> rs2;
					pc = pc + 4;
					break;
				}
			}
			break;

		case 0x67: if(DebugMode) cout << "I-type Instruction" << endl;
			// only JALR!
			II = immediate(opcode, curr_instr);
			cout << "JALR detected" << endl;					// JALR detected (I-type)
			x[rd] = pc + 4;
			pc = ((x[rs1] + II) & 0xFFFFFFFE);
			break;

		case 0x23: if(DebugMode) cout << "S-type Instruction" << endl;
			// SB,SH,SW
			SI = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00: if(DebugMode | NormalMode) cout << "SB detected" << endl;				// SB detected (S-type)
				mem_wr((SI + x[rs1]), 1, x[rs2]);
				pc = pc + 4;
				if(SingleStep) {
					cout<< "Memory array updated"<<endl;
					print_mem();
				}
				break;

			case 0x01: if(DebugMode | NormalMode) cout << "SH detected" << endl;				// SH detected (S-type)
				// Operation here
				mem_wr((SI + x[rs1]), 2, x[rs2]);
				pc = pc + 4;
				if(SingleStep) {
					print_mem();
					cout<< "Memory array updated"<<endl;
				}
				break;

			case 0x02: if(DebugMode | NormalMode) cout << "SW detected" << endl;				// SW detected (S-type)
				// Operation here
				mem_wr((SI + x[rs1]), 4, x[rs2]);
				pc = pc + 4;
				if(SingleStep) {
					print_mem();
					cout<< "Memory array updated"<<endl;
				}
				break;
			}
			break;

		case 0x63: if(DebugMode) cout << "B-type Instruction" << endl;
			// BEQ, BNE, BLT, BGE, BLTU, BGEU
			BI = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00: if(DebugMode | NormalMode) cout << "BEQ detected" << endl;				// BEQ detected (B-type)
				if (x[rs1] == x[rs2]){
					if(DebugMode) cout << "Branch Taken" << endl;
					pc = pc + BI;
				}
				else
					pc = pc + 4;
				break;

			case 0x01: if(DebugMode | NormalMode) cout << "BNE detected" << endl;				// BNE detected (B-type)
				if (x[rs1] != x[rs2]){
					if(DebugMode) cout << "Branch Taken" << endl;
					pc = pc + BI;
				}
				else
					pc = pc + 4;
				break;

			case 0x04: if(DebugMode | NormalMode) cout << "BLT detected" << endl;				// BLT detected (B-type)
				if (int32_t(x[rs1]) < int32_t(x[rs2])){
					if(DebugMode) cout << "Branch Taken" << endl;
					pc = pc + BI;
				}
				else
					pc = pc + 4;
				break;

			case 0x05: if(DebugMode | NormalMode) cout << "BGE detected" << endl;				// BGE detected (B-type)
				if (int32_t(x[rs1]) >= int32_t(x[rs2])){
					if(DebugMode) cout << "Branch Taken" << endl;
					pc = pc + BI;
				}
				else
					pc = pc + 4;
				break;

			case 0x06: if(DebugMode | NormalMode) cout << "BLTU detected" << endl;				// BLTU detected (B-type)
				if ((uint32_t(x[rs1])) < (uint32_t(x[rs2]))){
					if(DebugMode) cout << "Branch Taken" << endl;
					pc = pc + BI;
				}
				else
					pc = pc + 4;
				break;

			case 0x07: if(DebugMode | NormalMode) cout << "BGEU detected" << endl;				// BGEU detected (B-type)
				if ((uint32_t(x[rs1])) >= (uint32_t(x[rs2]))){
					if(DebugMode) cout << "Branch Taken" << endl;
					pc = pc + BI;
				}
				else
					pc = pc + 4;
				break;
			}
			break;

		case 0x37: if(DebugMode) cout << "U-type Instruction" << endl;						// LUI
			UI = immediate(opcode, curr_instr);
			if(DebugMode | NormalMode) cout << "LUI detected" << endl;
			x[rd] = UI;
			pc = pc + 4;
			break;

		case 0x17: if(DebugMode) cout << "U-type Instruction" << endl;						// AUIPC
			UI = immediate(opcode, curr_instr);
			if(DebugMode | NormalMode) cout << "AUIPC detected" << endl;
			x[rd] = pc + UI;
			pc = pc + 4;
			break;

		case 0x6F: if(DebugMode) cout << "J-type Instruction" << endl;						// JAL
			JI = immediate(opcode, curr_instr);
			if(DebugMode | NormalMode) cout << "JAL detected" << endl;
			x[rd] = pc + 4;
			pc = pc + JI;
			break;

		case 0x0F: if(DebugMode | NormalMode) cout << "FENCE detected" << endl;
			// FENCE
			break;

		case 0x73: if(DebugMode | NormalMode) cout << "ECALL/EBREAK detected" << endl;
			// ECALL, EBREAK
			break;


		case 0x07: if(DebugMode) cout << "Floating point I-type Instruction" << endl;
			switch (funct3) {
			case 0x2: if(DebugMode | NormalMode) cout << "FLW detected" << endl;							//FLW
				FI = curr_instr >> 20;
				if(FI>>11){
					FI = FI | 0xFFFFF000;
				}
				if(DebugMode) cout << "F-immediate: " << FI <<endl;
				if ((FI + x[rs1]) % 4 == 0) {
					f[rd] = (mem_acc((FI + x[rs1]), 4, 1));
					pc = pc + 4;
				}
				else {
					cout << "Unaligned mem reference" << endl;
					x[1] = 0;
					pc = program_space - 4;
				}
				
				break;
			}
			break;
		case 0x27: if(DebugMode) cout << "Floating point S-type Instruction" << endl;
			switch (funct3) {
			case 0x2: if(DebugMode | NormalMode) cout << "FSW detected" << endl;         						//FSW
				FI = ((curr_instr & 0x00000F80) >> 7) | ((curr_instr & 0xFE000000) >> 20);
				if(FI>>11){
					FI = FI | 0xFFFFF000;
				}
				if(DebugMode) cout << "F-immediate: " << FI << endl;							
				mem_wr((FI + x[rs1]), 4, f[rs2]);
				pc = pc + 4;
				if(SingleStep) {
					print_mem();
					cout<< "Memory array updated"<<endl;
				}
				break;
			}
			break;
		case 0x43: if(DebugMode) cout << "Floating point R4-type Instruction" << endl;
			switch (funct3) {

			case 0x7:
				switch (funct2) {
				case 0x0: if(DebugMode | NormalMode) cout << "FMADD.S detected" << endl;
					f[rd] = (f[rs1] * f[rs2]) + f[rs3];  		//FMADD.S 
					pc = pc + 4;
					  break;
				}
				break;
			}
			break;
		case 0x47: if(DebugMode) cout << "Floating point R4-type Instruction" << endl;
			switch (funct3) {

			case 0x7:
				switch (funct2) {
				case 0x0: if(DebugMode | NormalMode) cout << "FMSUB.S detected" << endl;
					f[rd] = (f[rs1] * f[rs2]) - f[rs3];  		//FMSUB.S
					pc = pc + 4;
					break;
				}
				break;
			}
		case 0x4B: if(DebugMode) cout << "Floating point R4-type Instruction" << endl;
			switch (funct3) {
			case 0x7:
				switch (funct2) {
				case 0x0: if(DebugMode | NormalMode) cout << "FNMSUB.S detected" << endl;
					f[rd] = (-(f[rs1] * f[rs2])) + f[rs3]; 		//FNMSUB.S
					pc = pc + 4;
					break;
				}
				break;
			}
		case 0x4F: if(DebugMode) cout << "Floating point R4-type Instruction" << endl;
			switch (funct3) {
			case 0x7:
				switch (funct2) {
				case 0x0: if(DebugMode | NormalMode) cout << "FNMADD.S detected" << endl;
					f[rd] = (-(f[rs1] * f[rs2])) - f[rs3]; 		//FNMADD.S
					pc = pc + 4;
					break;
				}
				break;
			}

		case 0x53: if(DebugMode) cout << "Floating point R-type Instruction" << endl;
			switch (funct3) {

			case 0x07:
				switch (funct7) {
				case 0x00: if(DebugMode | NormalMode) cout << "FADD.S detected" << endl;
					f[rd] = f[rs1] + f[rs2];					//FADD.S
					pc = pc + 4;
					break;

				case 0x04: if(DebugMode | NormalMode) cout << "FSUB.S detected" << endl;
					f[rd] = f[rs1] - f[rs2];					//FSUB.S
					pc = pc + 4;
					break;

				case 0x08: if(DebugMode | NormalMode) cout << "FMUL.S detected" << endl;
					f[rd] = f[rs1] * f[rs2];					//FMUL.S
					pc = pc + 4;
					break;

				case 0x0C: if(DebugMode | NormalMode) cout << "FDIV.S detected" << endl;
					f[rd] = f[rs1] / f[rs2];					//FDIV.S
					pc = pc + 4;
					break;

				case 0x2C: if(DebugMode | NormalMode) cout << "FSQRT.S detected" << endl;
					f[rd] = sqrt(f[rs1]);						//FSQRT.S
					pc = pc + 4;
					break;

				case 0x60:
					switch (rs2) {
					case 0x00: if(DebugMode | NormalMode) cout << "FCVT.W.S detected" << endl;
						//operation here
						pc = pc + 4;
						break;
					case 0x01: if(DebugMode | NormalMode) cout << "FCVT.WU.S detected" << endl;
						//operation here
						pc = pc + 4;
						break;
					}
					break;

				case 0x68:
					switch (rs2) {
					case 0x00: if(DebugMode | NormalMode) cout << "FCVT.S.W detected" << endl;
						f[rd] = float(int32_t(x[rs1]));				//FCVT.S.W
						pc = pc + 4;
						break;
					case 0x01: if(DebugMode | NormalMode) cout << "FCVT.S.WU detected" << endl;
						f[rd] = float(uint32_t(x[rs1]));			//FCVT.S.WU
						pc = pc + 4;
						break;
					}
					break;

				}
				break;
			case 0x0:
				switch (funct7) {
				case 0x10: if(DebugMode | NormalMode) cout << "FSGNJ.S detected" << endl;
					//uint32_t temp1 = 0x80000000; uint32_t temp2 = 0x7FFFFFFF; uint32_t temp = (f[rs2] & temp1) >> 32; f[rd] = (f[rs1] & temp2) | (temp << 31);
				   //f[rd] = { f[rs2] & 0x80000000 | f[rs1] & 0x7FFFFFFF };		//FSGNJ.S
					pc = pc + 4;
					break;

				case 0x14: if(DebugMode | NormalMode) cout << "FMIN.S detected" << endl;
					f[rd] = min(f[rs1], f[rs2]);		 			//FMIN.S
					pc = pc + 4;
					break;

				case 0x78: if(DebugMode | NormalMode) cout << "FMV.X.W detected" << endl;
				{
					uint32_t value = (x[rs1] & 0xFFFFFFFF);
					float* ptr = reinterpret_cast<float*>(&value);
					f[rd] = *ptr;							//FMV.W.X
				}
				pc = pc + 4;
				break;

				case 0x50: if(DebugMode | NormalMode) cout << ".FLE.S detected" << endl;
					if (f[rs1] <= f[rs2]) {
						f[rd] = 1;
					}
					else {								//FLE.S
						f[rd] = 0;
					}
					pc = pc + 4;
					break;

				case 0x70: if(DebugMode | NormalMode) cout << "FMV.W.X detected" << endl;
					//operation here
					pc = pc + 4;
					break;
				}
				break;
			case 0x1:
				switch (funct7) {
				case 0x10: if(DebugMode | NormalMode) cout << "FSGNJN.S detected" << endl;
					//f[rd] = { ((!(f[rs2] & 0x80000000)) | (f[rs1] & 0x7FFFFFFF)) }; 	//FSGNJN.S
					pc = pc + 4;
					break;

				case 0x14: if(DebugMode | NormalMode) cout << "FMAX.S detected" << endl;
					f[rd] = max(f[rs1], f[rs2]);						//FMAX.S
					pc = pc + 4;
					break;

				case 0x50: if(DebugMode | NormalMode) cout << "FLT.S detected" << endl;
					if (f[rs1] < f[rs2]) {
						f[rd] = 1;
					}
					else {									//FLT.S
						f[rd] = 0;
					}
					pc = pc + 4;
					break;

				case 0x70: if(DebugMode | NormalMode) cout << "FCLASS.S detected" << endl;
					//operation here
					pc = pc + 4;
					break;
				}
				break;
			case 0x2:
				switch (funct7) {
				case 0x10: if(DebugMode | NormalMode) cout << "FSGNJX.S detected" << endl;
					//f[rd] = { (((f[rs1] & 0x80000000) ^ (f[rs2] & 0x80000000)) | (f[rs1] & 0x7FFFFFFF)) };	 //FSGNJX.S
					pc = pc + 4;
					break;

				case 0x50: if(DebugMode | NormalMode) cout << "FEQ.S detected" << endl;
					if (f[rs1] == f[rs2]) {
						f[rd] = 1;
					}
					else {									//FEQ.S
						f[rd] = 0;
					}
					pc = pc + 4;
					break;
				}
				break;

			default: if(DebugMode | NormalMode) cout << "Opcode doesn't exist" << endl;
				break;
			}
		}
		if(DebugMode | NormalMode){
		print_regs();
		if(opcode == 0x07 || opcode == 0x27 || opcode == 0x43 || opcode == 0x47 || opcode == 0x4B || opcode == 0x4F || opcode == 0x53)
			print_floatregs();
		}
		if(SingleStep)
			cin.get();
		if (pc % 4 != 0) {
			cout << "Unaligned pc" << endl;
			x[1] = 0;
			pc = program_space - 4;
		}
	}

	print_mem();
	cout << endl;
	cout << "************************************DONE************************************" << endl;
	if (DebugMode)
	cout << endl << "Last PC : " << pc << endl;
	cout << endl << "******************************Registers***********************************" << endl;
	print_regs();
	cout << "************************************END************************************" << endl;
	return 0;
}

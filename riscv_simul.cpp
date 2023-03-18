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

using namespace std;

#define PC_LOC 0
#define STACK_ADDR 65536
#define I_FILENAME "program.mem"

// Global vars
int DebugMode;
int pc; 				// program counter
int sp;					// stack address
int file;
// uint32_t registers[32];
uint8_t memory_array[65536] = {};		// Memory in bytes
uint32_t x[32] = {}; 					// r0 is zero

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
			if(mem_cont >> 15)
				mem_cont = mem_cont | 0xFFFF0000;
		}
	}
	else {
		mem_cont = memory_array[mem_p];
		if (sign) {
			if(mem_cont >> 7)
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
	std::cerr << endl << "Usage: riscv_simul <input file> <program counter> <stack address> <mode>" << endl;
	std::cerr << "input file \t- default is program.mem. Provide filename to read a specific file" << endl;
	std::cerr << "program counter - default is 0" << endl;
	std::cerr << "stck address \t- default is 65536" << endl;
	std::cerr << "mode \t\t- default is silent. pass 'debug' to enable debug mode " << endl;		// we can later add normal mode also if needed
	std::cerr << "Example - \'riscv_simul abcd.mem 4 65535 debug\'" << endl;
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
	int count = 0;
	for (int i = 0; i < 32; i++) {
		cout << "x[" << std::dec << i << "]" << " - " << std::hex << std::setw(8) << setfill('0') << x[i] << " \t ";
		count++;
		if (count % 8 == 0) {
			cout << endl;
		}
	}
	cout << endl;
}

int main(int argc, char* argv[]) {
	std::string line;
	std::ifstream infile;
	std::ofstream outfile("memory_array.txt");
	unsigned int trace[2];
	uint32_t instr, curr_instr;
	uint32_t file_pc;
	uint32_t opcode, funct3, funct7, rd, rs1, rs2;
	int32_t II, SI, BI, UI, JI; 						// Immediate fields 
	bool MSBimmediate;
	uint32_t memory_loc;
	int bytes;
	uint32_t f_byte[4];
	int program_space;
	int t;

	switch (argc) {
	case 1: infile.open(I_FILENAME);			// No Arguments provided. Read program.mem, pc 0, sa 65535, verbose disabled
		check_file(infile.is_open());
		pc = PC_LOC;
		sp = STACK_ADDR;
		DebugMode = 0;
		break;
	case 2: infile.open(argv[1]);				// 1 Arguments provided. Read xyz.mem, pc 0, sa 65535, verbose disabled
		check_file(infile.is_open());
		pc = PC_LOC;
		sp = STACK_ADDR;
		DebugMode = 0;
		break;
	case 3: infile.open(argv[1]);				// 2 Arguments provided. Read xyz.mem, pc x, sa 65535, verbose disabled
		check_file(infile.is_open());
		sscanf(argv[2], "%x", &pc);
		//pc = (int) atoi(argv[2]);			// To change from char* to int
		sp = STACK_ADDR;
		DebugMode = 0;
		break;
	case 4: infile.open(argv[1]);				// 3 Arguments provided. Read xyz.mem, pc x, sa y, verbose disabled
		check_file(infile.is_open());
		//pc = (int) atoi(argv[2]);
		sscanf(argv[2], "%x", &pc);
		sp = (int)atoi(argv[3]);
		DebugMode = 0;
		break;
	case 5: infile.open(argv[1]);				// 4 Arguments provided. Read xyz.mem, pc x, sa y, verbose enabled
		check_file(infile.is_open());
		//pc = (int) atoi(argv[2]);
		sscanf(argv[2], "%x", &pc);
		sp = (int)atoi(argv[3]);
		if (strcmp(argv[4], "debug") == 0) {
			cout << "Debug Mode enabled" << endl;
			DebugMode = 1;
		}
		else {
			cout << "Incorrect mode specified" << endl;
			usage();
		}
		break;
	default: usage();
	}
	cout << endl << "WELCOME TO RISC-V SIMULATOR" << endl;
	cout << "Program Counter - " << std::hex << pc << endl;
	cout << "Stack Address - " << sp << endl;
	cout << "Debug mode - " << DebugMode << endl;
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

		cout << endl << "pc: " << std::hex << std::uppercase << file_pc << " instr: " << std::hex << instr;		// Comment later
		program_space = file_pc + 4;																// Save Program space
	}
	infile.close();
	cout << endl << "End of file reading and program saved to memory" << endl;						// Comment later

	while (pc < program_space)
	{
		curr_instr = memory_array[pc] | (memory_array[pc + 1] << 8) | (memory_array[pc + 2] << 16) | (memory_array[pc + 3] << 24);
		cout << endl << "pc: " << std::hex << std::uppercase << pc << " instr: " << std::hex << curr_instr << endl;
		if (curr_instr == 0x00000000) {								// Check for all 0 instr
			cout<<"Ending simulation !!! (all 0 instr)";
			break;
		}
		if ((curr_instr == 0x00008067) && (x[1] == 0)) {			// Check for jr ra where ra is 0
			cout<<"Ending simulation !!! (jr ra 0)";
			break;
		}
		// Store everything regardless of instr type 
		opcode = curr_instr & (0x7F);								// bits [6:0]
		funct3 = (curr_instr & (0x7000)) >> 12;						// bits [14:12]
		funct7 = (curr_instr & (0xFE000000)) >> 25;					// bits [31:25]
		rd = (curr_instr & (0xF80)) >> 7;							// bits [11:7]
		rs1 = (curr_instr & (0xF8000)) >> 15;						// bits [19:15]
		rs2 = (curr_instr & (0x1F00000)) >> 20;						// bits [24:20]
		// We have to calculate immediate fields also - refer page 16/17
		
		x[0] = 0;
		// Enable this in debug mode
		cout << std::uppercase << std::hex << "Opcode: " << opcode << " funct3: " << funct3 << " funct7: " << funct7 << endl;
		cout << std::uppercase << std::hex << "rd: " << rd << " rs1: " << rs1 << " rs2: " << rs2 << endl;

		switch (opcode) {
		case 0x33: cout << "R-type Instruction" << endl;
			switch (funct7) {
			case 0x00:
				switch (funct3) {
				case 0x00: 	cout << "ADD detected" << endl;			// ADD detected (R-type)
					x[rd] = x[rs1] + x[rs2];
					pc = pc + 4;
					break;

				case 0x01: 	cout << "SLL detected" << endl;			// SLL detected (R-type)
					x[rd] = (uint32_t(x[rs1]) << (x[rs2] & 0x1F));
					pc = pc + 4;
					break;

				case 0x02:	cout << "SLT detected" << endl;			// SLT detected (R-type)
					if (int32_t(x[rs1]) < int32_t(x[rs2]))
						x[rd] = 1;
					else
						x[rd] = 0;
					pc = pc + 4;
					break;

				case 0x03:	cout << "SLTU detected" << endl;		// SLTU detected (R-type)
					if ((uint32_t(x[rs1])) < (uint32_t(x[rs2])))
						x[rd] = 1;
					else
						x[rd] = 0;
					pc = pc + 4;
					break;

				case 0x04:	cout << "XOR detected" << endl;			// XOR detected (R-type)
					x[rd] = (x[rs1] ^ x[rs2]);
					pc = pc + 4;
					break;

				case 0x05:	cout << "SRL detected" << endl;			// SRL detected (R-type)
					x[rd] = ((uint32_t(x[rs1])) >> (x[rs2] & 0x1F));
					pc = pc + 4;
					break;

				case 0x06:	cout << "OR detected" << endl;			// OR detected (R-type)
					x[rd] = (x[rs1] | x[rs2]);
					pc = pc + 4;
					break;

				case 0x07:	cout << "AND detected" << endl;			// AND detected (R-type)
					x[rd] = (x[rs1] & x[rs2]);
					pc = pc + 4;
					break;
				}
				break;

			case 0x20:
				switch (funct3) {
				case 0x00:	cout << "SUB detected" << endl;			// SUB detected (R-type)
					x[rd] = x[rs1] - x[rs2];
					pc = pc + 4;
					break;

				case 0x05: cout << "SRA detected" << endl;			// SRA detected (R-type)
					x[rd] = (int32_t(x[rs1]) >> (x[rs2] & 0x1F));
					pc = pc + 4;
					break;
				}
				break;
			}
			break;

		case 0x03: cout << "I-type Instruction" << endl;
			// LB, LH, LW, LBU, LHU
			II = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00:	cout << "LB detected" << endl;				// LB detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 1,1));
				pc = pc + 4;
				break;

			case 0x01: cout << "LH detected" << endl;				// LH detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 2,1));
				pc = pc + 4;
				break;

			case 0x02: cout << "LW detected" << endl;				// LW detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 4,1));
				pc = pc + 4;
				break;

			case 0x04: cout << "LBU detected" << endl;				// LBU detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 1,0));
				pc = pc + 4;
				break;

			case 0x05: cout << "LHU detected" << endl;				// LHU detected (I-type)
				x[rd] = (mem_acc((II + x[rs1]), 2,0));
				pc = pc + 4;
				break;
			}
			break;

		case 0x13: cout << "I-type Instruction" << endl;
			// ADDI, SLTI, SLTIU, XORI, ORI, ANDI 
			II = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00:	cout << "ADDI detected" << endl;			// ADDI detected (I-type)
				x[rd] = x[rs1] + II;
				pc = pc + 4;
				break;

			case 0x02: cout << "SLTI detected" << endl;				// SLTI detected (I-type)
				if (int32_t(x[rs1]) < II)
					x[rd] = 1;
				else
					x[rd] = 0;
				pc = pc + 4;
				break;

			case 0x03: cout << "SLTIU detected" << endl;			// SLTIU detected (I-type)
				if (uint32_t(x[rs1]) < uint32_t(II))
					x[rd] = 1;
				else
					x[rd] = 0;
				pc = pc + 4;
				break;

			case 0x04: cout << "XORI detected" << endl;				// XORI detected (I-type)
				x[rd] = x[rs1] ^ II;
				pc = pc + 4;
				break;

			case 0x06: cout << "ORI detected" << endl;				// ORI detected (I-type)
				x[rd] = x[rs1] | II;
				pc = pc + 4;
				break;

			case 0x07: cout << "ANDI detected" << endl;				// ANDI detected (I-type)
				x[rd] = x[rs1] & II;
				pc = pc + 4;
				break;
				
			case 0x01: cout << "SLLI detected" << endl;             //SLLI detected (I-type)
				x[rd] = x[rs1] << rs2;
				pc = pc + 4;
				break;
				
			case 0x05: 
				switch(funct7){
				case 0x00: cout << "SRLI detected" << endl;         //SRLI detected (I-type)
					x[rd] = uint32_t(x[rs1]) >> rs2;
					pc = pc + 4;
					break;
					
				case 0x20: cout << "SRAI detected" << endl;         //SRLI detected (I-type)
					x[rd] = int32_t(x[rs1]) >> rs2;
					pc = pc + 4;
					break;
				}
			}
			break;

		case 0x67: cout << "I-type Instruction" << endl;
			// only JALR!
			II = immediate(opcode, curr_instr);
			cout << "JALR detected" << endl;						// JALR detected (I-type)
			x[rd] = pc + 4;
			pc = ((x[rs1] + II) & 0xFFFFFFFE);
			break;

		case 0x23: cout << "S-type Instruction" << endl;			
			// SB,SH,SW
			SI = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00:	cout << "SB detected" << endl;				// SB detected (S-type)
				mem_wr((SI + x[rs1]),1,x[rs2]);
				pc = pc + 4;
				break;

			case 0x01: cout << "SH detected" << endl;				// SH detected (S-type)
				// Operation here
				mem_wr((SI + x[rs1]),2,x[rs2]);
				pc = pc + 4;
				break;

			case 0x02: cout << "SW detected" << endl;				// SW detected (S-type)
				// Operation here
				mem_wr((SI + x[rs1]),4, x[rs2]);
				pc = pc + 4;
				break;
			}
			break;

		case 0x63: cout << "B-type Instruction" << endl;
			// BEQ, BNE, BLT, BGE, BLTU, BGEU
			BI = immediate(opcode, curr_instr);
			switch (funct3) {
			case 0x00:	cout << "BEQ detected" << endl;				// BEQ detected (B-type)
				if (x[rs1] == x[rs2])
					pc = pc + BI;
				else
					pc = pc + 4;
				break;

			case 0x01: cout << "BNE detected" << endl;				// BNE detected (B-type)
				if (x[rs1] != x[rs2])
					pc = pc + BI;
				else
					pc = pc + 4; 
				break;

			case 0x04: cout << "BLT detected" << endl;				// BLT detected (B-type)
				x[14]=2;
				if (int32_t(x[rs1]) < int32_t(x[rs2]))
					pc = pc + BI;
				else
					pc = pc + 4;
				break;

			case 0x05: cout << "BGE detected" << endl;				// BGE detected (B-type)
				if (int32_t(x[rs1]) >= int32_t(x[rs2]))
					pc = pc + BI;
				else
					pc = pc + 4;
				break;

			case 0x06: cout << "BLTU detected" << endl;				// BLTU detected (B-type)
				if ((uint32_t(x[rs1])) < (uint32_t(x[rs2])))
					pc = pc + BI;
				else
					pc = pc + 4;
				break;

			case 0x07: cout << "BGEU detected" << endl;				// BGEU detected (B-type)
				if ((uint32_t(x[rs1])) >= (uint32_t(x[rs2])))
					pc = pc + BI;
				else
					pc = pc + 4;
				break;
			}
			break;

		case 0x37: cout << "U-type Instruction" << endl;
																	// LUI
			UI = immediate(opcode, curr_instr);
			cout << "LUI detected" << endl;
			x[rd] = UI;
			pc = pc + 4;
			break;

		case 0x17: cout << "U-type Instruction" << endl;
			// AUIPC
			UI = immediate(opcode, curr_instr);
			cout << "AUIPC detected" << endl;
			x[rd] = pc + UI;
			pc = pc + 4;
			break;

		case 0x6F: cout << "J-type Instruction" << endl;
			// JAL
			JI = immediate(opcode, curr_instr);
			cout << "JAL detected" << endl; 
			x[rd] = pc + 4;
			pc = pc + JI;
			break;

		case 0x0F: cout << "FENCE detected" << endl;
			// FENCE
			break;

		case 0x73: cout << "ECALL/EBREAK detected" << endl;
			// ECALL, EBREAK
			break;

		default: cout << "Opcode doesn't exist" << endl;
			break;
		}
		cout<<pc<<endl;
		print_regs();
	}
	// For printing the contents of memory into a file - memory_array.txt
	for (int i = 0; i < 65536; i++) {
		outfile << i << " : " << std::hex << std::setw(2) << setfill('0') << static_cast<int>(memory_array[i]) << endl;
	}
	outfile.close();
	return 0;
}

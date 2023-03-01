#include <bits/stdc++.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include <string.h>
#include <cctype>
#include <cstring>

using namespace std;

#define PC_LOC 0
#define STACK_ADDR 65535
#define I_FILENAME "program.mem"

// Prints the usage of the program.
void usage()
{
    std::cerr << endl <<"Usage: riscv_simul <input file> <program counter> <stack address> <mode>" << endl;
    std::cerr << "input file \t- default is program.mem. Provide filename to read a specific file" << endl;
	std::cerr << "program counter - default is 0" << endl;
	std::cerr << "stck address \t- default is 65535" << endl;
	std::cerr << "mode \t\t- default is silent. pass 'debug' to enable debug mode " << endl;		// we can later add normal mode also if needed
	std::cerr << "Example - \'riscv_simul abcd.mem 4 65535 debug\'" << endl;
    exit(1);
}

// To check if file exists and whether to proceed or not.
void check_file (int check) {
	if(check) {
		cout<<"File exists"<<endl;
	}
	else {
		cout<<"File does not exist. Please check the input mem file."<<endl;
		usage();
	} 
}

// Parse for trace file.
void parse_line(std::string access,unsigned int arr[]) {
	// What we want to parse
	unsigned int instruction;
	unsigned int trace_pc;
	// Parse from the string we read from the file
	sscanf(access.c_str(), "%lx: %lx", &trace_pc, &instruction);
	arr[0]=trace_pc;
	arr[1]=instruction;
}

// Global vars
int DebugMode;
int pc; 				// program counter
int sa;					// stack address
int file;

int main(int argc, char* argv[]) {
	std::string line;
	std::ifstream infile;
	unsigned int trace[2];
	uint32_t instr;
	uint32_t trace_pc;
	uint32_t opcode, funct3, funct7, rd, rs1, rs2;
	uint32_t II,SI,BI,UI,JI; 						// Immediate fields 
	
	switch (argc) {
		case 1: infile.open(I_FILENAME);			// No Arguments provided. Read program.mem, pc 0, sa 65535, verbose disabled
				check_file(infile.is_open());
				pc = PC_LOC;
				sa = STACK_ADDR;
				DebugMode = 0;
				break;
		case 2: infile.open(argv[1]);				// 1 Arguments provided. Read xyz.mem, pc 0, sa 65535, verbose disabled
				check_file(infile.is_open());
				pc = PC_LOC;
				sa = STACK_ADDR;
				DebugMode = 0;
				break;
		case 3: infile.open(argv[1]);				// 2 Arguments provided. Read xyz.mem, pc x, sa 65535, verbose disabled
				check_file(infile.is_open());
				sscanf(argv[2],"%lx",&pc);
				//pc = (int) atoi(argv[2]);			// To change from char* to int
				sa = STACK_ADDR;
				DebugMode = 0;
				break;
		case 4: infile.open(argv[1]);				// 3 Arguments provided. Read xyz.mem, pc x, sa y, verbose disabled
				check_file(infile.is_open());
				//pc = (int) atoi(argv[2]);
				sscanf(argv[2],"%lx",&pc);
				sa = (int) atoi(argv[3]);
				DebugMode = 0;
				break;
		case 5: infile.open(argv[1]);				// 4 Arguments provided. Read xyz.mem, pc x, sa y, verbose enabled
				check_file(infile.is_open());
				//pc = (int) atoi(argv[2]);
				sscanf(argv[2],"%lx",&pc);
				sa = (int) atoi(argv[3]);
				if(strcmp(argv[4],"debug")==0) {
					cout<<"Debug Mode enabled"<<endl;	
					DebugMode = 1;
				}
				else {
					cout<<"Incorrect mode specified"<<endl;
					usage();
				}
				break;
		default: usage();
		}
	cout<<endl<<"WELCOME TO RISC-V SIMULATOR"<<endl;
	cout<<"Program Counter - "<<std::hex<<pc<<endl;
	cout<<"Stack Address - "<<sa<<endl;
	cout<<"Debug mode - "<<DebugMode<<endl;
	
	// Main Program begins here!
	
	while(std::getline(infile, line)){
		parse_line(line,trace);
		trace_pc= trace[0];
		instr= trace[1];
		
		if(trace_pc==pc){
			cout<<endl<<"pc: "<<std::hex<<std::uppercase<<trace_pc<<" instr: "<<std::hex<<instr<<endl;
			
			// Store everything regardless of instr type 
			opcode = instr & (0x7F);								// bits [6:0]
			funct3 = (instr & (0x7000)) >> 12;						// bits [14:12]
			funct7 = (instr & (0xFE000000)) >> 25;					// bits [31:25]
			rd = (instr & (0xF80)) >> 7;							// bits [11:7]
			rs1 = (instr & (0xF8000)) >> 15;						// bits [19:15]
			rs2 = (instr & (0x1F00000)) >> 20;						// bits [24:20]
			// We have to calculate immediate fields also - refer page 16/17
			
			// Enable this in debug mode
			cout<<std::uppercase<<std::hex<<"Opcode: "<<opcode<<" funct3: "<<funct3<<" funct7: "<<funct7<<endl;
			cout<<std::uppercase<<std::hex<<"rd: "<<rd<<" rs1: "<<rs1<<" rs2: "<<rs2<<endl;
			
			switch(opcode) {
				case 0x33: cout<<"R-type Instruction"<<endl;
					switch(funct7) {
						case 0x00:
							switch(funct3) {
								case 0x00: 	cout<<"ADD detected"<<endl;
											// Operation here
								break;
								
								case 0x01: 	cout<<"SLL detected"<<endl;
											// Operation here
								break;
								
								case 0x02:	cout<<"SLT detected"<<endl;
											// Operation here
								break;
								
								case 0x03:	cout<<"SLTU detected"<<endl;
											// Operation here
								break;
								
								case 0x04:	cout<<"XOR detected"<<endl;
											// Operation here
								break;
								
								case 0x05:	cout<<"SRL detected"<<endl;
											// Operation here
								break;
								
								case 0x06:	cout<<"OR detected"<<endl;
											// Operation here
								break;
								
								case 0x07:	cout<<"AND detected"<<endl;
											// Operation here
								break;
							}
						break;
						
						case 0x20:
							switch(funct3) {
								case 0x00:	cout<<"SUB detected"<<endl;
											// Operation here
								break;
								
								case 0x05: cout<<"SRA detected"<<endl;
											// Operation here
								break;
							}
						break;
					}
				break;
					
				case 0x03: cout<<"I-type Instruction"<<endl;
					II = (instr & (0xFFF00000))>>20;					// I-immediate
					cout<<"I-immediate: "<<II<<endl;
					// LB, LH, LW, LBU, LHU
					switch(funct3) {
						case 0x00:	cout<<"LB detected"<<endl;
									// Operation here
						break;
						
						case 0x01: cout<<"LH detected"<<endl;
									// Operation here
						break;
						
						case 0x02: cout<<"LW detected"<<endl;
									// Operation here
						break;
						
						case 0x04: cout<<"LBU detected"<<endl;
									// Operation here
						break;
						
						case 0x05: cout<<"LHU detected"<<endl;
									// Operation here
						break;
					}
				break;
				
				case 0x13: cout<<"I-type Instruction"<<endl;
					II = (instr & (0xFFF00000))>>20;					// I-immediate
					cout<<"I-immediate: "<<II<<endl;
					// ADDI, SLTI, SLTIU, XORI, ORI, ANDI 
					switch(funct3) {
						case 0x00:	cout<<"ADDI detected"<<endl;
									// Operation here
						break;
						
						case 0x02: cout<<"SLTI detected"<<endl;
									// Operation here
						break;
						
						case 0x03: cout<<"SLTIU detected"<<endl;
									// Operation here
						break;
						
						case 0x04: cout<<"XORI detected"<<endl;
									// Operation here
						break;
						
						case 0x06: cout<<"ORI detected"<<endl;
									// Operation here
						break;
						
						case 0x07: cout<<"ANDI detected"<<endl;
									// Operation here
						break;
					}
				break;
				
				case 0x67: cout<<"I-type Instruction"<<endl;
					II = (instr & (0xFFF00000))>>20;					// I-immediate
					cout<<"I-immediate: "<<II<<endl;
					// only JALR!
					cout<<"JALR detected"<<endl;
				break;
				
				case 0x23: cout<<"S-type Instruction"<<endl;
					SI = ((instr & (0xF80))>>7) | ((instr & (0xFE000000))>>20);
					cout<<"S immediate: "<<SI<<endl;
				// SB,SH,SW
					switch(funct3) {
						case 0x00:	cout<<"SB detected"<<endl;
									// Operation here
						break;
						
						case 0x01: cout<<"SH detected"<<endl;
									// Operation here
						break;
						
						case 0x02: cout<<"SW detected"<<endl;
									// Operation here
						break;
					}
				break;
				
				case 0x63: cout<<"B-type Instruction"<<endl;
					// BEQ, BNE, BLT, BGE, BLTU, BGEU
					//BI = (instr & (0xFFF00000))>>20;					// B-immediate
					//cout<<"B-immediate: "<<BI<<endl;
					switch(funct3) {
						case 0x00:	cout<<"BEQ detected"<<endl;
									// Operation here
						break;
						
						case 0x01: cout<<"BNE detected"<<endl;
									// Operation here
						break;
						
						case 0x04: cout<<"BLT detected"<<endl;
									// Operation here
						break;
						
						case 0x05: cout<<"BGE detected"<<endl;
									// Operation here
						break;
						
						case 0x06: cout<<"BLTU detected"<<endl;
									// Operation here
						break;
						
						case 0x07: cout<<"BGEU detected"<<endl;
									// Operation here
						break;
					}
				break;
				
				case 0x37: cout<<"U-type Instruction"<<endl;
					// LUI
					//UI = ;					// U-immediate
					//cout<<"U-immediate: "<<UI<<endl;
					cout<<"LUI detected"<<endl;
				break;
				
				case 0x17: cout<<"U-type Instruction"<<endl;
					// AUIPC
					//UI = ;					// U-immediate
					//cout<<"U-immediate: "<<UI<<endl;
					cout<<"AUIPC detected"<<endl;
				break;
				
				case 0x6F: cout<<"J-type Instruction"<<endl;
					// JAL
					//UI = ;					// J-immediate
					//cout<<"J-immediate: "<<JI<<endl;
					cout<<"JAL detected"<<endl;
				break;
				
				// !!! Need to check on SLLI, SRLI and SRAI - opcode is 0x13, but not I type? !!!
				
				case 0x0F: cout<<"FENCE detected"<<endl;
				// FENCE
				break;
				
				case 0x73: cout<<"ECALL/EBREAK detected"<<endl;
				// ECALL, EBREAK
				break;
				
				default: cout<<"Opcode doesn't exist"<<endl; 
				break;
			}
			pc= pc+4;
		}
	}
	infile.close();
	return 0;
}

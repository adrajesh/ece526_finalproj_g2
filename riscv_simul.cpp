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

// Global vars
int DebugMode;
int pc; 				// program counter
int sa;					// stack address

int main(int argc, char* argv[]) {
	std::string line;
	std::ifstream infile;
	
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
				pc = (int) atoi(argv[2]);			// To change from char* to int
				sa = STACK_ADDR;
				DebugMode = 0;
				break;
		case 4: infile.open(argv[1]);				// 3 Arguments provided. Read xyz.mem, pc x, sa y, verbose disabled
				check_file(infile.is_open());
				pc = (int) atoi(argv[2]);
				sa = (int) atoi(argv[3]);
				DebugMode = 0;
				break;
		case 5: infile.open(argv[1]);				// 4 Arguments provided. Read xyz.mem, pc x, sa y, verbose enabled
				check_file(infile.is_open());
				pc = (int) atoi(argv[2]);
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
	cout<<"Program Counter - "<<pc<<endl;
	cout<<"Stack Address - "<<sa<<endl;
	cout<<"Debug mode - "<<DebugMode;
	
	// Main Program begins here!
	
	infile.close();
	return 0;
}
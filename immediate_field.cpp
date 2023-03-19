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

int immediate(uint32_t opcode, uint32_t instr){
	uint32_t I;
	bool MSBimmediate;
	switch(opcode){
		case 0x03: I = (instr & (0xFFF00000))>>20;				   // I-immediate
				MSBimmediate= I>>11;                               //check the MSB of 12 bit immediate field
				if(MSBimmediate){                                   
					I= (I | 0xFFFFF000);                           //extend 1 from 12th bit to 31th bit
				}
				cout<<"I-immediate: "<<I<<endl;
				break;
		
		case 0x13: I = (instr & (0xFFF00000))>>20;				   // I-immediate
				MSBimmediate= I>>11;                               //check the MSB of 12 bit immediate field
				if(MSBimmediate){                                   
					I= (I | 0xFFFFF000);                           //extend 1 from 12th bit to 31th bit
				}
				cout<<"I-immediate: "<<I<<endl;
				break;
		
		case 0x67: I = (instr & (0xFFF00000))>>20;				   // I-immediate
				MSBimmediate= I>>11;                               //check the MSB of 12 bit immediate field
				if(MSBimmediate){                                   
					I= (I | 0xFFFFF000);                           //extend 1 from 12th bit to 31th bit
				}
				cout<<"I-immediate: "<<I<<endl;
				break;
        	
		case 0x23: I = ((instr & (0xF80))>>7) | ((instr & (0xFE000000))>>20);         //S-immediate
				MSBimmediate= I>>11;                               //check the MSB of 12 bit immediate field
				if(MSBimmediate){                                   
					I= (I | 0xFFFFF000);                             //extend 1 from 12th bit to 31th bit
				}
				cout<<"S immediate: "<<I<<endl;
				break;
				
		case 0x63: I = ((instr & (0xF00))>>7) | ((instr & (0x7E000000))>>20) | ((instr & (0x80))<<4) | ((instr & (0x80000000))>>19);	// B-immediate
				MSBimmediate= I>>12;                               //check the MSB of 13 bit immediate field
				if(MSBimmediate){                                   
					I= (I | 0xFFFFE000);                             //extend 1 from 13th bit to 31th bit
				}
				cout<<"B-immediate: "<<I<<endl;
				break;
				
		case 0x37: I = (instr & (0xFFFFF000));					// U-immediate
				cout<<"U-immediate: "<<I<<endl;
				break;
				
		case 0x17: I = (instr & (0xFFFFF000));					// U-immediate
				cout<<"U-immediate: "<<I<<endl;
				break;
				
		case 0x6F: I = ((instr & (0x7FE00000))>>20) | ((instr & (0x100000))>>9) | (instr & (0xFF000)) | ((instr & (0x80000000))>>11);	// J-immediate
				MSBimmediate= I>>20;                               //check the MSB of 21 bit immediate field
				if(MSBimmediate){                                   
					I= (I | 0xFFFE00000);                             //extend 1 from 21th bit to 31th bit
				}
				cout<<"J-immediate: "<<I<<endl;
				break;
					
		case 0x07: I = ((instr & (0xFFF00000))>>20);
				cout<<"Floating point I-immediate"<<I<<endl;
		  		break;
		  		
		case 0x27: I = ((instr & (0x7F00000))>>25) | ((instr & (0x00000F80))>>7);
				cout << "floating point S-immediate"<<I<<endl;
				break;
	}
	return I;	
}

/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "pin.H"
#include <ctime>


namespace WINDOWS
{
#include <Windows.h>
}

using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

ofstream OutFile;
using namespace std;

bool heavensGateFlag = false;
bool after_hevens_gate = false;
bool trackflag2 = false;
bool only_once = true;
clock_t handling_total_elasped = 0;
clock_t bypassing_total_elasped = 0;
int handling_times = 0;
int bypassing_times = 0;
string prev_call = "";

bool is_first = false;

bool bStopTrace = false;

bool OEP_has_reached = false;
int temp_cnt = 0;
string current_file_name;
ADDRINT OEP = 0;
ADDRINT OEPLowerBound, OEPUpperBound = 0;
ADDRINT APPAddrLowerBound, APPAddrUpperBound = 0;
int procId;
bool log_flag = false;

VOID imgInstrumentation(IMG img, VOID *val)
{
    if (IMG_IsMainExecutable(img)) {
        int sec_count = 0; 
        APPAddrLowerBound = SEC_Address(IMG_SecHead(img));
        for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
            if (SEC_Name(sec) == ".text" || SEC_Name(sec) == "UPX0") {
                OEPLowerBound = SEC_Address(sec);
                OEPUpperBound = SEC_Address(SEC_Next(sec));
                current_file_name = IMG_Name(img);
                std::cout << hex << "lower : " << OEPLowerBound << " upper: " << OEPUpperBound << std::endl;
            }
            APPAddrUpperBound = SEC_Address(sec) + SEC_Size(sec) ;
        }
        std::cout << hex << "Applower : " << APPAddrLowerBound << " and " << "Appupper : " << APPAddrUpperBound << std::endl;
    }
}

VOID bypass_heavens_Gate(CONTEXT* ctxt) {
    bypassing_times += 1;
    clock_t bypassing_start = clock();
    ADDRINT esp = PIN_GetContextReg(ctxt, REG_ESP);
    
    PIN_SetContextReg(ctxt, REG_ESP, esp+0x8);
    //if (!log_flag){
        

    procId = PIN_GetPid();
    
    WINDOWS::DWORD dwPid = 0;
    WINDOWS::HANDLE hProc = WINDOWS::OpenProcess(PROCESS_ALL_ACCESS, TRUE, procId);
    WINDOWS::LPVOID lpScanAddress = (WINDOWS::LPVOID)APPAddrLowerBound;
    WINDOWS::LPVOID lastSectionAddress = (WINDOWS::LPVOID)((WINDOWS::DWORD)APPAddrUpperBound);
    WINDOWS::MEMORY_BASIC_INFORMATION MmInfo = { 0, };

    //std::cout << "heaven from " << lpScanAddress << " to " << lastSectionAddress << std::endl;
    while (lpScanAddress < lastSectionAddress ){
        WINDOWS::VirtualProtectEx(hProc, lpScanAddress, sizeof(WINDOWS::DWORD), PAGE_EXECUTE_READWRITE, &dwPid);
        lpScanAddress = (WINDOWS::LPVOID)((WINDOWS::DWORD)lpScanAddress + (WINDOWS::DWORD)0x1000);
    }
    
    WINDOWS::CloseHandle(hProc);
    only_once = false;

        
    //}
    //std::cout << "executed " << std::endl;
    after_hevens_gate = true;
    clock_t bypassing_end = clock();
    bypassing_total_elasped += (bypassing_end - bypassing_start);
    PIN_ExecuteAt(ctxt);  
     
}



VOID dump_ss(std::string *instructionString, ADDRINT ip)
{	
    if(RTN_FindNameByAddress(ip) == prev_call){
        return;
    }
    OutFile << *instructionString;
}


vector<string> split(string str, char Delimiter){
    istringstream iss(str);             
    string buffer;                      
 
    vector<string> result;
 
    while(getline(iss, buffer, Delimiter)) {
        result.push_back(buffer);
    }
 
    return result;
}

VOID set_flag()
{	
    heavensGateFlag = true;
    trackflag2 = true;
}
// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID* v)
{
    string asm_string = INS_Disassemble(ins);
    stringstream ss;
    ADDRINT address = INS_Address(ins);
    /*
    if (log_flag == true) {
    ss << hex << "(" << RTN_FindNameByAddress(address) << ") " << address << " : " << asm_string;
    INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)dump_reg, IARG_PTR, new string(ss.str()),
            IARG_INST_PTR,
            IARG_REG_VALUE, REG_EAX,
            IARG_REG_VALUE, REG_ECX,
            IARG_REG_VALUE, REG_EDX,
            IARG_REG_VALUE, REG_EBX,
            IARG_REG_VALUE, REG_ESP,
            IARG_REG_VALUE, REG_EBP,
            IARG_REG_VALUE, REG_ESI,
            IARG_REG_VALUE, REG_EDI,
            IARG_END);
    }*/
    if (!OEP_has_reached) {
        handling_times += 1;
        clock_t handling_start = clock();                
        if (heavensGateFlag){           
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)bypass_heavens_Gate, IARG_CONTEXT, IARG_END);
            heavensGateFlag = false;
        }

        if(INS_IsFarJump(ins)){    
            //ADDRINT targetSelector = INS_OperandReg(ins, 1);
            vector<string> splited_string1 = split(asm_string, ',');
            //std::cout<< hex << "debug addr:" << address << std::endl;
            
            ADDRINT target_address1 = AddrintFromString(splited_string1[1]);
            //std::cout<< hex << "target_address1 addr:" << target_address1<< std::endl;
            if (target_address1 != 0x33) {
                return;
            }
            
            INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)dump_ss, IARG_PTR, new string(ss.str()), IARG_INST_PTR, IARG_END);
            INS push_ins = INS_Prev(ins);
            string push_asm = INS_Disassemble(push_ins);

            vector<string> splited_string = split(push_asm, ' ');
            
            ADDRINT target_address = AddrintFromString(splited_string[1]);
            //std::cout<< hex << "target_address:" << target_address << std::endl;
            INS_Delete(ins);
            heavensGateFlag = true;            
            INS_InsertDirectJump(ins, IPOINT_BEFORE, target_address);
        } 
        clock_t handling_end = clock();
        handling_total_elasped += (handling_end - handling_start);
        

        if (address >= OEPLowerBound && address <= OEPUpperBound) {            
            OutFile << hex <<   "OEP Reached, OEP is " << address << endl;
            OutFile << hex <<   "Target Process ID is " << procId << endl;
            OutFile << "Handling elasped time : " << (double(handling_total_elasped) / CLOCKS_PER_SEC)  << endl;
            OutFile << "executed : " << handling_times  << endl;            
            OutFile << "Bypassing elasped time : " << (double(bypassing_total_elasped) / CLOCKS_PER_SEC) << endl;
            OutFile << "executed : " << bypassing_times  << endl;            
            OutFile.close();
            WINDOWS::Sleep(1000000000000);
            OEP_has_reached = true;
        } else {
            return;
        }
    }
    
}


KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount_64.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID* v)
{
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char* argv[])
{
    // Initialize pin

    PIN_InitSymbols();
    
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());
    
    IMG_AddInstrumentFunction(imgInstrumentation, 0);
    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);
    //TRACE_AddInstrumentFunction(Trace, 0);
    

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}

#include "pipeline.h"
#include <iostream>

Pipeline::Pipeline(std::vector<Instruction> prog)
    : program(prog) {}

// ---------------- STEP ----------------

void Pipeline::step() {
    cycles++;

    next_IF_ID = {};
    next_ID_EX = {};
    next_EX_MEM = {};
    next_MEM_WB = {};

    writeback();

    // 🔥 Global memory stall control
    if (mem_stall_cycles > 0) {
        mem_stall_cycles--;
        mem_stalls++;   // track memory stall cycles

        // freeze entire pipeline
        next_IF_ID = IF_ID;
        next_ID_EX = ID_EX;
        next_EX_MEM = EX_MEM;
        next_MEM_WB = MEM_WB;

        IF_ID = next_IF_ID;
        ID_EX = next_ID_EX;
        EX_MEM = next_EX_MEM;
        MEM_WB = next_MEM_WB;

        return;
    }

    memory();
    execute();
    decode();
    fetch();

    IF_ID = next_IF_ID;
    ID_EX = next_ID_EX;
    EX_MEM = next_EX_MEM;
    MEM_WB = next_MEM_WB;
}

// ---------------- STAGES ----------------

void Pipeline::fetch() {
    if (pc < program.size()) {
        next_IF_ID.inst = program[pc];
        next_IF_ID.valid = true;
        pc++;
    }
}

void Pipeline::decode() {

    next_ID_EX = IF_ID;

    if (!IF_ID.valid) return;

    // 🔥 Data hazard (RAW) → stall
    if (ID_EX.valid) {
        int prev_dest = ID_EX.dest;

        if (prev_dest != -1 &&
            (prev_dest == IF_ID.inst.rs1 ||
             prev_dest == IF_ID.inst.rs2)) {

            data_stalls++;   // track data hazard

            next_ID_EX.valid = false;

            next_IF_ID = IF_ID; // freeze IF
            pc--;

            return;
        }
    }

    // normal decode
    next_ID_EX.op1 = regFile.read(IF_ID.inst.rs1);
    next_ID_EX.op2 = regFile.read(IF_ID.inst.rs2);
    next_ID_EX.dest = IF_ID.inst.rd;
}

void Pipeline::execute() {

    next_EX_MEM = ID_EX;

    if (!ID_EX.valid) return;

    int op1 = ID_EX.op1;
    int op2 = ID_EX.op2;

    // 🔥 Forwarding from EX/MEM
    if (EX_MEM.valid && EX_MEM.dest != -1) {
        if (EX_MEM.dest == ID_EX.inst.rs1)
            op1 = EX_MEM.result;

        if (EX_MEM.dest == ID_EX.inst.rs2)
            op2 = EX_MEM.result;
    }

    // 🔥 Forwarding from MEM/WB
    if (MEM_WB.valid && MEM_WB.dest != -1) {
        if (MEM_WB.dest == ID_EX.inst.rs1)
            op1 = MEM_WB.result;

        if (MEM_WB.dest == ID_EX.inst.rs2)
            op2 = MEM_WB.result;
    }

    switch (ID_EX.inst.type) {
        case ADD:
            next_EX_MEM.result = op1 + op2;
            break;

        case SUB:
            next_EX_MEM.result = op1 - op2;
            break;

        case BRANCH:
            if (op1 == 0) {
                pc = ID_EX.inst.rd;

                // flush pipeline
                next_IF_ID = {};
                next_ID_EX = {};
            }
            break;

        default:
            break;
    }
}

void Pipeline::memory() {

    next_MEM_WB = EX_MEM;

    if (!EX_MEM.valid) return;

    // 🔥 Trigger memory stall ONCE
    if (EX_MEM.inst.type == LOAD && mem_stall_cycles == 0) {
        mem_stall_cycles = 4;  // simulate miss penalty
        return;
    }

    // normal LOAD completion
    if (EX_MEM.inst.type == LOAD) {
        next_MEM_WB.result = EX_MEM.op1;
    }
}

void Pipeline::writeback() {
    if (!MEM_WB.valid) return;

    regFile.write(MEM_WB.dest, MEM_WB.result);

    instructions++;  // completed instruction
}

// ---------------- UTIL ----------------

bool Pipeline::isDone() const {
    return pc >= program.size() &&
           !IF_ID.valid &&
           !ID_EX.valid &&
           !EX_MEM.valid &&
           !MEM_WB.valid;
}

void Pipeline::printState() const {
    std::cout << "IF: " << (IF_ID.valid ? "INST" : "NOP") << " | ";
    std::cout << "ID: " << (ID_EX.valid ? "INST" : "NOP") << " | ";
    std::cout << "EX: " << (EX_MEM.valid ? "INST" : "NOP") << " | ";
    std::cout << "MEM: " << (MEM_WB.valid ? "INST" : "NOP") << "\n";
}

void Pipeline::printStats() const {

    std::cout << "\n===== PERFORMANCE STATS =====\n";

    std::cout << "Total Cycles: " << cycles << "\n";
    std::cout << "Instructions: " << instructions << "\n";

    if (instructions > 0) {
        std::cout << "IPC: " << (double)instructions / cycles << "\n";
        std::cout << "CPI: " << (double)cycles / instructions << "\n";
    }

    std::cout << "\nStalls:\n";
    std::cout << "Data Hazard Stalls: " << data_stalls << "\n";
    std::cout << "Memory Stalls: " << mem_stalls << "\n";
}
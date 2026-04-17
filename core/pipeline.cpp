#include "pipeline.h"
#include <iostream>

Pipeline::Pipeline(std::vector<Instruction> prog)
    : program(prog) {}

// ---------------- STEP ----------------

void Pipeline::step() {

    // reset next state
    next_IF_ID = {};
    next_ID_EX = {};
    next_EX_MEM = {};
    next_MEM_WB = {};

    // reverse order
    writeback();
    memory();
    execute();
    decode();
    fetch();

    // commit (clock edge)
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

    // Default: pass instruction forward
    next_ID_EX = IF_ID;

    if (!IF_ID.valid) return;

    // 🔥 Check RAW hazard with previous instruction
    if (ID_EX.valid) {

        int prev_dest = ID_EX.dest;

        if (prev_dest != -1 &&
            (prev_dest == IF_ID.inst.rs1 ||
             prev_dest == IF_ID.inst.rs2)) {

            // 🚨 Hazard detected

            // Insert bubble into EX stage
            next_ID_EX.valid = false;

            // Stall IF stage (freeze instruction)
            next_IF_ID = IF_ID;

            // Undo PC increment
            pc--;

            return;
        }
    }

    // Normal decode
    next_ID_EX.op1 = regFile.read(IF_ID.inst.rs1);
    next_ID_EX.op2 = regFile.read(IF_ID.inst.rs2);
    next_ID_EX.dest = IF_ID.inst.rd;
}

void Pipeline::execute() {

    next_EX_MEM = ID_EX;

    if (!ID_EX.valid) return;

    int op1 = ID_EX.op1;
    int op2 = ID_EX.op2;

    // 🔥 Forwarding from EX/MEM stage
    if (EX_MEM.valid && EX_MEM.dest != -1) {
        if (EX_MEM.dest == ID_EX.inst.rs1)
            op1 = EX_MEM.result;

        if (EX_MEM.dest == ID_EX.inst.rs2)
            op2 = EX_MEM.result;
    }

    // 🔥 Forwarding from MEM/WB stage
    if (MEM_WB.valid && MEM_WB.dest != -1) {
        if (MEM_WB.dest == ID_EX.inst.rs1)
            op1 = MEM_WB.result;

        if (MEM_WB.dest == ID_EX.inst.rs2)
            op2 = MEM_WB.result;
    }

    // ALU operation
    switch (ID_EX.inst.type) {
        case ADD:
            next_EX_MEM.result = op1 + op2;
            break;
        case SUB:
            next_EX_MEM.result = op1 - op2;
            break;
        case BRANCH:
            if (op1 == 0) {
                // branch taken → fix PC
                pc = ID_EX.inst.rd;  // use rd as target

                // 🔥 flush pipeline
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
}

void Pipeline::writeback() {
    if (!MEM_WB.valid) return;

    regFile.write(MEM_WB.dest, MEM_WB.result);
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
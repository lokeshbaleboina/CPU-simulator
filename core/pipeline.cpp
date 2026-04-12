#include "pipeline.h"

// Constructor
Pipeline::Pipeline(std::vector<Instruction> prog)
    : program(prog) {}

void Pipeline::step() {


    // Reset next state (VERY IMPORTANT)
    next_IF_ID = {};
    next_ID_EX = {};
    next_EX_MEM = {};
    next_MEM_WB = {};

    // Compute next state (IMPORTANT: reverse order)
    writeback();
    memory();
    execute();
    decode();
    fetch();

    // Commit (simulate clock edge)
    IF_ID = next_IF_ID;
    ID_EX = next_ID_EX;
    EX_MEM = next_EX_MEM;
    MEM_WB = next_MEM_WB;
}

// ---------------- STAGES ----------------

// IF stage
void Pipeline::fetch() {
    if (pc < program.size()) {
        next_IF_ID.inst = program[pc];
        next_IF_ID.valid = true;
        pc++;
    } else {
        next_IF_ID.valid = false;
    }
}

// ID stage
void Pipeline::decode() {
    next_ID_EX = IF_ID;
}

// EX stage
void Pipeline::execute() {
    next_EX_MEM = ID_EX;
}

// MEM stage
void Pipeline::memory() {
    next_MEM_WB = EX_MEM;
}

// WB stage
void Pipeline::writeback() {
    // nothing yet
}

// Check if simulation finished
bool Pipeline::isDone() const {
    return pc >= program.size() &&
           !IF_ID.valid &&
           !ID_EX.valid &&
           !EX_MEM.valid &&
           !MEM_WB.valid;
}
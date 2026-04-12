#pragma once
#include "pipe_reg.h"
#include <vector>

class Pipeline {
public:
    Pipeline(std::vector<Instruction> program);

    void step();        // simulate 1 clock cycle
    bool isDone() const;

private:
    std::vector<Instruction> program;
    int pc = 0;

    // pipeline registers (current state)
    PipeReg IF_ID;
    PipeReg ID_EX;
    PipeReg EX_MEM;
    PipeReg MEM_WB;

    // next state (VERY IMPORTANT)
    PipeReg next_IF_ID;
    PipeReg next_ID_EX;
    PipeReg next_EX_MEM;
    PipeReg next_MEM_WB;

    // stages
    void fetch();
    void decode();
    void execute();
    void memory();
    void writeback();
};
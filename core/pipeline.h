#pragma once
#include "pipe_reg.h"
#include "register_file.h"
#include <vector>

class Pipeline {
public:
    Pipeline(std::vector<Instruction> program);

    void step();
    bool isDone() const;

    void printState() const;

private:
    std::vector<Instruction> program;
    int pc = 0;
    
    long long cycles = 0;
    long long instructions = 0;

    long long data_stalls = 0;
    long long mem_stalls = 0;

    RegisterFile regFile;

    PipeReg IF_ID, ID_EX, EX_MEM, MEM_WB;
    PipeReg next_IF_ID, next_ID_EX, next_EX_MEM, next_MEM_WB;

    int mem_stall_cycles = 0;

    void fetch();
    void decode();
    void execute();
    void memory();
    void writeback();
};
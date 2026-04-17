#pragma once
#include "instruction.h"

struct PipeReg {
    Instruction inst;

    int op1 = 0;
    int op2 = 0;
    int result = 0;
    int dest = -1;

    bool valid = false;
};
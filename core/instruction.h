#pragma once

enum InstType {
    ADD,
    SUB,
    LOAD,
    BRANCH,
    NOP
};

struct Instruction {
    InstType type;
    int rd = -1;
    int rs1 = -1;
    int rs2 = -1;
};

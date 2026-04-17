#pragma once

enum InstType {
    ADD,
    SUB,
    NOP
};

enum InstType {
    ADD,
    SUB,
    BRANCH,
    NOP
};

struct Instruction {
    InstType type;
    int rd = -1;
    int rs1 = -1;
    int rs2 = -1;
};

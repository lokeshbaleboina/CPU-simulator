#pragma once
#include <string>

enum InstType {
    ADD,
    SUB,
    LOAD,
    STORE,
    BRANCH,
    NOP
};

struct Instruction {
    InstType type;

    int rd = -1;
    int rs1 = -1;
    int rs2 = -1;
    int imm = 0;
    int pc = 0;

    std::string toString() const {
        switch (type) {
            case ADD: return "ADD";
            case SUB: return "SUB";
            case LOAD: return "LOAD";
            case STORE: return "STORE";
            case BRANCH: return "BRANCH";
            default: return "NOP";
        }
    }
};
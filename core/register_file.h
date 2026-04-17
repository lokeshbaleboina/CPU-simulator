#pragma once
#include <vector>

class RegisterFile {
public:
    RegisterFile(int size = 32) : regs(size, 0) {}

    int read(int reg) const {
        if (reg < 0) return 0;
        return regs[reg];
    }

    void write(int reg, int value) {
        if (reg <= 0) return;
        regs[reg] = value;
    }

private:
    std::vector<int> regs;
};
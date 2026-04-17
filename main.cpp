#include "core/pipeline.h"

int main() {

    std::vector<Instruction> program = {
        {LOAD, 1, 2, -1},
        {ADD, 3, 1, 4},
        {SUB, 5, 3, 6}
    };

    Pipeline cpu(program);

    while (!cpu.isDone()) {
        cpu.printState();
        cpu.step();
    }
    cpu.printStats();
    return 0;
} 
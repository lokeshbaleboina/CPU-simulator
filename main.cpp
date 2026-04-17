#include "core/pipeline.h"

int main() {

    std::vector<Instruction> program = {
        {ADD, 1, 2, 3},
        {BRANCH, 5, 1, -1},  // if R1 == 0 → jump to inst 5
        {ADD, 4, 5, 6},      // should be flushed if taken
        {ADD, 7, 8, 9},
    };

    Pipeline cpu(program);

    while (!cpu.isDone()) {
        cpu.printState();
        cpu.step();
    }

    return 0;
}
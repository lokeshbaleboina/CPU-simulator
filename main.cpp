#include "core/pipeline.h"

int main() {

    std::vector<Instruction> program = {
        {ADD, 1, 2, 3},   // R1 = R2 + R3
        {SUB, 4, 1, 5},   // dependency (wrong for now)
        {ADD, 6, 1, 4}
    };

    Pipeline cpu(program);

    while (!cpu.isDone()) {
        cpu.printState();
        cpu.step();
    }

    return 0;
}
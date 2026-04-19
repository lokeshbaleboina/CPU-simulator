# Cycle-Accurate In-Order CPU Pipeline Simulator

This repository contains a C++ implementation of a cycle-accurate, in-order CPU pipeline simulator that models a classical 5-stage pipeline and handles hazards and memory latency.

## Overview

The simulator models a 5-stage in-order processor pipeline:
- IF: Instruction Fetch
- ID: Instruction Decode
- EX: Execute
- MEM: Memory Access
- WB: Writeback

It was designed to capture realistic microarchitectural behavior while remaining simple and easy to reason about.

## Key Features

- Cycle-accurate pipeline simulation
- Double-buffered pipeline registers to model clocked stage updates
- Data hazard detection and stall insertion
- Operand forwarding from EX/MEM and MEM/WB stages
- Control hazard handling with branch flushes
- Simplified memory system with cache hit/miss latency modeling
- Performance instrumentation for IPC, CPI, and stall breakdowns

## Architecture

### Pipeline Stages

1. **IF (Instruction Fetch)**
   - Fetches the next instruction from the program counter.
2. **ID (Instruction Decode)**
   - Reads register operands and prepares the instruction for execution.
3. **EX (Execute)**
   - Performs ALU operations and branch evaluation.
4. **MEM (Memory)**
   - Handles load operations and applies memory latency stalls.
5. **WB (Writeback)**
   - Writes results back to the register file.

### Pipeline Registers

The simulator separates stages with pipeline registers using a double-buffering model:
- `current state` represents values latched in flip-flops
- `next state` is computed during the cycle

At the end of each cycle, next-state pipeline values are committed simultaneously to emulate a clock edge.

### Execution Model

Each simulation step corresponds to one clock cycle:
1. Read current pipeline registers
2. Compute the next state for each stage
3. Commit the next state to become the current state

Stages are evaluated in reverse order (WB → IF) to avoid overwriting dependencies prematurely.

## Instruction and Register Modeling

- Instructions are represented in a decoded, high-level structure with fields for operation type and register operands.
- The register file is a 32-entry array with two read ports and one write port.
- Reads occur in ID, writes occur in WB, and register `0` is hardwired to zero.

## Hazard Handling

### Data Hazards

- The simulator detects RAW hazards in the decode stage.
- If a hazard is found and the result is not yet available, it inserts a bubble (NOP) and freezes fetch.
- This models a stall signal and pipeline hold behavior.

### Forwarding

To reduce stalls, forwarding is implemented from:
- EX/MEM → EX
- MEM/WB → EX

This allows operands to come directly from an earlier pipeline stage result instead of waiting for writeback.

### Control Hazards

- Branch conditions are evaluated in EX.
- If a branch is taken, the pipeline flushes the IF and ID stages and updates the PC.
- The current implementation uses a static "predict not taken" strategy.

## Memory and Cache Modeling

The simulator includes a simplified cache model:
- Hit latency: 1 cycle
- Miss latency: 5 cycles

Load misses trigger a global stall counter and freeze the pipeline for the configured memory latency.

## Performance Instrumentation

The simulator tracks:
- Total cycles
- Completed instructions
- IPC (Instructions Per Cycle)
- CPI (Cycles Per Instruction)
- Data hazard stalls
- Memory stalls

These metrics help identify whether workloads are compute-bound, memory-bound, or stall-limited.

## Usage

### Build

Compile with a C++17-compatible compiler. For example:

```bash
cd /Users/lokeshbaleboina/Documents/cpu-simulator
c++ -std=c++17 main.cpp core/pipeline.cpp core/register_file.cpp core/pipe_reg.cpp -o cpu_simulator
```

> Adjust the source list if there are additional implementation files under `core/`.

### Run

```bash
./cpu_simulator
```

The sample program in `main.cpp` executes a simple sequence of load and arithmetic instructions, printing pipeline state each cycle and final statistics.

## Project Structure

- `main.cpp` — entry point and example program
- `core/` — pipeline implementation and core simulation classes
- `sim/` — simulation-related helpers (if present)
- `stats/` — performance measurement and statistics
- `utils/` — utility code
- `workloads/` — example programs or workloads
- `config/` — configuration files

## Limitations and Future Work

Current limitations:
- No out-of-order execution
- No superscalar issue
- Simplified memory model
- No dynamic branch predictor

Potential extensions:
- 2-bit branch predictor
- Multi-level cache hierarchy (L1/L2)
- Instruction-level parallelism experiments
- Out-of-order execution model

## Conclusion

This simulator bridges theoretical pipeline concepts and practical microarchitectural behavior by modeling hazards, forwarding, branch handling, memory latency, and cycle-level performance metrics.

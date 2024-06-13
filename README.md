### Overview of Lab Assignments

Overview of the lab assignments for the "Computer Organization" course at Instituto Superior Técnico. These assignments are designed to give students practical experience in key areas of computer architecture, including system modeling, cache simulation, and instruction level parallelism (ILP). The assignments progressively build on each other, providing a comprehensive understanding of these concepts.

### Lab Assignment 1: System Modeling and Profiling

#### Objective:
The goal of this assignment is twofold: (i) to determine the characteristics of a computer’s caches, and (ii) to leverage the obtained knowledge about the caches in order to optimize the performance of a given program. For this task, the students will make use of a performance analysis tool to have direct access to hardware performance counters available on most modern microprocessors. The tool used is the standard Application Programming Interface (API): PAPI.

#### Procedure:
1. **Setup and Introduction to PAPI:**
   - Download and configure the PAPI library.
   - Familiarize with the lab computers' hardware and the PAPI toolkit.

2. **Cache Modeling:**
   - Determine the size, block size, and associativity of the L1 and L2 caches using small C applications.
   - Modify and analyze the provided source code to model the cache characteristics.
   - Use PAPI to measure and analyze cache performance metrics.

3. **Performance Optimization:**
   - Profile a straightforward matrix multiplication program.
   - Optimize the program by transposing the matrix and applying a blocked matrix multiplication technique.
   - Evaluate the impact of these optimizations on cache performance.

4. **Report:**
   - Document the procedures, results, and analysis.
   - Discuss the findings and the impact of cache characteristics on program performance.

### Lab Assignment 2: Cache Simulator

#### Objective:
This assignment focuses on understanding the operation and performance of cache memory. Students will implement and analyze a cache simulator to study the impact of different cache configurations on performance.

#### Procedure:
1. **Cache Simulator Implementation:**
   - Develop a simulator for different cache configurations (e.g., L1 direct-mapped with multiple lines, L2 direct-mapped, two way set-associat L2).
   - Implement various cache replacement policies (e.g., LRU, FIFO).

2. **Simulation Tasks:**
   - Run simulations with various cache sizes and associativities.
   - Measure hit rates, miss rates, and overall execution time for different configurations.

3. **Analysis and Optimization:**
   - Compare the performance of different cache configurations.
   - Identify the most efficient configuration for given workloads.

4. **Report:**
   - Document the implementation details, simulation results, and analysis.
   - Discuss the trade-offs between different cache configurations and replacement policies.

### Lab Assignment 3: Instruction Level Parallelism

#### Objective:
The main purpose of this assignment is to provide students with an in-depth understanding of pipelined computer architecture and techniques to maximize efficiency through instruction level parallelism (ILP).

#### Procedure:
1. **Introduction to WinMIPS64:**
   - Familiarize with the WinMIPS64 simulator, a tool to visualize and debug MIPS64 instruction set architecture.

2. **Simple Execution Without Data Forwarding:**
   - Execute a provided MIPS64 program in WinMIPS64 without data forwarding.
   - Analyze pipeline stages, compute CPI, and characterize branch prediction.

3. **Application of Data Forwarding Techniques:**
   - Enable data forwarding in the simulator.
   - Re-run the program, analyze the pipeline stages, and compute the improved CPI.
   - Compare the performance with and without data forwarding.

4. **Source Code Optimization:**
   - Apply re-ordering techniques to minimize data and structural hazards.
   - Implement loop unrolling to reduce control hazards.
   - Utilize the branch delay slot to optimize branch handling.

5. **Optimization Analysis:**
   - Compare the performance before and after applying optimization techniques.
   - Compute the speedup achieved through each optimization.

6. **Report:**
   - Summarize the findings, detailing the impact of each optimization on the program’s performance.
   - Discuss the importance of ILP in modern processors and the effectiveness of various optimization strategies.

### Conclusion

These lab assignments are designed to provide a comprehensive understanding of key concepts in computer architecture. By progressively building on each topic, students will gain valuable hands-on experience that will enhance their theoretical knowledge and practical skills in system modeling, cache simulation, and instruction level parallelism.
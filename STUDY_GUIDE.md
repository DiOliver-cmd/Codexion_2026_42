# Codexion Project - Study Guide

## Project Overview
Codexion is a concurrency simulation project from the 42 curriculum that models coders competing for limited USB dongles to compile quantum code. The project demonstrates mastery of POSIX threads, mutexes, condition variables, and scheduling algorithms (FIFO and EDF).

## Core Concepts Covered

### 1. POSIX Threads (pthreads)
**Theoretical Foundation:**
- Thread creation, joining, and synchronization
- Thread-safe programming patterns
- Race conditions and critical sections

**Key References:**
- "The Linux Programming Interface" by Michael Kerrisk - Chapters 29-31 (Threads)
- POSIX.1-2008 Threads Specification (IEEE Std 1003.1)
- "Programming with POSIX Threads" by David Butenhof

**Project Implementation:**
- Each coder runs in its own thread (`pthread_create`)
- Monitor thread detects burnout
- Thread cleanup with `pthread_join`

### 2. Mutexes (Mutual Exclusion)
**Theoretical Foundation:**
- Critical section protection
- Lock acquisition and release
- Deadlock prevention (lock ordering)

**Key References:**
- "Operating Systems: Three Easy Pieces" - Chapter on Locks
- "The Art of Multiprocessor Programming" by Herlihy & Shavit - Chapter 2

**Project Implementation:**
- Each dongle protected by `pthread_mutex_t`
- Log output serialized with dedicated mutex
- Simulation state protected by stop_mutex
- Consistent lock ordering (by dongle ID) prevents deadlock

### 3. Condition Variables
**Theoretical Foundation:**
- Thread waiting and signaling
- Spurious wakeups handling
- Timed waits for timeouts

**Key References:**
- "The Linux Programming Interface" - Chapter 30 (Condition Variables)
- POSIX Condition Variables Specification

**Project Implementation:**
- Each dongle has `pthread_cond_t` for waiting queue
- `pthread_cond_wait` for blocking until dongle available
- `pthread_cond_timedwait` for cooldown periods
- `pthread_cond_broadcast` for simulation shutdown

### 4. Scheduling Algorithms

#### FIFO (First In, First Out)
**Theory:** Requests served in arrival order. Simple but can cause starvation.

**Project Implementation:**
- Heap ordered by arrival timestamp
- Tie-breaker: earlier arrival wins

#### EDF (Earliest Deadline First)
**Theory:** Real-time scheduling algorithm. Task with earliest deadline executes first. Optimal for uniprocessor systems.

**Key References:**
- "Real-Time Systems" by Jane W. S. Liu - Chapter 3
- Liu & Layland (1973) "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment"

**Project Implementation:**
- Deadline = last_compile_start + time_to_burnout
- Heap ordered by deadline, then arrival time (deterministic tie-breaker)
- Guarantees no starvation under feasible parameters

### 5. Priority Queue (Binary Heap)
**Theoretical Foundation:**
- Complete binary tree with heap property
- O(log n) insertion and extraction
- Array-based implementation

**Key References:**
- "Introduction to Algorithms" (CLRS) - Chapter 6 (Heapsort)
- "Data Structures and Algorithm Analysis" by Mark Allen Weiss

**Project Implementation:**
- Custom binary heap (no stdlib priority queue)
- Supports both FIFO and EDF ordering
- Dynamic array with capacity management

### 6. Time Management
**Theoretical Foundation:**
- High-resolution timers
- Monotonic vs real-time clocks
- Precision sleep implementation

**Key References:**
- "The Linux Programming Interface" - Chapter 23 (Timers)
- POSIX `clock_gettime` and `gettimeofday` specifications

**Project Implementation:**
- `gettimeofday` for millisecond timestamps
- `nanosleep` with retry loop for precise sleeping
- Relative timestamps from simulation start

### 7. Deadlock Prevention
**Coffman Conditions (all four must hold for deadlock):**
1. Mutual Exclusion
2. Hold and Wait
3. No Preemption
4. Circular Wait

**Project Solutions:**
- **Lock Ordering:** Always acquire dongles by ID order (breaks circular wait)
- **Timeout/Cooldown:** Dongle cooldown prevents immediate re-acquisition
- **Monitor Thread:** Detects burnout (liveness property)

### 8. Starvation Prevention
**Theory:** EDF scheduling guarantees no starvation if system is feasible (utilization ≤ 1).

**Project Implementation:**
- EDF with deterministic tie-breaker (arrival time)
- Cooldown prevents busy-waiting
- Monitor ensures bounded waiting time

### 9. Memory Management
**Theoretical Foundation:**
- Dynamic allocation patterns
- Ownership and lifetime management
- Leak prevention

**Project Implementation:**
- All allocations paired with frees
- Heap structures cleaned up on simulation end
- Request objects freed after dequeuing

## Advanced Topics for Further Study

### 1. Real-Time Scheduling Theory
- Rate Monotonic Scheduling (RMS)
- Deadline Monotonic Scheduling
- Schedulability analysis (Liu & Layland bound)
- Response time analysis

**Resources:**
- "Real-Time Systems" by Jane W. S. Liu
- "Hard Real-Time Computing Systems" by Giorgio Buttazzo

### 2. Concurrency Patterns
- Readers-Writers problem
- Producer-Consumer with bounded buffer
- Barrier synchronization
- Thread pools and work stealing

**Resources:**
- "Concurrent Programming in Java" by Doug Lea (patterns applicable to C)
- "Patterns for Parallel Programming" by Mattson et al.

### 3. Lock-Free Programming
- Atomic operations (C11 `<stdatomic.h>`)
- Compare-and-swap (CAS) loops
- Memory ordering (acquire/release/seq_cst)
- ABA problem and solutions

**Resources:**
- "C++ Concurrency in Action" by Anthony Williams (principles apply to C)
- "Is Parallel Programming Hard?" by Paul McKenney

### 4. Formal Verification
- Model checking (SPIN, TLA+)
- Thread safety proofs
- Linearizability verification

**Resources:**
- "Principles of Model Checking" by Baier & Katoen
- TLA+ Video Course by Leslie Lamport

### 5. Performance Optimization
- Cache-friendly data structures
- False sharing prevention
- NUMA-aware thread placement
- Lock contention profiling

**Resources:**
- "Systems Performance" by Brendan Gregg
- "Optimizing Software in C++" by Agner Fog

## Project Architecture Summary

```
┌─────────────────────────────────────────────────────────────┐
│                        main.c                                │
│  - Argument parsing & validation                            │
│  - Simulation initialization & cleanup                      │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│   heap/       │    │   time/       │    │   coders/     │
│               │    │               │    │               │
│ heap.c        │    │ time_utils.c  │    │ coder.c       │
│ heap_ops.c    │    │ logger.c      │    │ coder_utils.c │
│               │    │               │    │ monitor.c     │
│ Priority      │    │ Timing &      │    │ dongle.c      │
│ Queue (Heap)  │    │ Logging       │    │ dongle_utils.c│
│               │    │               │    │ simulation.c  │
└───────────────┘    └───────────────┘    └───────────────┘
```

## Key Data Structures

### t_simulation
Global simulation state (no global variables per 42 norm):
- Coder array, dongle array, priority heap
- Timing parameters, scheduler type
- Synchronization primitives (mutexes, cond vars)

### t_coder
Per-coder state:
- Thread handle, ID, compile count
- Deadline tracking, dongle references

### t_dongle
Shared resource with arbitration:
- Mutex + condition variable
- Availability state, cooldown timer
- Per-dongle request heap

### t_heap / t_request
Priority queue for scheduling:
- Array-based binary heap
- Configurable comparator (FIFO/EDF)
- Request metadata (coder_id, deadline, arrival)

## Testing Checklist

### Functional Tests
- [ ] Single coder completes required compiles
- [ ] Multiple coders with FIFO scheduling
- [ ] Multiple coders with EDF scheduling
- [ ] Burnout detection within 10ms
- [ ] Cooldown enforcement
- [ ] Log serialization (no interleaved output)
- [ ] Memory leak free (valgrind clean)

### Edge Cases
- [ ] 1 coder (single dongle)
- [ ] 2 coders (deadlock-prone without ordering)
- [ ] Large number of coders (heap performance)
- [ ] Zero cooldown
- [ ] Equal deadlines (tie-breaker)
- [ ] Immediate burnout (time_to_burnout < time_to_compile)

### Stress Tests
- [ ] High contention (many coders, short times)
- [ ] Long-running simulation (memory stability)
- [ ] Rapid thread creation/destruction

## Compilation & Norm Compliance

### Build
```bash
make          # Build with -Wall -Wextra -Werror -pthread
make clean    # Remove object files
make fclean   # Remove objects and binary
make re       # Rebuild from scratch
```

### Norminette Rules (42 São Paulo)
- Maximum 5 functions per .c file
- Maximum 25 lines per function (including variable declarations)
- No global variables
- No for loops (while only)
- No variable declarations after first statement
- Standard C89/C90 compliance

### Verification
```bash
norminette Includes/ src/
valgrind --leak-check=full ./codexion 3 10000 200 200 200 3 100 fifo
```

## Further Reading & Resources

### Books
1. **"The Linux Programming Interface"** - Michael Kerrisk (Definitive Linux/UNIX system programming reference)
2. **"Operating Systems: Three Easy Pieces"** - Remzi & Andrea Arpaci-Dusseau (Free online, excellent concurrency chapters)
3. **"Real-Time Systems"** - Jane W. S. Liu (Scheduling theory)
4. **"Introduction to Algorithms"** - Cormen, Leiserson, Rivest, Stein (CLRS) (Heap data structure)
5. **"The Art of Multiprocessor Programming"** - Herlihy & Shavit (Concurrent data structures)

### Papers
- Liu & Layland (1973) "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment"
- Dijkstra (1965) "Solution of a Problem in Concurrent Programming Control" (Mutex concept)
- Lamport (1979) "How to Make a Multiprocessor Computer That Correctly Executes Multiprocess Programs"

### Online Resources
- POSIX Threads Programming Tutorial (LLNL)
- Beej's Guide to C Programming
- Linux man pages: `pthread_mutex_lock`, `pthread_cond_wait`, `gettimeofday`, `nanosleep`
- 42 Norminette documentation

### Tools for Learning
- `valgrind` / `helgrind` - Thread error detection
- `strace` - System call tracing
- `perf` - Performance profiling
- `gdb` - Thread debugging (`info threads`, `thread apply all bt`)

## AI Usage Disclosure
This project was developed with AI assistance for:
- Code structure planning and file organization
- Norminette compliance verification (function/line counting)
- Debugging deadlock and race condition issues
- Scheduling algorithm implementation guidance
- Documentation and study guide generation

All code was reviewed, tested, and understood by the developer. The AI served as a pair programming partner and reference resource, not a code generator for uncomprehended solutions.
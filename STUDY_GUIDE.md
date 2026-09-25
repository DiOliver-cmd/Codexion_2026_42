# Codexion — Academic Study Guide

*Concurrency Simulation: POSIX Threads, Scheduling Algorithms, and Synchronization Primitives*

---

## Course Information

| Field | Detail |
|-------|--------|
| **Project** | Codexion (42 Curriculum) |
| **Domain** | Systems Programming / Real-Time Concurrency |
| **Language** | C (C99, POSIX.1-2008) |
| **Prerequisites** | C pointers, memory management, basic OS concepts |
| **Estimated Total Study Time** | 25–35 hours |

---

## Concept Map & Estimated Study Time

| # | Concept | Subtopics | Est. Time | Prerequisites |
|---|---------|-----------|-----------|---------------|
| 1 | **POSIX Threads (pthreads)** | Thread lifecycle, attributes, join/detach, thread-safe design | 4–5 h | C functions, pointers |
| 2 | **Mutual Exclusion (Mutexes)** | Lock/unlock, trylock, recursive vs. fast mutex, lock hierarchies | 3–4 h | Concept 1 |
| 3 | **Condition Variables** | Wait/signal/broadcast, timed wait, spurious wakeups, predicate loops | 4–5 h | Concepts 1–2 |
| 4 | **Deadlock Theory & Prevention** | Coffman conditions, resource-allocation graphs, lock ordering, trylock backoff | 3–4 h | Concepts 2–3 |
| 5 | **Scheduling Algorithms** | FIFO vs. EDF, Liu & Layland optimality, utilization bounds, density test | 4–5 h | Discrete math, algorithms |
| 6 | **Priority Queues (Binary Heaps)** | Heap property, swim/sink, O(log n) insert/extract, heapify, array representation | 3–4 h | Data structures, arrays |
| 7 | **Real-Time Constraints** | Deadline monotonic, burnout detection, cooldown, precision timing | 2–3 h | Concepts 5–6 |
| 8 | **Memory & Resource Management** | Ownership, RAII patterns in C, leak prevention, valgrind/helgrind | 2–3 h | C malloc/free |

> **Pedagogical Note**: Concepts 1–3 form the *synchronization foundation*; 4 is the *correctness guarantee*; 5–6 are the *algorithmic core*; 7–8 are *systems integration*. Study in order.

---

## Reference Index

| # | Citation | Type | Key Chapters/Sections |
|---|----------|------|----------------------|
| **[R1]** | Kerrisk, M. *The Linux Programming Interface*. No Starch Press, 2010. | Textbook | Ch. 29–31 (Threads, Mutexes, CVs), Ch. 23 (Timers) |
| **[R2]** | Butenhof, D. *Programming with POSIX Threads*. Addison-Wesley, 1997. | Textbook | Ch. 2–5 (Mutexes, CVs, Deadlock), Ch. 8 (Performance) |
| **[R3]** | Arpaci-Dusseau, R. & A. *Operating Systems: Three Easy Pieces*. 2018. | Textbook (Free) | Ch. 26–31 (Locks, CVs, Deadlock) |
| **[R4]** | Herlihy, M. & Shavit, N. *The Art of Multiprocessor Programming*. Morgan Kaufmann, 2012. | Textbook | Ch. 2 (Mutual Exclusion), Ch. 3 (Spin Locks), Ch. 8 (Priority Queues) |
| **[R5]** | Liu, C.L. & Layland, J.W. "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment." *JACM* 20(1), 1973. | Seminal Paper | Full paper (EDF optimality, RM bound) |
| **[R6]** | Liu, J.W.S. *Real-Time Systems*. Prentice Hall, 2000. | Textbook | Ch. 3–6 (EDF, RM, Schedulability, Density) |
| **[R7]** | Coffman, E.G. et al. "System Deadlocks." *ACM Computing Surveys* 3(2), 1971. | Seminal Paper | Four necessary conditions |
| **[R8]** | Cormen, T.H. et al. *Introduction to Algorithms* (CLRS), 3rd ed. MIT Press, 2009. | Textbook | Ch. 6 (Heapsort), Ch. 19 (Binomial Heaps) |
| **[R9]** | Sedgewick, R. & Wayne, K. *Algorithms*, 4th ed. Princeton, 2011. | Textbook | §2.4 (Priority Queues, Binary Heaps) |
| **[R10]** | LLNL. *POSIX Threads Programming Tutorial*. Lawrence Livermore National Lab. | Tutorial | https://computing.llnl.gov/tutorials/pthreads/ |
| **[R11]** | Baker, T.P. "Stack-Based Scheduling of Real-Time Processes." *Advances in Real-Time Systems*, 1993. | Paper | EDF density proof |
| **[R12]** | Baruah, S. et al. "Algorithms and Complexity Concerning the Preemptive Scheduling of Periodic Real-Time Tasks." *Real-Time Systems*, 1990. | Paper | Processor demand criterion |
| **[R13]** | Dertouzos, M.L. "Control Robotics: The Procedural Control of Physical Processes." *IFAC*, 1974. | Paper | EDF optimality (exchange argument) |
| **[R14]** | Solaris/IBM. *Multithreaded Programming Guide*. Oracle/IBM Docs. | Reference | Lock hierarchies, condvar attributes |
| **[R15]** | Michael, M.M. & Scott, M.L. "Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms." *PODC*, 1996. | Paper | Lock-free alternatives (advanced) |

---

## 1. POSIX Threads — The Concurrency Foundation

### 1.1 Thread Model & Lifecycle

POSIX threads (pthreads) provide a standardized C API for shared-memory concurrency. Each thread has its own stack, register set, and thread-local storage, but shares the process's heap, file descriptors, and signal handlers.

**Key API** (from **[R1] Ch. 29**, **[R2] Ch. 2**):

```c
// Thread creation
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);

// Thread termination
void pthread_exit(void *retval);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);
```

**Codexion Application**: Each coder is a `pthread_t` created in `simulation.c:start_coders()`. The monitor thread runs `monitor_routine()`. All threads are joined in `cleanup_simulation()`.

> **Study Exercise**: Modify `coder_routine` to accept a `struct timespec` for randomized think times. Observe how thread scheduling non-determinism affects log ordering.

### 1.2 Thread Attributes & Scheduling

`pthread_attr_t` controls stack size, guard pages, scheduling policy (`SCHED_FIFO`, `SCHED_RR`, `SCHED_OTHER`), and inheritance. Codexion uses default attributes (`NULL`), meaning `SCHED_OTHER` (time-sharing) — the OS scheduler decides which thread runs. Our *application-level* scheduler (FIFO/EDF) operates *above* the OS scheduler by controlling dongle access.

> **Advanced Reading**: **[R1] §29.7** (Thread scheduling), **[R2] §3.4** (Scheduling policies). Real-time policies require `CAP_SYS_NICE` and are rarely used in user code.

---

## 2. Mutual Exclusion — Mutexes

### 2.1 Mutex Semantics

A mutex (mutual exclusion) is a synchronization primitive with two atomic operations: `lock` and `unlock`. The POSIX mutex guarantees:

1. **Mutual Exclusion**: At most one thread holds the lock.
2. **Progress**: If no thread holds the lock, a waiting thread eventually acquires it.
3. **Bounded Waiting**: No thread waits forever (assuming fair scheduling).

**Key API** (**[R1] §30.2**, **[R2] §3.2**, **[R3] Ch. 26**):

```c
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);  // Non-blocking
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

### 2.2 Mutex Types

| Type | Behavior | Use Case |
|------|----------|----------|
| `PTHREAD_MUTEX_DEFAULT` (fast) | Undefined behavior on recursive lock | Most cases |
| `PTHREAD_MUTEX_ERRORCHECK` | Returns `EDEADLK` on recursive lock | Debugging |
| `PTHREAD_MUTEX_RECURSIVE` | Allows same thread to lock multiple times | Reentrant code |
| `PTHREAD_MUTEX_NORMAL` | No error checking, no recursion | Performance-critical |

Codexion uses default (fast) mutexes. **Never** call `pthread_mutex_lock` twice on the same fast mutex from the same thread — it deadlocks.

### 2.3 Lock Hierarchies — Preventing Deadlock by Design

The single most effective deadlock prevention technique is **global lock ordering** (**[R7]**, **[R14]**, **[R3] Ch. 30**).

**Theorem**: If all threads acquire multiple locks in a globally consistent total order, circular wait (Coffman condition #4) is impossible.

**Proof Sketch**: A cycle in the wait-for graph requires some thread to hold lock `L_i` and wait for `L_j` where `j < i` (backwards edge). If all threads only acquire locks in increasing order, no backwards edges exist → no cycles.

**Codexion Implementation** (`coder_utils.c:acquire_dongles`):

```c
void acquire_dongles(t_coder *coder)
{
    if (coder->sim->num_coders == 1) { /* single dongle */ }
    // Global order: lower dongle ID first
    if (coder->left_dongle->id < coder->right_dongle->id)
        acquire_first_dongle(coder, coder->left_dongle, coder->right_dongle);
    else
        acquire_first_dongle(coder, coder->right_dongle, coder->left_dongle);
}
```

**Why this works**: With N coders in a circle, coder `i` needs dongles `i` and `(i+1)%N`. Without ordering, coder 1 holds 0→waits for 1, coder 2 holds 1→waits for 2, ..., coder N holds N-1→waits for 0: **cycle**. By always taking the lower ID first, we impose a total order `0 < 1 < ... < N-1`. The cycle is broken.

> **Critical Insight**: Lock ordering works *only if every code path obeys it*. A single stray function that locks in reverse order reintroduces the bug. Route all multi-lock acquisition through a single helper (`acquire_first_dongle`).

### 2.4 Alternative: `pthread_mutex_trylock` Backoff

When global ordering is impractical (e.g., locks chosen at runtime from a large set), use **trylock with backoff** (**[R2] §3.3**, **[R14]**):

```c
// Lock first mutex normally
pthread_mutex_lock(&m1);
// Try second; if busy, release first and retry
if (pthread_mutex_trylock(&m2) == EBUSY) {
    pthread_mutex_unlock(&m1);
    // Optional: exponential backoff
    goto retry;
}
```

**Pitfall**: Forgetting to release the first lock on `EBUSY` leaks the lock — the most common trylock bug.

---

## 3. Condition Variables — Efficient Waiting

### 3.1 The Problem with Busy Waiting

A naive waiter might spin:

```c
while (!condition) { /* busy wait */ }
```

This wastes CPU cycles. Condition variables (CVs) allow threads to **sleep** until signaled.

### 3.2 CV Mechanics

A CV is **always paired with a mutex**. The canonical pattern (**[R1] §30.3**, **[R2] §3.5**, **[R3] Ch. 28**):

```c
// Waiter
pthread_mutex_lock(&mutex);
while (!predicate) {
    pthread_cond_wait(&cond, &mutex);  // Atomically unlocks mutex + sleeps
}
// predicate now true, mutex held
pthread_mutex_unlock(&mutex);

// Signaler
pthread_mutex_lock(&mutex);
predicate = true;
pthread_cond_signal(&cond);  // or broadcast
pthread_mutex_unlock(&mutex);
```

**Why the `while` loop?** Spurious wakeups: `pthread_cond_wait` may return without a signal. The predicate re-check handles this.

**Why mutex locked before wait?** The wait atomically unlocks the mutex and blocks. Without the mutex, a signal could occur between checking the predicate and calling wait — the signal would be lost.

### 3.3 Timed Waits — Cooldown Implementation

`pthread_cond_timedwait` accepts an **absolute timeout** (`struct timespec`). Codexion uses this for dongle cooldown (`dongle_utils.c:wait_for_cooldown`):

```c
void wait_for_cooldown(t_dongle *dongle)
{
    struct timespec ts;
    long now = get_current_time_ms();
    if (now >= dongle->cooldown_end) return;
    long wait_ms = dongle->cooldown_end - now;
    ts.tv_sec = wait_ms / 1000;
    ts.tv_nsec = (wait_ms % 1000) * 1000000;
    pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}
```

**Why absolute time?** POSIX specifies absolute timeout to avoid drift from repeated relative waits. The caller must compute `now + wait_ms` each call.

### 3.4 Signal vs. Broadcast

| Function | Wakes | Use When |
|----------|-------|----------|
| `pthread_cond_signal` | At least one waiter | Single resource, one waiter can proceed |
| `pthread_cond_broadcast` | All waiters | State change affects all (e.g., shutdown) |

Codexion uses `signal` for dongle release (one waiter gets it) and `broadcast` for simulation shutdown (`simulation.c:broadcast_all_dongles`).

---

## 4. Deadlock Theory — Formal Foundations

### 4.1 The Coffman Conditions ([R7])

Deadlock occurs **iff** all four hold simultaneously:

| # | Condition | Formal Statement |
|---|-----------|------------------|
| 1 | **Mutual Exclusion** | ∃ resource R: only one thread may hold R at a time |
| 2 | **Hold and Wait** | ∃ thread T holding R₁ while waiting for R₂ |
| 3 | **No Preemption** | Resources cannot be forcibly taken from holders |
| 4 | **Circular Wait** | ∃ cycle T₀→R₁→T₁→R₂→...→Tₙ→R₀→T₀ in wait-for graph |

These are **necessary but not sufficient** — all four can hold without deadlock (e.g., timing prevents cycle formation), but deadlock *cannot* occur if any is missing.

### 4.2 Resource-Allocation Graphs (RAG)

A RAG is a directed graph with:
- **Process nodes** (circles)
- **Resource nodes** (squares)
- **Request edges** P→R (P waits for R)
- **Assignment edges** R→P (R held by P)

**Theorem** (**[R1] §8.3.2**): A cycle in the RAG is **necessary** for deadlock. If all resources have single instances, a cycle is also **sufficient**.

### 4.3 Prevention Strategies (Eliminate One Condition)

| Strategy | Condition Broken | Practicality |
|----------|------------------|--------------|
| Make all resources shareable | Mutual Exclusion | Rarely possible (writes need exclusion) |
| Acquire all locks atomically / release before requesting | Hold and Wait | Impractical for dynamic needs |
| Allow preemption (trylock + backoff) | No Preemption | Complex, livelock risk |
| **Global lock ordering** | **Circular Wait** | **Best practical choice** |

Codexion uses **lock ordering** (Condition 4). See §2.3.

### 4.4 Detection & Recovery (When Prevention Fails)

If prevention is impossible, systems use:
- **Detection**: Periodic RAG cycle detection (O(n²) for single-instance resources)
- **Recovery**: Victim selection → kill thread → release locks → restart

**Codexion Note**: The monitor thread *detects* burnout (liveness failure), not deadlock. Our lock ordering makes deadlock impossible by construction.

---

## 5. Scheduling Algorithms — FIFO vs. EDF

### 5.1 Problem Statement

We have N coders competing for N dongles. Each coder needs 2 adjacent dongles. When multiple coders wait for the same dongle, the **scheduler** decides who gets it.

### 5.2 FIFO (First In, First Out)

**Policy**: Requests served in arrival order.

**Properties**:
- Simple, fair in arrival order
- No starvation if system stable
- No priority inversion

**Codexion Implementation** (`heap_ops.c:compare_requests`):

```c
if (type == SCHED_TYPE_FIFO) {
    if (a->arrival_time < b->arrival_time) return -1;
    if (a->arrival_time > b->arrival_time) return 1;
    return 0;
}
```

### 5.3 EDF (Earliest Deadline First) — Optimal Dynamic Scheduling

**Policy**: Request with earliest absolute deadline executes first.

**Theoretical Foundation** (**[R5]**, **[R6] Ch. 4**, **[R13]**):

**Theorem (Liu & Layland, 1973; Dertouzos, 1974)**: For independent, preemptible jobs on a single processor, EDF is **optimal** — if *any* algorithm can schedule a task set, EDF can.

**Proof Sketch (Exchange Argument)**: Take any feasible schedule. Find the first point where it deviates from EDF (job A runs but job B has earlier deadline). Swap A and B. The swap cannot cause a deadline miss because B's deadline is earlier. Repeat until schedule matches EDF.

**Schedulability Test** (Implicit deadlines, D = T):

```
U = Σ(Cᵢ / Tᵢ) ≤ 1   ⟺   EDF feasible
```

Where Cᵢ = execution time, Tᵢ = period.

**Constrained Deadlines (D ≤ T)**: Use **density** δᵢ = Cᵢ / min(Dᵢ, Tᵢ). Sufficient condition: Σδᵢ ≤ 1 (**[R6] §4.5**, **[R11]**).

**Codexion Deadline Model**: Each coder's deadline = `last_compile_start + time_to_burnout`. This is a **relative deadline** from the last compile. The scheduler uses absolute deadlines for comparison.

**Codexion EDF Comparator** (`heap_ops.c`):

```c
// EDF: earliest deadline first
if (a->deadline < b->deadline) return -1;
if (a->deadline > b->deadline) return 1;
// Tie-breaker: arrival time for determinism
if (a->arrival_time < b->arrival_time) return -1;
if (a->arrival_time > b->arrival_time) return 1;
return 0;
```

**Why the tie-breaker?** Millisecond precision means equal deadlines can occur. Arrival time ensures deterministic, reproducible behavior — critical for grading and debugging.

### 5.4 EDF in Codexion — Special Considerations

Codexion's "jobs" are **dongle acquisition requests**, not CPU tasks. The scheduler arbitrates *access to dongles*, not CPU time. However, the same optimality principle applies: serving the coder closest to burnout first maximizes the chance all coders meet their deadlines.

**Feasibility Condition for Codexion**: The system is feasible if the total "compile demand" fits within the burnout window. Roughly:

```
N * time_to_compile / time_to_burnout ≤ 1   (for 2 dongles per coder, more complex)
```

Exact analysis requires processor-demand criterion (**[R12]**).

---

## 6. Priority Queues — Binary Heap Implementation

### 6.1 Abstract Data Type

A priority queue supports:
- `insert(key, priority)` — O(log n)
- `extract_max()` / `extract_min()` — O(log n)
- `peek()` — O(1)
- `size()`, `is_empty()` — O(1)

### 6.2 Binary Heap Properties

A **binary heap** is a complete binary tree satisfying the **heap property**:

- **Min-heap**: `parent.key ≤ child.key` (root = minimum)
- **Max-heap**: `parent.key ≥ child.key` (root = maximum)

**Array Representation** (**[R8] Ch. 6**, **[R9] §2.4**):

```
Index:     1   2   3   4   5   6   7
Tree:      A
          / \
         B   C
        / \ / \
       D  E F  G
```

For node at index `k` (1-based):
- Parent: `k/2`
- Left child: `2k`
- Right child: `2k+1`

Codexion uses 0-based indexing (`heap.c`, `heap_ops.c`):
- Parent: `(k-1)/2`
- Left: `2k+1`
- Right: `2k+2`

### 6.3 Core Operations

**Swim (Percolate Up)** — after insert at end:

```c
void heapify_up(t_heap *heap, int idx)
{
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (compare(heap->data[idx], heap->data[parent], heap->type) >= 0)
            break;
        swap(heap->data[idx], heap->data[parent]);
        idx = parent;
    }
}
```

**Sink (Percolate Down)** — after extract (root replaced by last element):

```c
void heapify_down(t_heap *heap, int idx)
{
    while (1) {
        int smallest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        if (left < heap->size && compare(heap->data[left], heap->data[smallest], heap->type) < 0)
            smallest = left;
        if (right < heap->size && compare(heap->data[right], heap->data[smallest], heap->type) < 0)
            smallest = right;
        if (smallest == idx) break;
        swap(heap->data[idx], heap->data[smallest]);
        idx = smallest;
    }
}
```

**Complexity**: Both O(log n) — height of complete binary tree is ⌊log₂ n⌋.

### 6.4 Codexion's Heap Design

- **Per-dongle heaps**: Each dongle has its own request queue (`t_dongle.heap`). This avoids global lock contention.
- **Comparator parameterized by scheduler**: Same heap structure, different `compare_requests` function.
- **Dynamic capacity**: Created with `num_coders * 10` capacity (sufficient for all waiters).

> **Study Exercise**: Implement `heap_decrease_key` (for priority updates) and analyze its use in Dijkstra's algorithm.

---

## 7. Real-Time Constraints — Precision & Liveness

### 7.1 Time Measurement

Codexion uses `gettimeofday()` for millisecond timestamps (**[R1] §23.4**):

```c
long get_current_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
```

**Why not `clock_gettime(CLOCK_MONOTONIC)`?** `gettimeofday` is simpler and sufficient. `CLOCK_MONOTONIC` is immune to NTP adjustments but requires `librt` on some systems. The subject permits `gettimeofday`.

### 7.2 Precision Sleep

`nanosleep` with retry loop handles `EINTR` (signal interruption):

```c
void precise_sleep(long ms)
{
    struct timespec req = { ms / 1000, (ms % 1000) * 1000000 };
    struct timespec rem;
    while (nanosleep(&req, &rem) == -1)
        req = rem;
}
```

**Why retry?** If a signal arrives during sleep, `nanosleep` returns -1 with `errno=EINTR` and writes remaining time to `rem`. The loop continues sleeping the remainder.

### 7.3 Burnout Detection — Liveness Monitor

The monitor thread checks deadlines every 1 ms (`usleep(1000)`). This guarantees burnout logged within 10 ms of actual deadline.

**Correctness Argument**:
- Monitor period = 1 ms
- Worst case: deadline passes just after check → detected at next check (≤1 ms later)
- Logging adds negligible overhead (mutex + printf)
- Total ≤ 10 ms requirement satisfied

**Codexion Implementation** (`monitor.c:monitor_routine`):

```c
while (1) {
    pthread_mutex_lock(&sim->stop_mutex);
    if (sim->stop_simulation) { pthread_mutex_unlock(&sim->stop_mutex); break; }
    pthread_mutex_unlock(&sim->stop_mutex);
    
    long current_time = get_timestamp_ms(sim);
    for (int i = 0; i < sim->num_coders; i++) {
        if (current_time >= sim->coders[i].deadline) {
            log_state(sim, sim->coders[i].id, STATE_BURNED_OUT);
            // ... stop simulation
            return NULL;
        }
    }
    usleep(1000);
}
```

### 7.4 Cooldown — Resource Protection

After release, a dongle is unavailable for `dongle_cooldown` ms. This models physical dongle reconnection time and prevents immediate re-acquisition by the same coder (which could starve others).

**Implementation**: `cooldown_end` timestamp + `pthread_cond_timedwait` (see §3.3).

---

## 8. Memory & Resource Management

### 8.1 Ownership Model

| Resource | Owner | Lifetime |
|----------|-------|----------|
| `t_coder[]` array | `t_simulation` | Simulation duration |
| `t_dongle[]` array | `t_simulation` | Simulation duration |
| `t_heap` (per dongle) | `t_dongle` | Dongle lifetime |
| `t_request` nodes | `t_heap` | Enqueue → dequeue |
| Thread stacks | OS | Thread lifetime |

### 8.2 Cleanup Protocol

`cleanup_simulation()` (**[R2] §2.4**):

1. Join all coder threads (wait for termination)
2. Free coder array
3. Destroy all dongles (mutex, condvar, heap)
4. Destroy simulation mutexes/condvars

**No memory leaks**: Every `malloc` has matching `free`. Every `pthread_mutex_init` has `pthread_mutex_destroy`. Every `pthread_cond_init` has `pthread_cond_destroy`.

### 8.3 Verification Tools

```bash
# Memory leaks
valgrind --leak-check=full --show-leak-kinds=all ./codexion ...

# Thread errors (data races, deadlocks)
valgrind --tool=helgrind ./codexion ...

# Thread sanitizer (compile-time)
cc -fsanitize=thread -g -O1 ... -o codexion_tsan
./codexion_tsan ...
```

---

## 9. Codexion Architecture Deep Dive

### 9.1 Data Structures

```c
// Core types (types.h)
typedef enum { SCHED_TYPE_FIFO, SCHED_TYPE_EDF } t_scheduler;

typedef struct s_request {
    int coder_id;
    long deadline;       // For EDF
    long arrival_time;   // For FIFO / tie-break
    struct s_request *next;
} t_request;

typedef struct s_heap {
    t_request **data;
    int size, capacity;
    t_scheduler type;
} t_heap;

typedef struct s_dongle {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool available;
    long cooldown_end;
    int id;
    t_heap *heap;        // Per-dongle request queue
} t_dongle;

typedef struct s_coder {
    int id;
    pthread_t thread;
    int compile_count;
    long last_compile_start;
    long deadline;       // last_compile + time_to_burnout
    t_dongle *left_dongle, *right_dongle;
    struct s_simulation *sim;
} t_coder;

typedef struct s_simulation {
    int num_coders;
    long time_to_burnout, time_to_compile, time_to_debug, time_to_refactor;
    int compiles_required;
    long dongle_cooldown;
    t_scheduler scheduler;
    t_coder *coders;
    t_dongle *dongles;
    pthread_mutex_t log_mutex;
    pthread_mutex_t stop_mutex;
    pthread_cond_t  stop_cond;
    bool stop_simulation;
    long start_time;
    int finished_coders;
} t_simulation;
```

### 9.2 Thread Communication Flow

```
┌─────────────────────────────────────────────────────────────────────┐
│                        SIMULATION (main thread)                     │
│  init_simulation() → start_coders() → start_monitor()              │
│                           │                    │                    │
│                           ▼                    ▼                    │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ CODER THREADS (N threads)              MONITOR THREAD       │   │
│  │ coder_routine()                          monitor_routine()  │   │
│  │                                           │                 │   │
│  │ acquire_dongles() ──────────────────────► │ (checks deadlines)│
│  │   wait_for_dongle()                       │                 │   │
│  │     lock dongle mutex                     │                 │   │
│  │     enqueue request                       │                 │   │
│  │     cond_wait / timed_wait                │                 │   │
│  │     acquire dongle                        │                 │   │
│  │ release_dongles()                         │                 │   │
│  │   set available=true                      │                 │   │
│  │   set cooldown_end                        │                 │   │
│  │   cond_signal                             │                 │   │
│  │                                           │                 │   │
│  │ log_state() ◄─────────────────────────────┤ (logs burnout)  │
│  │   lock log_mutex                          │                 │   │
│  │   printf                                  │                 │   │
│  │   unlock log_mutex                        │                 │   │
│  └─────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
```

### 9.3 Critical Section Analysis

| Critical Section | Protected By | Max Hold Time |
|------------------|--------------|---------------|
| Dongle state (avail, cooldown, heap) | `dongle->mutex` | O(log n) heap ops |
| Log output | `sim->log_mutex` | printf time |
| Stop flag | `sim->stop_mutex` | Few instructions |
| Coder deadline update | None (thread-local) | N/A |

**No lock nesting** between dongle mutex and stop mutex — they are acquired in separate code regions. This avoids deadlock.

---

## 10. Advanced Topics for Further Study

### 10.1 Real-Time Scheduling Theory
- Rate Monotonic (RM) vs. Deadline Monotonic (DM) — fixed priority
- Response Time Analysis (RTA) for FP scheduling
- Multiprocessor EDF (partitioned, global, semi-partitioned)
- Scheduling servers (deferrable, sporadic, constant bandwidth)

**Sources**: **[R6] Ch. 7–10**, **[R12]**, **[R15]**

### 10.2 Lock-Free & Wait-Free Programming
- Atomic operations (C11 `<stdatomic.h>`)
- Compare-and-swap (CAS) loops
- Memory ordering (acquire/release/seq_cst)
- ABA problem, hazard pointers, epoch-based reclamation

**Sources**: **[R4] Ch. 10–11**, **[R15]**, Michael & Scott (1996)

### 10.3 Formal Verification
- Model checking (SPIN/Promela, TLA+)
- Linearizability proofs
- Rely-guarantee reasoning
- Owicki-Gries method

**Sources**: Baier & Katoen *Principles of Model Checking*, Lamport *Specifying Systems*

### 10.4 Performance Engineering
- False sharing (cache line ping-pong)
- NUMA-aware thread placement
- Lock contention profiling (`perf`, `VTune`)
- Scalable synchronization (MCS locks, CLH locks)

**Sources**: **[R1] Ch. 6**, McKenney *Is Parallel Programming Hard?*

---

## Verification Checklist

### Functional Correctness
- [ ] Single coder completes required compiles without burnout
- [ ] Multiple coders with FIFO scheduling — no deadlock, fair ordering
- [ ] Multiple coders with EDF scheduling — earliest deadline served first
- [ ] Burnout detected and logged within 10 ms of deadline
- [ ] Dongle cooldown enforced (no immediate re-acquisition)
- [ ] Log lines never interleaved (serialized output)
- [ ] Simulation stops when all coders meet compile target
- [ ] Simulation stops on first burnout

### Edge Cases
- [ ] 1 coder (single dongle, self-loop)
- [ ] 2 coders (minimal deadlock-prone configuration)
- [ ] Large N (heap performance, contention)
- [ ] Zero cooldown
- [ ] Equal deadlines (tie-breaker determinism)
- [ ] `time_to_burnout < time_to_compile` (immediate burnout)

### Stress & Robustness
- [ ] High contention (many coders, short times)
- [ ] Long-running simulation (memory stability, no leaks)
- [ ] Rapid thread creation/destruction
- [ ] Signal interruption during sleep (`nanosleep` retry)

### Norminette Compliance (42 São Paulo)
- [ ] ≤ 5 functions per `.c` file
- [ ] ≤ 25 lines per function (including variable declarations)
- [ ] No global variables
- [ ] No `for` loops (only `while`)
- [ ] No variable declarations after first statement
- [ ] Standard C89/C90 compliance

### Tool Verification
```bash
# Compile with required flags
make re

# Norminette
norminette Includes/ src/

# Memory safety
valgrind --leak-check=full --error-exitcode=1 ./codexion 3 10000 200 200 200 3 100 fifo

# Thread safety
valgrind --tool=helgrind --error-exitcode=1 ./codexion 3 10000 200 200 200 3 100 edf
```

---

## Appendix: Quick Reference — POSIX API Used

| Function | Header | Purpose |
|----------|--------|---------|
| `pthread_create` | `<pthread.h>` | Create thread |
| `pthread_join` | `<pthread.h>` | Wait for thread termination |
| `pthread_mutex_init` | `<pthread.h>` | Initialize mutex |
| `pthread_mutex_lock` | `<pthread.h>` | Acquire mutex (blocking) |
| `pthread_mutex_trylock` | `<pthread.h>` | Acquire mutex (non-blocking) |
| `pthread_mutex_unlock` | `<pthread.h>` | Release mutex |
| `pthread_mutex_destroy` | `<pthread.h>` | Destroy mutex |
| `pthread_cond_init` | `<pthread.h>` | Initialize condition variable |
| `pthread_cond_wait` | `<pthread.h>` | Wait on CV (unlocks mutex) |
| `pthread_cond_timedwait` | `<pthread.h>` | Wait on CV with timeout |
| `pthread_cond_signal` | `<pthread.h>` | Wake one waiter |
| `pthread_cond_broadcast` | `<pthread.h>` | Wake all waiters |
| `pthread_cond_destroy` | `<pthread.h>` | Destroy CV |
| `gettimeofday` | `<sys/time.h>` | Wall-clock time (µs) |
| `nanosleep` | `<time.h>` | High-resolution sleep |
| `usleep` | `<unistd.h>` | Microsecond sleep |
| `malloc`/`free` | `<stdlib.h>` | Dynamic memory |
| `printf`/`fprintf` | `<stdio.h>` | Formatted output |
| `strcmp`/`strlen` | `<string.h>` | String operations |
| `atoi`/`atol` | `<stdlib.h>` | String to integer |
| `memset` | `<string.h>` | Memory fill |

---

*This guide is intended as a companion to the Codexion project. Mastery comes from implementing, breaking, and fixing — not just reading. Compile with `-Wall -Wextra -Werror -pthread`, run under `valgrind` and `helgrind`, and trace every execution path.*

**— Prof. Systems Programming**
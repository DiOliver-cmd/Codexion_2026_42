*This project has been created as part of the 42 curriculum by dilferre*

# Codexion

## Description

Codexion is a concurrency simulation project that models a shared co-working hub where multiple coders compete for limited USB dongles to compile quantum code. The project demonstrates mastery of POSIX threads, mutexes, condition variables, and scheduling algorithms (FIFO and EDF).

### Project Goal

Simulate `N` coders sitting in a circle, each needing two adjacent dongles (left and right) to compile. Coders cycle through three phases:
1. **Compile** — Hold two dongles for `time_to_compile` ms
2. **Debug** — Release dongles, debug for `time_to_debug` ms
3. **Refactor** — Refactor for `time_to_refactor` ms, then attempt to compile again

The simulation stops when either:
- A coder **burns out** (fails to start compiling within `time_to_burnout` ms since last compile)
- All coders have compiled at least `number_of_compiles_required` times

### Key Features

- **Two scheduling policies**: FIFO (First In, First Out) and EDF (Earliest Deadline First)
- **Dongle cooldown**: After release, a dongle is unavailable for `dongle_cooldown` ms
- **Deadlock prevention**: Consistent lock ordering by dongle ID
- **Precise burnout detection**: Monitor thread logs burnout within 10 ms
- **Thread-safe logging**: Serialized output with mutex protection
- **Custom priority queue**: Binary heap implementation (no stdlib containers)

## Instructions

### Compilation

```bash
make        # Build the project
make clean  # Remove object files
make fclean # Remove objects and binary
make re     # Rebuild from scratch
```

The Makefile compiles with `-Wall -Wextra -Werror -pthread` as required.

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

**Arguments (all mandatory):**
| Argument | Description |
|----------|-------------|
| `number_of_coders` | Number of coders (and dongles) |
| `time_to_burnout` | Max ms between compiles before burnout |
| `time_to_compile` | Ms to compile (holding 2 dongles) |
| `time_to_debug` | Ms to debug |
| `time_to_refactor` | Ms to refactor |
| `number_of_compiles_required` | Target compiles per coder to stop |
| `dongle_cooldown` | Ms dongle unavailable after release |
| `scheduler` | `fifo` or `edf` |

### Examples

**Example 1: Basic FIFO with 3 coders**
```bash
./codexion 3 10000 200 200 200 3 100 fifo
```
Output:
```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
300 3 has taken a dongle
300 3 has taken a dongle
300 3 is compiling
306 2 has taken a dongle
400 1 is refactoring
500 3 is debugging
600 2 has taken a dongle
600 2 is compiling
600 1 has taken a dongle
700 3 is refactoring
800 2 is debugging
900 1 has taken a dongle
900 1 is compiling
...
```

**Example 2: EDF scheduling with burnout**
```bash
./codexion 3 500 200 200 200 10 100 edf
```
Output:
```
0 1 has taken a dongle
0 2 has taken a dongle
0 2 has taken a dongle
0 2 is compiling
200 2 is debugging
300 1 has taken a dongle
300 1 is compiling
400 2 is refactoring
500 1 is debugging
501 2 burned out
501 3 has taken a dongle
501 3 has taken a dongle
501 3 is compiling
700 1 is refactoring
701 3 is debugging
901 3 is refactoring
```

**Example 3: Single coder (edge case)**
```bash
./codexion 1 5000 200 200 200 3 100 fifo
```
Output:
```
0 1 has taken a dongle
0 1 is compiling
201 1 is debugging
401 1 is refactoring
601 1 has taken a dongle
601 1 is compiling
801 1 is debugging
1001 1 is refactoring
1201 1 has taken a dongle
1201 1 is compiling
1402 1 is debugging
1602 1 is refactoring
```

## Resources

### Theoretical References

1. **POSIX Threads Programming**
   - *The Linux Programming Interface* by Michael Kerrisk — Chapters 29-31 (Threads, Mutexes, Condition Variables)
   - POSIX.1-2008 Threads Specification (IEEE Std 1003.1)
   - *Programming with POSIX Threads* by David Butenhof

2. **Scheduling Algorithms**
   - Liu & Layland (1973) "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment" — EDF optimality proof
   - *Real-Time Systems* by Jane W. S. Liu — Chapter 3 (EDF, RMS, schedulability analysis)
   - *Hard Real-Time Computing Systems* by Giorgio Buttazzo

3. **Data Structures**
   - *Introduction to Algorithms* (CLRS) — Chapter 6 (Binary Heaps, Heapsort)
   - *Data Structures and Algorithm Analysis* by Mark Allen Weiss

4. **Concurrency Theory**
   - *Operating Systems: Three Easy Pieces* by Remzi & Andrea Arpaci-Dusseau — Locks, Condition Variables chapters
   - *The Art of Multiprocessor Programming* by Herlihy & Shavit — Chapter 2 (Mutual Exclusion)
   - Coffman et al. (1971) "System Deadlocks" — Four necessary conditions for deadlock

5. **Time Management**
   - *The Linux Programming Interface* — Chapter 23 (Timers, `gettimeofday`, `clock_gettime`, `nanosleep`)

### AI Usage

AI assistance was used for:
- Code structure planning and file organization
- Norminette compliance verification (function/line counting)
- Debugging deadlock and race condition issues
- Scheduling algorithm implementation guidance
- Documentation and study guide generation

All code was reviewed, tested, and understood by the developer. The AI served as a pair programming partner and reference resource, not a code generator for uncomprehended solutions.

## Blocking Cases Handled

### 1. Deadlock Prevention (Coffman's Conditions)

The four Coffman conditions for deadlock are:
1. **Mutual Exclusion** — Dongles are exclusive resources ✓
2. **Hold and Wait** — Coders hold one dongle while waiting for another ✓
3. **No Preemption** — Dongles cannot be forcibly taken ✓
4. **Circular Wait** — **BROKEN** by consistent lock ordering

**Solution**: Always acquire dongles in ascending ID order. This eliminates circular wait, making deadlock impossible.

```c
// src/coders/coder_utils.c
void acquire_dongles(t_coder *coder)
{
    if (coder->sim->num_coders == 1)
    {
        wait_for_dongle(coder->left_dongle, coder);
        log_state(coder->sim, coder->id, STATE_TAKEN_DONGLE);
        return;
    }
    // Lock ordering: lower ID first prevents circular wait
    if (coder->left_dongle->id < coder->right_dongle->id)
        acquire_first_dongle(coder, coder->left_dongle, coder->right_dongle);
    else
        acquire_first_dongle(coder, coder->right_dongle, coder->left_dongle);
}
```

**What this code does**: When a coder needs both dongles, it doesn't just grab them in left-then-right order. Instead, it compares the dongle IDs and always acquires the lower-ID dongle first. With 3 coders in a circle (dongles 0, 1, 2), coder 1 needs dongles 0 and 1, coder 2 needs 1 and 2, coder 3 needs 2 and 0. Without ordering, coder 1 could hold 0 and wait for 1, coder 2 holds 1 and waits for 2, coder 3 holds 2 and waits for 0 — classic circular wait. By always taking the lower ID first, we guarantee a global ordering that prevents cycles.

### 2. Starvation Prevention

**FIFO**: Requests served in arrival order. No starvation if system is stable.

**EDF**: Earliest deadline first. Optimal for uniprocessor real-time systems. With deterministic tie-breaker (arrival time), no starvation under feasible parameters (utilization ≤ 1).

```c
// src/heap/heap_ops.c
int compare_requests(t_request *a, t_request *b, t_scheduler type)
{
    if (type == SCHED_TYPE_FIFO)
    {
        if (a->arrival_time < b->arrival_time) return -1;
        if (a->arrival_time > b->arrival_time) return 1;
        return 0;
    }
    // EDF: earliest deadline first
    if (a->deadline < b->deadline) return -1;
    if (a->deadline > b->deadline) return 1;
    // Tie-breaker: arrival time for determinism
    if (a->arrival_time < b->arrival_time) return -1;
    if (a->arrival_time > b->arrival_time) return 1;
    return 0;
}
```

**What this code does**: This is the comparator function for the binary heap that implements the priority queue. For FIFO, it simply compares arrival timestamps — earlier arrival wins. For EDF, it compares deadlines (last_compile_start + time_to_burnout) — the coder closest to burning out gets priority. The tie-breaker using arrival time ensures deterministic behavior even when two coders have exactly the same deadline (which can happen due to millisecond precision). This comparator is used by `heapify_up` and `heapify_down` to maintain the heap property after insertions and removals.

### 3. Cooldown Handling

After release, dongle enters cooldown period. The releasing thread marks the dongle as available but sets a cooldown timestamp. Waiting threads check this timestamp and use timed waits instead of busy-polling.

```c
// src/coders/coder_utils.c
void release_dongles(t_coder *coder)
{
    long now = get_current_time_ms();
    pthread_mutex_lock(&coder->left_dongle->mutex);
    coder->left_dongle->available = true;
    coder->left_dongle->cooldown_end = now + coder->sim->dongle_cooldown;
    if (coder->left_dongle->heap->size > 0)
        pthread_cond_signal(&coder->left_dongle->cond);
    pthread_mutex_unlock(&coder->left_dongle->mutex);
    // ... same for right dongle
}
```

**What this code does**: When a coder finishes compiling, it releases both dongles. For each dongle, it acquires the dongle's mutex, marks it as available, calculates when the cooldown expires (`now + dongle_cooldown`), and signals the condition variable if there are waiters. The mutex ensures that the state change (available + cooldown_end) is atomic — no waiter can see an inconsistent state.

Waiters use `pthread_cond_timedwait` to sleep until cooldown expires:

```c
// src/coders/dongle_utils.c
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

**What this code does**: Instead of spinning in a loop checking the cooldown (which wastes CPU), the waiter calculates exactly how long until the cooldown ends, converts that to a `timespec`, and calls `pthread_cond_timedwait`. This atomically releases the mutex and sleeps until either the timeout expires or another thread signals the condition variable. When it wakes up, it re-acquires the mutex and re-checks the condition. This is far more efficient than busy-waiting with `usleep`.

### 4. Precise Burnout Detection

A dedicated monitor thread wakes up every 1 ms and checks if any coder has missed their deadline. This ensures burnout is logged within 10 ms of the actual deadline.

```c
// src/coders/monitor.c
void *monitor_routine(void *arg)
{
    t_simulation *sim = (t_simulation *)arg;
    while (1)
    {
        pthread_mutex_lock(&sim->stop_mutex);
        if (sim->stop_simulation) { pthread_mutex_unlock(&sim->stop_mutex); break; }
        pthread_mutex_unlock(&sim->stop_mutex);
        
        long current_time = get_timestamp_ms(sim);
        int i = 0;
        while (i < sim->num_coders)
        {
            if (current_time >= sim->coders[i].deadline)
            {
                log_state(sim, sim->coders[i].id, STATE_BURNED_OUT);
                // ... stop simulation
                return NULL;
            }
            i++;
        }
        usleep(1000); // 1 ms precision
    }
    return NULL;
}
```

**What this code does**: The monitor thread runs independently of the coder threads. It loops forever, checking the stop flag first (with its own mutex to avoid races). Then it gets the current simulation time and iterates through all coders, comparing the current time against each coder's deadline. If any coder's deadline has passed, it logs the burnout and sets the global stop flag. The `usleep(1000)` gives 1 ms granularity — the monitor checks deadlines 1000 times per second, guaranteeing the 10 ms logging requirement.

### 5. Log Serialization

All output protected by dedicated mutex to prevent interleaved lines from multiple threads.

```c
// src/time/logger.c
void log_state(t_simulation *sim, int coder_id, t_state state)
{
    long timestamp = get_timestamp_ms(sim);
    pthread_mutex_lock(&sim->log_mutex);
    if (state == STATE_TAKEN_DONGLE)
        printf("%ld %d has taken a dongle\n", timestamp, coder_id);
    else if (state == STATE_COMPILING)
        printf("%ld %d is compiling\n", timestamp, coder_id);
    // ... other states
    pthread_mutex_unlock(&sim->log_mutex);
}
```

**What this code does**: Every state change (taking a dongle, compiling, debugging, refactoring, burning out) goes through this function. It takes the log mutex, computes the timestamp, prints the formatted line, and releases the mutex. Without this mutex, two threads calling `printf` simultaneously could produce garbled output like "0 1 has tak0 2 has taken a dongle". The mutex ensures each log line is printed atomically.

## Thread Synchronization Mechanisms

### Primitives Used

| Primitive | Purpose | Location |
|-----------|---------|----------|
| `pthread_mutex_t` | Protect dongle state, log output, simulation stop flag | `t_dongle`, `t_simulation` |
| `pthread_cond_t` | Wait queues for dongles, cooldown timing, shutdown broadcast | `t_dongle`, `t_simulation` |
| `pthread_create`/`join` | Thread lifecycle management | `simulation.c` |

### Shared Resources & Protection

**1. Dongle State (`t_dongle`)**

```c
// Includes/types.h
typedef struct s_dongle
{
    pthread_mutex_t mutex;      // Protects all fields below
    pthread_cond_t  cond;       // Wait queue + cooldown timer
    bool            available;  // Current availability
    long            cooldown_end; // Timestamp when cooldown ends
    int             id;         // Dongle identifier
    t_heap         *heap;       // Per-dongle request queue
} t_dongle;
```

**Race condition prevented**: Multiple coders checking `available` and `cooldown_end` simultaneously. Mutex ensures atomic check-and-acquire.

**2. Simulation Stop Flag**

```c
// Includes/types.h
typedef struct s_simulation
{
    // ...
    pthread_mutex_t stop_mutex;  // Protects stop_simulation
    pthread_cond_t  stop_cond;   // Broadcast on shutdown
    bool            stop_simulation;
    // ...
} t_simulation;
```

**Race condition prevented**: Coder threads and monitor thread reading/writing `stop_simulation` concurrently.

**3. Log Output**

```c
// src/time/logger.c
pthread_mutex_lock(&sim->log_mutex);
printf("%ld %d has taken a dongle\n", timestamp, coder_id);
pthread_mutex_unlock(&sim->log_mutex);
```

**Race condition prevented**: Interleaved `printf` calls from multiple threads corrupting output lines.

### Thread Communication Flow

```
┌─────────────┐     acquire_dongles()      ┌─────────────┐
│  Coder 1    │ ─────────────────────────► │  Dongle 0   │
│  (thread)   │  lock mutex, check avail,  │  (mutex +   │
└─────────────┘  enqueue request, wait     │   condvar)  │
       ▲                                   └──────┬──────┘
       │                                           │
       │ release_dongles()                         │ signal/broadcast
       │  set available=true                       │
       │  set cooldown_end                         │
       │  pthread_cond_signal()                    │
       └───────────────────────────────────────────┘

┌─────────────┐     monitor_routine()       ┌─────────────┐
│  Monitor    │ ─────────────────────────►  │  Simulation │
│  (thread)   │  check deadlines every 1ms  │  (stop flag)│
└─────────────┘  log burnout, set flag      └─────────────┘
       ▲                                           │
       │ broadcast_all_dongles()                   │
       │  pthread_cond_broadcast()                 │
       └───────────────────────────────────────────┘
```

### Example: Race Condition Prevention in `wait_for_dongle`

This is the core function where a coder waits for a dongle. It demonstrates multiple synchronization techniques working together:

```c
// src/coders/coder_utils.c
void wait_for_dongle(t_dongle *dongle, t_coder *coder)
{
    pthread_mutex_lock(&dongle->mutex);           // 1. Acquire dongle mutex
    
    t_request *req = create_request(coder->id,    // 2. Create request
        coder->deadline, get_current_time_ms());
    enqueue_request(dongle->heap, req);           // 3. Enqueue atomically
    
    while (1)
    {
        // 4. Check stop flag with separate mutex (avoid deadlock)
        pthread_mutex_lock(&coder->sim->stop_mutex);
        if (coder->sim->stop_simulation)
        {
            pthread_mutex_unlock(&coder->sim->stop_mutex);
            dequeue_request(dongle->heap);
            pthread_mutex_unlock(&dongle->mutex);
            return;
        }
        pthread_mutex_unlock(&coder->sim->stop_mutex);
        
        // 5. Try acquire if available AND at front of queue
        if (try_acquire_dongle(dongle, coder)     // Checks available + cooldown
            && is_request_at_front(dongle->heap, coder->id))
            break;
        
        // 6. Wait appropriately
        if (!dongle->available)
            pthread_cond_wait(&dongle->cond, &dongle->mutex);
        else
            wait_for_cooldown(dongle);            // Timed wait for cooldown
    }
    
    dequeue_request(dongle->heap);                // 7. Remove from queue
    pthread_mutex_unlock(&dongle->mutex);         // 8. Release mutex
}
```

**Step-by-step explanation**:

1. **Acquire dongle mutex** — This single mutex protects the dongle's availability flag, cooldown timestamp, and the request heap. Holding it ensures no other thread can modify the dongle state while we're examining it.

2. **Create and enqueue request** — We create a request object with the coder's ID, deadline (for EDF), and arrival time (for FIFO/tie-breaking), then insert it into the dongle's priority queue. This is done while holding the mutex so the enqueue is atomic with respect to other waiters.

3. **Loop until acquired** — We loop until we can successfully acquire the dongle. Each iteration re-checks conditions because of spurious wakeups and because the dongle state may have changed.

4. **Check stop flag with separate mutex** — We need to check if the simulation has stopped (due to burnout or all coders finishing). We use a *different* mutex (`stop_mutex`) for this. If we used the dongle mutex, we'd create a lock ordering problem: coder threads lock dongle mutex then stop mutex, while the monitor thread locks stop mutex then broadcasts on dongle condvars (which requires dongle mutex). Using separate mutexes avoids this deadlock.

5. **Try to acquire** — We check two conditions: (a) the dongle is available AND not in cooldown (`try_acquire_dongle`), and (b) this coder's request is at the front of the priority queue (`is_request_at_front`). Both must be true — this enforces the scheduling policy (FIFO or EDF).

6. **Wait appropriately** — If the dongle is not available (held by another coder), we wait on the condition variable with `pthread_cond_wait`. This atomically releases the mutex and sleeps. If the dongle IS available but we're not at the front of the queue (another coder has higher priority), OR if the dongle is in cooldown, we call `wait_for_cooldown` which uses `pthread_cond_timedwait` to sleep until the cooldown expires.

7. **Dequeue request** — Once we've acquired the dongle, we remove our request from the queue.

8. **Release mutex** — Finally, we release the dongle mutex so other waiters can proceed.

**Key design points**:
- **Single mutex** protects dongle state + request queue (no lock ordering issues within a dongle)
- **Separate mutex** for stop flag (prevents deadlock with dongle mutex)
- **Condition variable** for efficient waiting (no busy-wait)
- **Timed wait** for cooldown (precise, no polling)
- **Priority queue** ensures fair scheduling per the chosen policy

## Project Structure

```
.
├── Makefile
├── main.c
├── README.md
├── STUDY_GUIDE.md
├── Includes/
│   ├── codexion.h
│   └── types.h
└── src/
    ├── heap/
    │   ├── heap.c          # Heap create/destroy
    │   └── heap_ops.c      # Heapify, comparison
    ├── time/
    │   ├── time_utils.c    # Timing, deadlines, sleep
    │   └── logger.c        # Thread-safe logging
    └── coders/
        ├── coder.c         # Coder thread routine
        ├── coder_utils.c   # Dongle acquire/release/wait
        ├── monitor.c       # Burnout detection thread
        ├── dongle.c        # Dongle init/destroy/assign
        ├── dongle_utils.c  # Request queue helpers, broadcast
        └── simulation.c    # Simulation lifecycle
```

All `.c` files comply with 42 norminette: **≤5 functions per file, ≤25 lines per function**.
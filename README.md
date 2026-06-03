# Codexion

*This project has been created as part of the 42 curriculum by glieuw-a.*

---

## Description

Codexion is a C concurrency simulation inspired by the Dining Philosophers problem. Coders sit in a circle around a shared Quantum Compiler. Each coder needs two USB dongles (one from each neighbor) to compile their quantum code. The goal is to orchestrate multiple threads so no coder ever burns out — while handling dongle cooldowns, deadlocks, and scheduling policies.

The simulation stops when a coder burns out, or when all coders have compiled the required number of times.

---

## Instructions

**Compile:**
```
make
```

**Run:**
```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory.

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) |
| `time_to_burnout` | Milliseconds before a coder burns out without compiling |
| `time_to_compile` | Milliseconds to compile (requires two dongles) |
| `time_to_debug` | Milliseconds to debug |
| `time_to_refactor` | Milliseconds to refactor |
| `number_of_compiles_required` | Stop when all coders reach this compile count |
| `dongle_cooldown` | Milliseconds a dongle is unavailable after release |
| `scheduler` | `fifo` (First In First Out) or `edf` (Earliest Deadline First) |

**Examples:**
```bash
# normal run
./codexion 5 800 200 200 200 3 50 fifo
./codexion 5 800 200 200 200 3 50 edf

# single coder (no deadlock possible)
./codexion 1 800 200 200 200 3 50 fifo

# burnout scenario (compile time exceeds burnout window)
./codexion 5 200 800 200 200 3 50 fifo
```

**Memory check:**
```bash
valgrind --leak-check=full ./codexion 2 800 200 200 200 3 50 fifo
```

**ThreadSanitizer:**
```bash
make san
./codexion_san <args>
```

**Clean:**
```
make clean    # remove object files
make fclean   # remove object files and binaries
make re       # full rebuild
```

---

## Blocking cases handled

### Deadlock prevention
Each coder always acquires the lower-indexed dongle before the higher-indexed one. This breaks the circular wait condition from Coffman's four conditions. Coder `i` has left dongle `i-1` and right dongle `i % N`. The acquisition order is `min(left, right)` then `max(left, right)`, ensuring no cycle can form in the wait graph.

### Starvation prevention
Fair arbitration is enforced per dongle via an ordered wait queue. With `fifo`, requests are served in arrival order. With `edf`, the coder whose burnout deadline is earliest is served first. No coder can be skipped indefinitely under either policy.

### Cooldown handling
After a dongle is released, it enters a cooldown period of `dongle_cooldown` ms during which it cannot be taken. Waiting coders use `pthread_cond_timedwait` with a timeout set to the exact moment the cooldown expires, so they wake up immediately when the dongle becomes available — no busy-polling, no missed wakeup.

### Precise burnout detection
A dedicated monitor thread polls all coders every 500 µs. It checks `now - last_compile_start > time_to_burnout` for each coder. When burnout is detected, it logs the event and sets the global stop flag. The burnout message is guaranteed to appear within 10 ms of the actual burnout time.

### Log serialization
All output is protected by a single `log_mutex` (`pthread_mutex_t`). Only one thread can call `printf` at a time, so messages are never interleaved. The mutex is locked for the duration of the printf call and immediately released.

---

## Thread synchronization mechanisms

### Mutexes used

| Mutex | Protects |
|---|---|
| `dongle.mutex` | Each dongle's state: `available`, `ready_at`, and wait queue |
| `sim.stop_mutex` | The global `stop` flag read/written by all threads |
| `sim.log_mutex` | Stdout — prevents interleaved log lines |

### Condition variables

Each dongle has a `pthread_cond_t`. When a coder cannot take a dongle (unavailable, cooldown active, or not its turn in the queue), it blocks on `pthread_cond_wait` or `pthread_cond_timedwait` inside that dongle's mutex. `pthread_cond_broadcast` is called on release and on `sim_stop`, waking all waiters.

`pthread_cond_timedwait` is used specifically when a cooldown is active: the timeout is set to `ready_at` (the exact ms timestamp when the dongle becomes available again). This eliminates the missed-wakeup problem that would occur if the broadcast fires during cooldown and no one re-broadcasts when the cooldown expires.

### Race condition prevention examples

**Dongle state:** The queue and `available` flag are only ever read or written while holding `dongle.mutex`. A coder checks availability and registers its intent atomically — no coder can take a dongle that another coder has already claimed.

**Stop flag:** `sim_stop` locks `stop_mutex`, sets `stop = 1`, and unlocks before broadcasting. Any thread calling `sim_stopped` will either see `stop = 0` (before the lock) or `stop = 1` (after). There is no window where the flag is partially written.

**Thread-safe communication (coders ↔ monitor):** The monitor reads `last_compile_ms` from coder structs. Coder threads write it in `coder_compile`. Since `last_compile_start` is a single `long` and only moves forward in time, a stale read by the monitor means it detects burnout slightly later than the exact moment — still within the 10 ms tolerance guaranteed by the 500 µs polling interval.

---

## Resources

- [POSIX Threads Programming — Lawrence Livermore](https://hpc-tutorials.llnl.gov/posix/)
- [The Little Book of Semaphores — Allen B. Downey](https://greenteapress.com/semaphores/)
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First Scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [pthread_cond_timedwait — Linux man page](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)

**AI usage:** Claude Code (claude-sonnet-4-6) was used as a development assistant throughout this project. It helped explain threading concepts (condition variables, EDF scheduling, cooldown wakeup timing), reviewed logic for correctness, and assisted with debugging the cooldown-broadcast missed-wakeup bug and the stack-allocated monitor_args lifetime bug.

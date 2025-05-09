#include <atomic>



int main() {
  std::atomic_flag f = ATOMIC_FLAG_INIT; // std::atomic_flag object must be initialized with ATOMIC_FLAG_INIT. 
                                         // It initializes to a clear (false) state

  // Only 3 possible operations. destroy it, clear it or set it and query the previous value
  // destroy -> destructor
  // clear it -> clear()
  // set and query -> test_and_set()

  // clear() is a store operation | cant use memory_order_acquire or memory_order_acq_rel
  // test_and_set() is a read-modify-write operation | can use any memory ordering

  // default memory ordering is always memory_order_seq_cst

  f.clear(std::memory_order_release); // require memory_order_Release

  bool x = f.test_and_set(); // default memory ordering

  // You cant copy construct another std::atomic_flag and you cant assign an atomic flag to another
  
}

// We can use std::atomic_flga to create a spinlock mutex
class spinlock_mutex {
  std::atomic_flag flag;
public:
  spinlock_mutex() : flag(ATOMIC_FLAG_INIT) {}

  void lock() {
    while (flag.test_and_set(std::memory_order_acquire));
  }

  void unlock() {
    flag.clear(std::memory_order_release);
  }
};

/*

Spinlock mutex advantage:

  Low Latency: No OS involvement (no context switches).

  Lock-Free Guarantee: Uses std::atomic_flag, which is always lock-free.

  Predictable Timing: Ideal for real-time systems where worst-case latency matters.

Spinlock mutex disadvantage

  Busy-Waiting: Spins in a loop, consuming CPU cycles while waiting.

  Inefficient for Long Waits: Wastes CPU time if the lock is held for more than a few cycles.

  Priority Inversion Risk: High-priority threads can starve if a low-priority thread holds the lock.

*/
# Atomic Operation and Types in C++

An **atomic operation** is an indivisible operation. Cant be half done. It is either DONE or NOT DONE

If the **load** operation that reads the value of an object is *atomic*, and all **modifications** are also *atomic*
means that the load will retrieve either the **initial value of the object** or the value stored **after one of the modifications**.

## Standard Atomic Types

Contained in the **\<atomic\>** header.

All operations on such types are atomic, and only operations on these types are atomic.

Mutexes can be used to make operations **APPEAR** atomic, and the compiler might do this sometimes.

All standard atomic types contain a member function to know this for sure -> **is_lock_free()**

**is_lock_free()** return true to know if a operation is done with atomic instruction or is done by using a 
lock internal to the compiler and library.

This is very important for HPC and low latency. When a internal lock is used the performance gains of using 
atomics will not materialize.

It is SO IMPORTANT that, after C++17 there is a more powerful tool, a set of **macros to check at compile time
whether an atomic type is really lock-free**.

All atomic types have a **static constexpr** member variable, **X::is_always_lock_free**, which is true if the atomic
type X is lock-free for all supported hardware for this compilation

The macros are:
```cpp
  ATOMIC_BOOL_LOCK_FREE
  ATOMIC_CHAR_LOCK_FREE
  ATOMIC_CHAR16_T_LOCK_FREE
  ATOMIC_CHAR32_T_LOCK_FREE
  ATOMIC_WCHAR_T_LOCK_FREE
  ATOMIC_SHORT_LOCK_FREE
  ATOMIC_INT_LOCK_FREE
  ATOMIC_LONG_LOCK_FREE
  ATOMIC_LLONG_LOCK_FREE
  ATOMIC_POINTER_LOCK_FREE
```

3 possible values:
- 0 -> operation is never lock-free (internal mutexes used)
- 1 -> operation is sometimes lock-free. Now we need to use the runtime .is_lock_free() in the object to determine it
- 2 -> operation is always lock free.

We can use it like a Macro, for example:

```cpp
#include <iostream>
#include <atomic>   // For std::atomic and ATOMIC_BOOL_LOCK_FREE
#include <string>

std::string get_bool_atomic_strategy() {
    std::string strategy_description;

    #ifdef ATOMIC_BOOL_LOCK_FREE
        #if ATOMIC_BOOL_LOCK_FREE == 0
            // Case 0: std::atomic<bool> is never lock-free
            strategy_description = "Strategy for non-lock-free std::atomic<bool>: Using fallback (e.g., mutex for bool operations).";
            // Probably write a code based in locks, since it is simple to write, and anyway there will be locks
        #elif ATOMIC_BOOL_LOCK_FREE == 1
            // Case 1: std::atomic<bool> is sometimes lock-free
            strategy_description = "Strategy for sometimes-lock-free std::atomic<bool>: Requires runtime check (instance.is_lock_free()). Will adapt.";
            // Check at runtime and have 2 path executions
        #elif ATOMIC_BOOL_LOCK_FREE == 2
            // Case 2: std::atomic<bool> is always lock-free
            strategy_description = "Strategy for always-lock-free std::atomic<bool>: Optimized lock-free operations assumed.";
            // WE are assure that the operations are always atomic, so leverage those operation
        #else
            strategy_description = "Unknown value for ATOMIC_BOOL_LOCK_FREE. This should never happen.";
        #endif
    #else
        strategy_description = "ATOMIC_BOOL_LOCK_FREE is not defined. This should not happen if the header is included";
    #endif

    return strategy_description;
}
```

Only type withour *is_lock_free* is **std::atomic_flag**. It is just a boolean flag and operaion on that type are required to be lock-free.

It is the most basic and simple atomic operation, and be used as a build block for another atomic types.

Objects of this type are initialized to clear (false), by default aftwe C++20 or with this line before:
```cpp
std::atomic_flag flag = ATOMIC_FLAG_INIT;
```

Then they can be **queried and set** (check previous value and set the value to true). This can be used
with difference memory orders:
```cpp
bool was_set = flag.test_and_set(std::memory_order_acquire); // was_set store the old value, and the flag is set to
                                                            // true now
flag.clear(); // reset it to false
```
Possible orderings are (we will go into more detail later):
```cpp
std::memory_order_relaxed //-> dont impose any ordering but guarantee atomicity
                          // Usage: simple counters where the order onf icnreaseing between threads
                          // does not matter. It is also the fastest one.

std::memory_order_acquire // -> Load Operations. Synchronize with another thread relaseinrelease (store) operation
                          // Usage: similar to adquiring a lock, or read

std::memory_order_consume // not available for test_and_set()

std::memory_order_release // not available for test_and_set()


std::memory_order_acq_rel  // operations that both read and modify, so load modify and store

std::memory_order_seq_cst  // sequentially consistent. The most restrictive
```

Atomic flag only have these 2 operations. No assignment, no copy constructor, nothing else.
The remaining atomic types are acceses with specializations of **std::atomic<>** and they may not be lock-free. But in the most popular platforms normally all the atomic variants of the built-in types (like std::atomic<int> or std::atomic<void*>) are lock-free. However this is not required, so it is important to check it.

Some old names can be used instead of the specialization:
```cpp
atomic_bool     	std::atomic<bool>
atomic_char	      std::atomic<char>
atomic_schar	    std::atomic<signed char>
atomic_uchar	    std::atomic<unsigned char>
atomic_int	      std::atomic<int>
atomic_uint	      std::atomic<unsigned>
atomic_short	    std::atomic<short>
atomic_ushort	    std::atomic<unsigned short>
atomic_long	      std::atomic<long>
atomic_ulong    	std::atomic<unsigned long>
atomic_llong	    std::atomic<long long>
atomic_ullong    	std::atomic<unsigned long long>
atomic_char16_t	  std::atomic<char16_t>
atomic_char32_t	  std::atomic<char32_t>
atomic_wchar_t	  std::atomic<wchar_t>
```

Atomic types are not copyable or assignable, so no copy constructor or copy assignment operator.

They support assignment from and implicit conversion to the corresponding build-in types.
And also support the next member functions:
- load()
- store()
- exchange()
- compare_exchange_weak()
- compare_exchange_strong()
And the next operatios if appropiate
- +=
- -+
- *=
- |=

Return value for assignment operator and member functions is:
- value stored (assignment operators) -> they dont return a reference as it is tipical in the assigment operator to avoid problems with other thread modifying in the meanwhile.
- value prior to the operation (name functions)

## User Defined atomic type

**std::atomic<> can be used  to create an atomic variabt of a user defined type. The operations are again limited to:
- load()
- store()
- exchange()
- compare_exchange_weak()
- compare_exchange_strong()

As we wrote before there are 6 memory orderings. That are different values of the enum **std::memory_order**
- std::memory_order_relaxed 
- std::memory_order_acquire 
- std::memory_order_consume 
- std::memory_order_release /
- std::memory_order_acq_rel 
- std::memory_order_seq_cst 

## Atomic Operations

Permited values for memory ordering depend on the operation category. The operations are divided into 3 categories:

- **Store** operations can have:
- - memory_order_relaxed
- - memory_order_release
- - memory_order_seq_cst

- **Load** operations can have:
- - memory_order_relaxed
- - memory_order_acquire
- - memory_order_consume
- - memory_order_seq_cst

- **Read-modify-write** operations:
- - memory_order_relaxed
- - memory_order_acquire
- - memory_order_consume
- - memory_order_release
- - memory_order_acq_rel
- - memory_order_seq_cst
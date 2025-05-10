# Operations on the Atomic Flag and Overview

## std::atomic_flag

**Initialization**:
Needs the macro and always set the state to clear
```cpp
std::atomic_flag f=ATOMIC_FLAG_INIT;
```
**Clear** (set state to clear):
```cpp
std::atomic_flag f;
f.clear(std::memory_order_release);
```
**test_and_set()**:
Return current value and change its state to set
```cpp
std::atomic_flag f;
bool x = f.test_and_set();
```

**You cant copy or assign an atomic flag (or any atomic type)**. Copy or assignment involve to 2 object and cant be atomic
Spinlock mutex using the features of atomic flag
```cpp
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
```
## std::atomic\<bool\>
Atomic<bool> is the most basic of the atomic integral types.

**Build from non atomic**
```cpp
std::atomic<bool> b(true);
```
### Assign from non atomic
```cpp
b = false;
```
Return value of assignment is a bool, not a reference.

### Writes
Using *store* and we can specify the memory order
```cpp
b.store(true);
```

### Read
Using *load* and we can speficy memory ordering
```cpp
bool res = b.load(std::memory_order_acquire);
```

### read-and-modify
Using *exchange*. *exchange* returns the original value and replace it with a new one
```cpp
bool old_value = b.exchange(false,std::memory_order_acq_rel);
```

### Compare-exchange
Store a new value (or not) depending on the current value.

**compare-exchange operation** is very important. It compares the value of the atomic variable with a supplied expected value.
If those values are equal, the atomic stores the new value
If they are not equal, the expected value is updated with the value of the atomic.
The return type is a boolean, that is **true if the store was performed**. False otherwise.

Basically if the values were equal, the store is done, and the operation succeded, so the return is true.

**compare_exchange_weak()**
The store might not be succesful even if original value was equal. Value is unchanged and the return is false.
This happens if the machine lacks a *single compare-and-exchange instruction* and the processor cant guarantee that operation was done *atomically*
The thread performing the operation may have switched out in the middle of the sequence of instructions and another thread scheduled in its place did something ( a OS where you have more threads than processors).

This is called a *spurious failure*: reason of failure is a function of timing rather than value of the variables

The typical way of using compare_exchange_weak is in a loop due to the possibility of failing spuriously.
```cpp
bool expected = false;
extern atomic<bool> b;
while(!b.compare_exchange_weak(expected,true) !! !expected);
```
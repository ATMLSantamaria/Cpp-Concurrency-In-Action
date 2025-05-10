# std::atomic\<bool\>
Atomic<bool> is the most basic of the atomic integral types.

**Build from non atomic**
```cpp
std::atomic<bool> b(true);
```
## Assign from non atomic
```cpp
b = false;
```
Return value of assignment is a bool, not a reference.

## Writes
Using *store* and we can specify the memory order
```cpp
b.store(true);
```

## Read
Using *load* and we can speficy memory ordering
```cpp
bool res = b.load(std::memory_order_acquire);
```

## read-and-modify
Using *exchange*. *exchange* returns the original value and replace it with a new one
```cpp
bool old_value = b.exchange(false,std::memory_order_acq_rel);
```

## Compare-exchange
Store a new value (or not) depending on the current value.

### **compare-exchange operation** 
Is very important. It compares the value of a variable with a supplied expected value.
If those values are equal, the atomic stores the new value
If they are not equal, the expected value is updated with the value of the atomic.
The return type is a boolean, that is **true if the store was performed**. False otherwise.

Basically if the values were equal, the store is done, and the operation succeded, so the return is true.

Example of usage to flip a boolean atomically
```cpp
std::atomic<bool> flag = false;
bool expected, desired;
do {
    expected = flag.load();
    desired = !expected; // Toggle the flag
} while (!flag.compare_exchange_weak(expected, desired));

```

### **compare_exchange_weak()**
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
Here you keep looping as expected is still false;

**compare_exchange_strong()**
Guaranteed to return false only if the value was not equal. So, no loop required.

If the *calculation is simple* it could be better to use *compare_exchange_weak()*

If the *calculation of the value to be stored is time-consuming*, it make sense to use *compare_exchange_strong()* to avoid recalculate the value to store when the expected value has not changed.


Compare-exchange functions are also unsual in that they can take 2 memory ordering parameters. 
>
>It is possible to set one for the case or failure and one for the case of succed:
>- **Succesful call**: Better use **memory_order_acq_rel**
>- **A failed compare-exchange** does not do a store, so it canot have *memory_order_release* or *memory_order_acq_rel*
>
>Also, the memory order for failure cannot be stricter than for succes
>If you dont specify one memory ordering for failure, it is assumed to be the same that for success, but the release part is stripped. 
>
>For example, *memory_order_release* become *memory_order_relaxed* and *memory_order_acq_rel* become *memory_order_acquire*.
>
>If nothing it is specified, the default is as always *memory_order_seq_cst*.

Lets see a example where 2 calls to compare_exchange_weak are equivalent:
```cpp
std::atomic<bool> b;
bool expected;
b.compare_and_exchange_weak(expected,true,memory_order_acq_rel,memory_order_acquire);
b.compare_and_exchange_weak(expected,true,memory_order_acq_rel); // ordering for failure assumed the same than for success, but stripped of the release
```

# std::atomic<T*>: Pointer arithmetic

Atomic form of a pointer of type T is simply an std::atomic<T*>

As other atomic types is not copy-constructible or copy-assignable, but it can be constructed and assigned from the suitable *pointer values*


## Pointer Arithmetic operations

### fetch_add()
Also nown as *exchange-and-add* is an atomic read-modify-write like compare_echange_weak() or exchange
Add a number to the pointer, or increase its address by that number of elements, and return the original pointer
```cpp
Foo arr[10];
std::atomic<Foo*> x(arr); // x points to the first position
Foo * original_ptr = x.fetch_add(3); // now original_ptr points to the first position and x to the fourth
```
Because it is a *read-modify-write* they can have any of the memory ordering tags and participate in a *release sequence*
A release sequence is a chain of atomic operations starting with a release store and followed by RMWs.
The release sequence ends when a non-RMW atomic store occurs, or if any other kind of operation (including ordinary writes) happens.
Example of a release sequence
```cpp
// Thread 1
data = 42; // ordinary write
atomic_var.store(1, std::memory_order_release); // start of release sequence
atomic_var.fetch_add(1, std::memory_order_relaxed); // continues release sequence
atomic_var.store(2, std::memory_order_relaxed); // ends release sequence

```
### fetch_sub() (atomic read-modify-write)
It is all the same than for fetch_add but instead of add it substract

Because it is a *read-modify-write* they can have any of the memory ordering tags and participate in a *release sequence*

### Operators += and -=
Operates like in a the built-in type
```cpp
Foo arr[10];
std::atomic<Foo*> x(arr); // x points to the first position
x+=3; // now x point to the 4th position
```
The memory order in the operators is always **memory_order_seq_cst**

### Operators ++ and --

The memory order in the operators is always **memory_order_seq_cst**

## Common Operations

### is_lock_free()
Like in atomic flag and atomic<bool>

### load() 
Like atomic\<bool\>

### store() 
Like atomic\<bool\>

### exchange() 
Like atomic\<bool\>

### compare_and_exchange_weak() and compare_and_exchange_strong() 
Like atomic\<bool\>




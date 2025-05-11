# Atomic Integral Types

With respect to a normal integral type only division, multiplication and shift operators are missing.

This is not really important since atomic integral values usually are used as **counters or bitmasks**

If require, we can use *compare_exchange_weak()* in a loop to multiply

## fetch_add()
As in the pointer, this perform its operation (add) and return the old value

## fetch_sub()
As in the pointer, this perform its operation (subtract) and return the old value

## fetch_and()
Performs an **atomic bitwise AND** operation between the atomic variable and a given value.
```cpp
std::atomic<int> x(0b1100);           // x = 12 (binary 1100)
int old = x.fetch_and(0b1010);        // x = x & 0b1010 (binary 1010 = 10)
std::cout << "Old value: " << old << "\n";       // 12
std::cout << "New value: " << x.load() << "\n";  // 8 (binary 1000)
```

## fetch_or()
Performs an **atomic bitwise OR** operation between the atomic variable and a given value.
```cpp
std::atomic<int> x{0b1100};           // x = 12 (binary 1100)
int old = x.fetch_or(0b1010);         // x = x | 0b1010 (binary 1010 = 10)
std::cout << "Old Value: " << old << std::endl;   // 12
std::cout << "New Value: " << x << std::endl;     // 14 (binary 1110)
```

## fetch_xor()
Performs an **atomic bitwise XOR** operation between the atomic variable and a given value
```cpp
std::atomic<int> x(0b1111);           // x = 15 (binary 1111)
int old = x.fetch_xor(0b1010);        // x = x ^ 0b1010 (binary 1010 = 10)
std::cout << "Initial Value: " << old << std::endl; // 15
std::cout << "Final Value : " << x.load() << std::endl; // 5 (binary 0101)
```

## Compounds Assignments operations +-, -+, &=, |= and ^=
The compound assignment return the **new value**

## Pre and post increment and decrement ++x, --x, x++,x--
Perform as usual
++x : increase variable and return the new value
x++:  return the old value and increment the variable

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

# C++ Memory Model and Atomic types

- C++ memory model
- Atomic types in the C++ Standard Library
- Operation on those types
- Strategies for synchronization between threads


C++ is a systems programming language. One of its goal is to avoid the need of a lower level language.
Therefore we need to get close to the machine when the need arise.

**Atomic types and its operations** are facilities for low-level synchronization operations, that allow to
retuce one or two CPU instructrions with respect to higher level facilities.

## C++ memory model

First, C++ standard definition of memory location:


> The C++ standard has a specific definition for "memory location." A memory location is either:
>- An object (or sub-object) of scalar type (like int, char, float, double, pointers, enums).
>- A sequence of adjacent bit-fields.
>




If we consider the next struct

```cpp
struct my_struct {
  int i;            // a int object in its own memory location
  double d;         // double object in its own mem location
  unsigned a1 : 10; //
  int a2 : 25;      // this 2 adjacent bit_fields share a memory location since in total they are just 35 bits, and 
                    // their type is compatible (one is unsigned int and the other is an int). in a 64 bit machine 
                    // they will fit together
  int a3 : 0;       // force alignment of a4
  int a4 : 9;       // due to the prececind a3 : 0, a4 start in its own memory
  char c1,c2;       // both c1 and c2 are objects with its own memory location
}
```

Some notes:
- Every variable is an object, even member variables of other objects are objects
- Every object occupy at least one memory location
- Variables of fundamental type occupy exatcly one memory location, even if they are adjacent
- Adjacent bit fields are part of the same meory locatio,

### Impact on concurrency

If 2 threads access separate memory locations no problem. You can have 2 threads modifying different member variables of a struct, if they are not related.

If 2 threads access same memory location but they are read only also not a problem.

Problem come when 2 or more threads access same memory location, and 1 or more is modifying it. Then we have a potencal data race.

**Avoid data race** -> we need specific ordering. Fixed one if one always access before, or an ordering
that change in function of different runs in the application. But a ordering must exist

**Mutex** as we saw, it is a way of ensuring this ordering. One thread get blocked while other thread is in that memory.

**Atomic Operations** are the other way of ensure ordering between the access in the 2 or more threads.

If you have 2 or more access to a memory location, and 1 or more are write access, and you dont have a mutex or an atomic access, then you have a data race and the result is Undefined Behaviour. 

One point to notice, is that atomic operations dont exatly avoid the data race, but they bring order in the access, so
the sofware become defined and predictable.

## Modification Order

Each object in C++ program has a *modification order* composed of all the writes to that object from all threads in the program, starting with the initialization.

The order may change between runs, but in any given execution all threads must agree in thge order.

You are responsible to ensure there is sufficient synchronization to ensure that threads agree on the modification order.

If you use atomic types the compiler is responsible for ensuring the necessary synchronization.
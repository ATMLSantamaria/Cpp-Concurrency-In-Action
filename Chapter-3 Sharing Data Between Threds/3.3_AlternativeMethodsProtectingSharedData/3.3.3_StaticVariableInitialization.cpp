

// Prior to C++11 there could be a race condition over initialziation of a local variable declare static

// Static variable is initializard the first time control passes through its declaration. There is a race
// condition to define first.

// After C++11 the initialization is defined to happen on only one thread, and no other thread will proceed until that initialization is complete

class my_class {};
my_class & get_my_class_instance() {
  static my_class instance;
  return instance;
}
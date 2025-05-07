// In a class, protect the data is not as simple as put a lock in each member function.

// If any member function would return a pointer or reference, that would spoil such protection.

// E.g. ACCIDENTALLY PASSING OUT A REFERENCE TO PROTECTED DATA

#include <string>
#include <mutex>

class SomeData {
  int a;
  std::string b;
public:
  void do_something() {}
};


class DataWrapper {
private:
  SomeData data;
  std::mutex m;
public:
  template<typename Function>
  void process_data(Function func) {
    std::lock_guard<std::mutex> l(m);
    func(data);
  }
};

SomeData * unprotected;

void evil_function(SomeData & protected_data) {
  unprotected = &protected_data;
}

DataWrapper x;
void foo() {
  x.process_data(evil_function); //template argument deduction
  x.process_data<void(*)(SomeData&)>(evil_function); // if explicit argument is needed this is the type of a funtion. (return)(*)(argument)
  unprotected->do_something();
}

// This is a great example. Code in DataWrapper::process_data looks very harmless, being protected by the lockguard.

// But as we pass a function, and the function is user-supplied, we can pass a evil_function to bypass the protection.

// Then we can call do_something() and the mutex WONT be locked, so we will have the data race.

// Problem here from the point of view of designing SomeData and DataWrapper is that we did not mark all the pieces of code
// that access the data structure as mutually exclusive.

// We missed the unprotected->do_something();

// In general to protect classes. Dont pass ptr and references to protected data outside the scope of the lock. This may happen:

// 1 - Directly returning them from a function
// 2 - Storing them in externally visible memory
// 3 - Passing them as arguments to user-supplied functions.

// Also consider that just because individual operations are protected does not mean that you are safe.
// In the example of deleting a node from a Doubly Linked List you could protect the operations to change prev and next
// and still get a race condition. You would need also to protect the full "delete" operation.
int main(){}
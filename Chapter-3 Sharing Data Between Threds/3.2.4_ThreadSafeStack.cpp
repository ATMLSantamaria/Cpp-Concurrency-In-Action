
// This a simple implementation of a thread safe stack taking into consideration
// the issues we previously outlined

// Will include a pop whose signature include result by reference and a pop that return a ptr

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

// Define the empty stack exception
struct empty_stack : std::exception {
  const char * what() const throw();
};

// Now the stack itself
template<typename T>
class threadsafe_stack {

  std::stack<T> data;

  mutable std::mutex m;

public:

  threadsafe_stack(){};

  // Perform the copy in the body of the constructor and not in initializer list
  // in order to guarantee that the mutex is locked durign the copy.
  threadsafe_stack(const threadsafe_stack& other) {
    std::lock_guard<std::mutex> lock(other.m); // you activate other.m so other cannot be modified by its member functions since its m is locked
    data = other.data;
  }

  threadsafe_stack & operator=(const threadsafe_stack&) = delete; // we want to avoid assignments

  void push(T new_value) {
    std::lock_guard<std::mutex> lock(m);
    data.push(std::move(new_value)); // more efficient to move the local copy of new_value instead of copying again
  }

  // pop with signature
  void pop(T & result) {
    std::lock_guard<std::mutex> lock(m); // avoid modifications here while we are poping
    if (empty()) {
      throw empty_stack();
    }
    result = data.top(); // we make the data on the top available
    data.pop();
  }

  // pop returning a pointer
  std::shared_ptr<T> pop() {
    std::lock_guard<std::mutex> lock(m);
    if (empty()) {
      throw empty_stack();
    }
    std::shared_ptr<T> const res = std::make_shared<T>(data.top());
    data.pop();
    return res;
  }


  bool empty() {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }
};

// Overall problems with race conditions:
// 1 - Can appear for protecting frangments of code that are too small or granular, like pop top
// 2 - Can also arise if protecting too large pieces of code, like all shared data, losing
//     all benefits of concurrency.
//     e.g. first linux kernel for multiprocessor had a single global kernel lock.
//     This meant that 2 processor system had worse performance that 2 single processor systems.
//     This was due to having to much protection, so additional processors did not perform useful work.

int main() {}
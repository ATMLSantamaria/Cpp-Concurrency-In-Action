// Chapter 1 is pure introduction so lets start the repo with a classical hello workd

#include <iostream>
#include <string>
#include <thread>


// These threads are not synchronized, so their outputs to the cout stream
// may appear in any order due to concurrent execution. 

void hello() {
  std::cout << "Hello Concurrency Interested Friends from a function pointer\n";
}

class HelloFunctor {
public:
  // Functor: callable object that takes a string argument and prints a message
  void operator()(const std::string & name) {
    std::cout << "Hello Concurrency Interested Friends from the " << name << " functor\n";
  }
};

int main() {
  std::thread t1(hello);
  HelloFunctor thisFunctor;
  // Start a thread running the functor, passing "Pepe" as the argument to operator()
  // The functor is copied into the thread, and "Pepe" is passed as an argument
  std::thread t2(thisFunctor,"Pepe"); // Pepe is passed as argument to the callable object by thread
  std::thread t3([](){std::cout << "Hello Concurrency Interested Friends from a lambda\n";});
  t1.join();
  t2.join();
  t3.join();
}


#include <thread>
#include <iostream>
#include <exception>

class ThreadGuard {
  // Reference to the thread that will be guarded
  std::thread & thread_;
public:

// in the constructor we pass the thread to guard to our local handler of the thread
  explicit ThreadGuard(std::thread & target): thread_(target) {}
  // In the destructor we join it
  ~ThreadGuard() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }
  // Delete copy and assignment operators
  ThreadGuard(const ThreadGuard &) = delete;
  ThreadGuard & operator=(const ThreadGuard &) = delete;
};

struct MyFunctor {
  void operator()(int number) {
    //do something with number
  }
};


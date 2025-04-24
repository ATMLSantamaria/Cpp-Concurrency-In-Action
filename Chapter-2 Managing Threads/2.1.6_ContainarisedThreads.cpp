#include <vector>
#include <thread>
#include <iostream>
#include <mutex>

// Move Semantics allow us to pass threads to containers

void function() {
  for(int i = 0; i < 2; i++) {
    // std::cout on the thread ID have no semantic meaning, but it is guaranteed that the if will be comparable
    // with other ids,and the < and > operations will give the expected results
    std::cout << "thread id = " << std::this_thread::get_id() << " iteration = " << i << std::endl;
  }
}

//std::thread::id is a unique identifier for the thread that works in most circunstances
// It provides a set of comparison
// It also provide a std::hash<std::thread::id> if you need an associative container

// The code is problematic since the iostream suffer a heavy race condition here. But it is just to show
int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 30;i++) {
    threads.emplace_back(function); // use emplace for the move semantics
  }

  for (auto &t : threads) {
    t.join();
  }
}

#include <vector>
#include <thread>
#include <iostream>
#include <mutex>

// Move Semantics allow us to pass threads to containers

void function(int id) {
  for(int i = 0; i < 16; i++) {
    std::cout << "thread id = " << id << " iteration = " << i << std::endl;
  }
}


// The code is problematic since the iostream suffer a heavy race condition here. But it is just to show
int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 30;i++) {
    threads.emplace_back(function,i); // use emplace for the move semantics
  }
}

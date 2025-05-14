#include <iostream>
#include <atomic>
#include <cassert>



// Basic usage compare exchange weak
// Remember this is a read-modify-write operation

void simple_compare_exchange_weak() {
  std::atomic<int> value(0);

  int expected = 0;
  int desired = 33;

  // In principle this operation will procced, because expected is expected.
  // It can however fail spuriously, if the OS switch threads in the middle of the operation
  bool res = value.compare_exchange_weak(expected,desired);

  // So, after this operation, value should be equal to desired. Lets see
  assert(value.load() == 33);
  assert(res == true);

  desired = 44;
  expected = 22;
  res = value.compare_exchange_weak(expected,desired);
  // This operation will not succed, since value does not contain expected
  assert(value.load() != desired);
  assert(value.load() == 33);
  assert(res == false);
  // This last part is the important assertion, expected is updated to the value of the atomic val
  assert(expected == value.load());
}

// Now a sighly more complex usage
#include <thread>
#include <vector>
void increment_with_loop(std::atomic<int> & counter) {
  int expected = counter.load();
  int desired = expected+1;
  
  while(!counter.compare_exchange_weak(expected,desired)) {
    // operation returned false so expected is updated to the current value of counter
    
    desired = expected + 1; // we will try again to increment by one, the new value
  }

}

void multithread_compare_exchange_weak(){

  std::atomic<int> counter; // shared variable by threads;

  unsigned int n_cores = std::thread::hardware_concurrency();
  std::vector<std::thread> hardware_threads;
  hardware_threads.reserve(n_cores);

  for (u_int64_t i = 0; i < n_cores;++i) {
    hardware_threads.emplace_back(increment_with_loop,std::ref(counter));
    
    // Remember: hardware_threads.emplace_back(increment_with_loop,counter); WONT COMPILE
    // Atomic variables cannot be copied, if you dont use std::ref it will be copied
    // std::thread copies all values even if  the function expect a reference
    // std::thread, as std::bind applies std::decay to all arguments

    /*
      std::decay 
      type trait that standarise types:


      Remove references (e.g., int& becomes int)
      Remove const/volatile qualifiers (e.g., const int becomes int)
      Convert arrays to pointers (e.g., int[5] becomes int*)
      Convert function types to function pointers (e.g., void(int) becomes void(*)      
    */

    // Remember: std::ref avoid this because it wraps the reference in a copyable object
    // of type std::reference_wrapper. 
    // std::decay is applied to the object, but inside the object the reference is safely protected
  }

  for (u_int64_t i = 0; i < n_cores;++i) {
    hardware_threads[i].join();
  }


}




int main() {
  simple_compare_exchange_weak();
  multithread_compare_exchange_weak();
}
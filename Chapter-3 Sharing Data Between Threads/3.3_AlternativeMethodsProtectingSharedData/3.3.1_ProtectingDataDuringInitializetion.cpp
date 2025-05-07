/*
  Shared data may need protection from concurrent access only while it is being initialised. And after that no
  explicit synchronization is required.

  Maybe the resource is to expensive to construct it twice, opening a database conenction or allocating a lot of memory
*/

#include <memory>
#include <mutex>
class some_resource {
public:
  void do_something(){}
  void reset(some_resource & other){}
};

// In Single Threaded code we would do lazy initialization
std::shared_ptr<some_resource> resource_ptr;

void lazy_initialization_single_thread() {
  if (!resource_ptr) {
    resource_ptr.reset(new some_resource);
  }
  resource_ptr->do_something();
}

// A naive protection to the initialization can cause unnecesary serialization of thrads using the resources.
// This happen in the case of each thread waiting in the mutex to check whether the resource has been
// already initialized.

// Basically all threads wait in a line waiting to check if the resource was initialize, spoiling any benefit
// from initialization

std::mutex resource_mutex;
void unnecesary_serialized_protection() {
  std::unique_lock<std::mutex> lk(resource_mutex); // all threads are serialized here
  if (!resource_ptr) {
    resource_ptr.reset(new some_resource);
  }
  lk.unlock();
  resource_ptr->do_something();
}

// This code is really common, and the unnecessary serialization is problematic.
// Even more problematic is the infamous:  double-checked locking pattern

// The pointer is first read without acquirin the lock, and the lock is acquired only if pointer is null
// The pointer is hten check again, in case other thread has done the initialization between the first check and this
// thread acquiring the lock

void undefined_behaviour_with_double_checked_locking() {
  if (!resource_ptr) {                                  // ptr read without acquiring the lock
    std::lock_guard<std::mutex> lk(resource_mutex);
    if(!resource_ptr) {                                 // double check in case another thread has done initialization from the dirst check to the lock
      resource_ptr.reset(new some_resource);
    }
  }
  resource_ptr->do_something();
}

// PROBLEM: The read outside lock 1 is not synchronized with the write done by another thread inside the lock 3.

// Basically one thread arrive to 1 and read something. Then other thread arrive to 1, continue to 2 and 3 before the first thread moves
// What 1 have read is "old", now there is a different state in the shared resource, so resource_ptr is different.

// The call to do_something can operate on incorrect values.

// For this situation std::once_flag and std::call_once were created

std::shared_ptr<some_resource> resource_ptr;
std::once_flag resouce_flag;
void init_resource() {
  resource_ptr.reset(new some_resource);
}
void well_managed_protected_initialization() {
  std::call_once(resouce_flag,init_resource);
  resource_ptr->do_something();
}

// Synchronization data is stored in the std::once_flag
// Every thread can use std::call_once, and it is assure that the pointer will have been initialized
















int main(){}
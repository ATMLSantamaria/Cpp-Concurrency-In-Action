// unique_lock provides more flexibility than lock_guard by relaxing the invariants
// a unique_lock does not always own the mutex that it is associated with

// if you pass std::adopt_lock as second argument -> the lock object manage the lock o a mutex

// if you pass std::defer_lock as second argument -> the mutex remain unlocked on construction.
// the lock can be then acquired later by calling lock() on the std::unique_lock e.g. u_lock.lock()
// or can be locked by being passed to std::lock() e.g. std::lock(u_lock());

#include <mutex>

class some_big_object{};
void swap(some_big_object& lhs, some_big_object & rhs) {};

class X {
private:
  some_big_object some_detail;
  std::mutex m;
public:
  X(some_big_object const & sd) : some_detail(sd) {}
  friend void swap(X& lhs, X& rhs) {
    if (&lhs == &rhs) {
      return;
    }
    std::unique_lock<std::mutex> lock_a(lhs.m,std::defer_lock); // create a lock but dont block the mutex
    std::unique_lock<std::mutex> lock_b(rhs.m,std::defer_lock); // create a lock but dont block the mutex

    // now call std::lock and lock both locks simultaneusly
    std::lock(lock_a,lock_b);

    //std::scoped_lock guard(lhs.m,rhs.m); this is a better alternative or C++17 or plus

    swap(lhs.some_detail,rhs.some_detail);
  }
};
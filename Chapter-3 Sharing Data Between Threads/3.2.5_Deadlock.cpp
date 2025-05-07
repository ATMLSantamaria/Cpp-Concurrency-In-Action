// Deadlock -> happen one lock block the unlock of other lock
#include <mutex>
class some_big_object {
  int data[100000];
};

// let consider a operation that exchange data betweeen instances of the same class.
// you must lock both instances to assure that the data is correctly exchanged.
// If the same order is always fixed it may end bad.

// E.g. if we lock also the first element of the swap(first, second), what happen if other thread call swap with reversed order swap(second,first)
// We will have a deadlock.

// For such situation we have a std::lock, a function tht lock 2 mutexes at once



void swap(some_big_object& lhs, some_big_object& rhs) {
  some_big_object tmp = rhs;
  rhs = lhs;
  lhs = tmp;
};

class X {
private:
  some_big_object some_detail;
  std::mutex m;
public:
  X(some_big_object const& sd) : some_detail(sd) {}

  friend void swap(X& lhs, X& rhs) {
    if (&lhs == &rhs) { // 1 - check that the instances are not the same
      return;
    }

    std::lock(lhs.m,rhs.m);  // this call locks both mutexes simultaneously.
                             // If it have locked the first and failed to lock the second 
                             // (locking launch exception) then it will unlock the first

    std::lock_guard<std::mutex> lock_a(lhs.m,std::adopt_lock); // adopt lock indicated to the lock to manage unlocking, but knowing that the mutex is already locked
    std::lock_guard<std::mutex> lock_b(lhs.m,std::adopt_lock); // 
    // The above 2 std::lock_guard are constructed. std::adopt_lock indicate to the lock_guard that the mutexes are already locked by the previous call to std::lock

    // Only after protecting for race conditions while avoiding deadlock, the swap function is called
    swap(lhs.some_detail,rhs.some_detail);

    // After it both lock_guard are released and the mutex unlocke
  }

};

// In C++ 17 we also have scoped_lock

class X {
  private:
    some_big_object some_detail;
    std::mutex m;
  public:
    X(some_big_object const& sd) : some_detail(sd) {}
  
    friend void swap(X& lhs, X& rhs) {
      if (&lhs == &rhs) { // 1 - check that the instances are not the same
        return;
      }

      std::scoped_lock guard(lhs.m,rhs.m); // This put together the for both mutexes, simplifying a lot the syntax
      //std::scoped_lock<std::mutex,std::mutex> guard(lhs.m,rhs.m); under the hood this happens. Simply the template arguments can be deduced so we can omit them

      swap(lhs.some_detail,rhs.some_detail);
  

    }
  
  };

  // OVERALL -< RULE FOR Avoiding Deadlocks. dont wait for another thread if there is a chance that it is waiting for you


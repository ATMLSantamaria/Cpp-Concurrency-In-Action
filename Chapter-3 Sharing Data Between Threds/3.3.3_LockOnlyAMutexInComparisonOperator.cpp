/*
  As we discussed in previous code snippets, 
  a lock should be held for only the minimum possible time to perform the required operations.

  In a previous example we lock 2 mutexes for a swap operation. Logical since you need concurrent access
  to both objects.

  But what if we just onw to compare a data member that is a plain int? maybe is better to copy than to lock?
  In the case of a int to copy it is very cheap. 

  Doing this we guarantee that you are holding the lock on a mutex during the minimum possible time
*/

#include <mutex>

class Y {

  int some_detail;
  mutable std::mutex m;

  int get_detail() const {
    std::lock_guard<std::mutex> lock_a(m); // lock to protext the access while we copy some_detail
    return some_detail;
  }
public:
  Y(int sd) : some_detail(sd) {}


  friend bool operator==(Y const& lhs, Y const& rhs) {
    if (&lhs == &rhs) {
      return true;
    }
    int const lhs_value = lhs.get_detail(); // we copy  the some_detail value in lhs
    int const rhs_value = rhs.get_detail(); // we copy  the some_detail value in rhs

    return lhs_value == rhs_value;
  }

};

/*
  IMPORTANT NOTE:
  The operator== could return true when when lhs and rhs have never been equal at any moment of time.abs

  It could happen that after rhs is copied lhs is modified, and only then copied. Then both values may be equal, but they were
  not equal at the same moment of time.abs
  
  In this case, our change to use only one lock has changed the semantics of the operator== and we have to be careful with this.
  
*/


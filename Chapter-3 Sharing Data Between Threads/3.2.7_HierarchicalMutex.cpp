#include <mutex>
#include <thread>
#include <stdexcept>
#include <vector>
#include <string>

// A mutex class accepted by std::lock  need 3 methods: lock, unlock, try_lock. In addition we can add the logic we consider
// Each mutex is assigned a number. You must always lock mutexes in order from higher to lower hierarchy values
// Each thread remembers the lowest hierarchy value of the mutex it currently holds.

class hierachical_mutex {

  std::mutex internal_mutex;

  unsigned long const hierarchy_value;
  unsigned long previous_hierarchy_value;

  // this value is accessible to all mutext instances but has a different value on each thread.
  static thread_local unsigned long this_thread_hierarchy_value; // thread local give each thread its own copy of the variable. So it is like a static var per thread.

  void check_for_hierarchy_violation() {
    if (this_thread_hierarchy_value <= hierarchy_value) {
      throw std::logic_error("mutex hierarchy violated");
    }
  }

  void update_hierarchy_value(){
    previous_hierarchy_value = this_thread_hierarchy_value;
    // store the previous value is important to restore it in unlock, otherwise you could not lock a mutex with higher hierarchy value again
    this_thread_hierarchy_value = hierarchy_value;
  }

public:
  explicit hierachical_mutex(unsigned long value) : hierarchy_value(value),previous_hierarchy_value(0) {}

  void lock() {
    check_for_hierarchy_violation();
    internal_mutex.lock();
    update_hierarchy_value();
  }

  void unlock() {

    if(this_thread_hierarchy_value != hierarchy_value) {
      throw std::logic_error("mutex hierarchy violated");
    }
    this_thread_hierarchy_value = previous_hierarchy_value;
    internal_mutex.unlock();
  }

  bool try_lock() {
    check_for_hierarchy_violation();
    if (!internal_mutex.try_lock()) {
      return false;
    }
    update_hierarchy_value();
    return true;
  }
};

#include <climits>
thread_local unsigned long hierachical_mutex::this_thread_hierarchy_value = ULONG_MAX; // necessary definition in one translation unit
// Initialized to the biggest value so that the first time the mutex can be locked


// By enforcing that mutexes are always locked in a strict order (by hierarchy value),
// it prevents circular wait conditions-one of the four Coffman conditions 
hierachical_mutex high_level_mutex(10000);
hierachical_mutex low_level_mutex(5000);

void thread_func() {
  std::lock_guard<hierachical_mutex> lg1(high_level_mutex);
  std::lock_guard<hierachical_mutex> lg2(low_level_mutex); // OK
  // std::lock_guard<hierachical_mutex> lg3(high_level_mutex); // Would throw!
}

// The next function shows the corrext way of locking
void correct_locking() {
  high_level_mutex.lock();      // Lock the higher-level mutex first
  // ... critical section for high-level resource

  low_level_mutex.lock();       // Now lock the lower-level mutex
  // ... critical section for low-level resource

  low_level_mutex.unlock();     // Unlock the lower-level mutex first
  high_level_mutex.unlock();    // Then unlock the higher-level mutex
}



// Example of a shared resource


template<typename T>
class Registry {
public:
  std::vector<T> list;

  T * find(const std::string & name) {
    for (auto & element : list) {
      if (element.name == name) {
        return & element;
      }
    }
    return nullptr;
  }
  void add(T && element) {
    list.emplace_back(element);
  }
};

class User {
public:
  std::string name;
  int balance;
};



Registry<User> registry; // a global registry of some kind

void update_user_balance(std::string user_name, int amount) {
  high_level_mutex.lock(); // Lock registry to find user
  User * user = registry.find(user_name);

  low_level_mutex.lock(); // Lock specific user account
  user->balance += amount;
  low_level_mutex.unlock();

  high_level_mutex.unlock();
}

int main() {

  User a;
  a.name = "alejandro";
  a.balance = 100;
  User b;
  b.name = "andromeda";
  b.balance = 10;

  registry.add(std::move(a));
  registry.add(std::move(b));


  std::thread thread_a(update_user_balance,std::string("alejandro"),100);
  std::thread thread_b(update_user_balance,std::string("andromeda"),8);

  thread_a.join();
  thread_b.join();
}
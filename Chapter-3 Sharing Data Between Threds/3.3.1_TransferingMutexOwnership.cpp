// Unique lock instances dont have to own their associated mutex -> so lock ownership can be moved.

// can be implicit or maybe std::move may be needed.

// Implicit if the source is an rvalue, but for a lvalue std::move is needed.

#include <mutex>

void prepare_data(){}

// this function locks a mutex, prepare some data and then return the lock to the caller
std::unique_lock<std::mutex> get_lock() {
  extern std::mutex some_mutex;
  std::unique_lock<std::mutex> lk(some_mutex);
  prepare_data();
  return lk;
}
void do_something(){}

void process_data() {
  std::unique_lock<std::mutex> lk(get_lock());
  do_something();

}
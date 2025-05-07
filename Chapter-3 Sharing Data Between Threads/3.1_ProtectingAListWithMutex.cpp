#include <list>
#include <mutex>
#include <algorithm>

// Global variable in the context of the example (that will "suffer" a possible race condition).
// Normally the mutexes will not be global, but part of a class. This is simly a small example
std::list<int> myList;
std::mutex myMutex;

void add_to_list(int new_value) {
  //std::lock_guard<std::mutex> guard(myMutex); // C++17 class template argument deduction. If we compile with C++17 we can avoid to specified the type mutex
  std::scoped_lock guard(myMutex); // The example in C++17, also using a improved lock, call scoped_lock
  myList.push_back(new_value);
}

bool list_contains(int value_to_find) {
  std::lock_guard<std::mutex> guard(myMutex);
  return std::find(myList.begin(),myList.end(),value_to_find) != myList.end();
}

// So we have a global variable myList that will be protected by a global instance of std::mutex
// The lock guard means that the access in these functions are mutually exclusive.
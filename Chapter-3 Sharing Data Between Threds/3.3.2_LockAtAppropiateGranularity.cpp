/*
  As a general rule lock the mutex only while accesing the shared data that needs to be protected.

  Try to do any processing outside of the lock.alignas
  
  And specially always avoid time consuming actitivities like file I/O. Unless what is protected is the file itself, never
  do file I/O in a lock.

  This is important to minimise the waiting time of a group of threads accessing a resource.alignas
  
  unique_lock is versitile for this purpose since we can unlock it befor ebeing destroyed

*/

#include <mutex>

// dumb classes and functions
class some_class{};
class result_type{};

some_class get_next_data_chunck(){}
result_type process(some_class target) {}
void write_result(some_class data_to_process, result_type result) {}

std::mutex the_mutex; // the mutex is defined somewhere


void get_and_process_data() {

  std::unique_lock<std::mutex> my_lock(the_mutex);    // the creation of unique_lock "locks" the mutex unless we use defer_lock as second argument
  some_class data_to_process = get_next_data_chunck();
  my_lock.unlock();                                   // unlock after accesssing and while we process data  
  result_type result = process(data_to_process); 
  my_lock.lock();                                     // lock again to write resykt
  write_result(data_to_process,result);
                                                      // end of the scope
}

// With this structure we protect the access of shared data, but we unlock while processing to allow other threads to access the information
// while this thread is not doing it.
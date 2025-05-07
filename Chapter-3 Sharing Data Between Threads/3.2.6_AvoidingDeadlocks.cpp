/*
  1 - Avoid Nested locks -> dont acquire a lock if you already hold one
*/

/*
  2 - Avoid calling user supplied code while holding a lock. We dont know what that code may do, and it could be holding a lock also
*/

/*
  3 - Acquire locks in a fixed orders. 
  
  If you absolutely must acquire two or more locks,
  and you can’t acquire them as a single operation with std::lock, the next best thing is to acquire them in the same order in every thread

*/

/*
  4 - Use Lock Hierarchy. Divide the app into laters and identify mutex that may e locked ine ach layer
*/

#include <mutex>

// User define mutex must implement lock, unlock and try_lock. here just a dumb implementation
class hierarchical_mutex {
  public:
  int level;
  hierarchical_mutex(int l) : level(l){}

  void lock() {} 

  void unlock() {}
  bool try_lock() {return true;}



};

hierarchical_mutex high_level_mutex(10000);

hierarchical_mutex low_level_mutex(5000);

hierarchical_mutex other_mutex(6000); 

int do_low_level_stuff();

int low_level_func()
{
  std::lock_guard<hierarchical_mutex> lk(low_level_mutex);    // lock the low level computations
  return do_low_level_stuff();
}


void high_level_stuff(int some_param);
void high_level_func()
{
    std::lock_guard<hierarchical_mutex> lk(high_level_mutex);   // put a lock on high level things
    high_level_stuff(low_level_func());                         // then call low level things, but that is already protected                 
}


void thread_a()                                                 
{
    high_level_func();           // this works fine
}

void do_other_stuff();
void other_stuff()
{
    high_level_func();                                          
    do_other_stuff();
}


void thread_b()      // this will fail at run time                                           
{
    std::lock_guard<hierarchical_mutex> lk(other_mutex);        
    other_stuff();
}
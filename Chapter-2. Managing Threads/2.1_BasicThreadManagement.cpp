#include <thread>

void do_some_work(int target){;}

int worspace1() {
// Start a thread always means to construct a std::thread object
std::thread my_thread{do_some_work, int(9)};

// Since std::thread is a class always be careful with  C++ MOST VEXING PARSE.
class SomeTask {
public:
  void operator()(){
    doSomething();
  }
  void doSomething(){};
};
// This will be problematic
//std::thread myThread(SomeTask()); // Will be interpreted as a function declaration of a function that takes a function pointer
// Remember always use {} for initialization
std::thread myThread{SomeTask{}};

// Once the thread has started you always need to explicitly decide to wait for it to finish (joining it)
// Or leave it to run on its own by detaching it
my_thread.join();
myThread.detach();
// Without it the std::thread destructor will call std::terminate()
}

// One main point with detached threads is to ensure that data accessed by the thread
// is still valid. This is trickier with threads than in single threaded code.

// Look the next example

struct Funct {
  int & i;
  Funct(int & i_) : i(i_) {}
  void operator()() {
    for (uint64_t j = 0; j < 1000000;++j) {
      this->doSomething(i);
    }
  }
  private:
  void doSomething(int & target) {target++;}
};

void oops() {
  int someLocalState = 0;
  Funct myFunct(someLocalState);
  std::thread myThread(myFunct);
  myThread.detach();
}

// The problem here is obvious. someLocalState is passed by reference to Funct constructor
// So myFunct is accessing the variable in the local stack of oops().
// The thread is detached doing something and oops will quickly finish, someLocalState
// will go out of scope and doSomething will try to access and in this case modify 
// a already destroyed variable
// In multithreading coode can be a little bit easier to make this mistake, than in ST programmin
// it is easily spoted.

// The normal way is to make the thread function self contained and COPY (or MOVE) any needed data.

// WAITING FOR THE THREAD
// Also we could use .join() and make oops wait to the thread to finish.
// However, waiting for the thread in oops would make absurd to use a different thread
// It would mean that is better to have all that logic directly in oops

// .join() is a brute force approach. Wait or not. A more controlled approach implies
// condition variables or futures. .join also cleans up storage associated with the thread

// .join needs to be call in the normal execution and in ANY EXCEPTION that may be thrown
// Example
void doSomethingInthisThread();
void betterOops() {
  int someLocalState = 0;
  Funct myFunct(someLocalState);
  std::thread t(myFunct);
  try {
    doSomethingInthisThread();
  } catch(...) {
    t.join(); // join if a exception need to be raised
    throw;
  }
  t.join(); //join in a normal state
}

// Problem. In Try/Catch is easy to get the scope wrong and mess something.
// It is better to apply RAII and create a class threadGuard that does the join()
// in its destructor
class ThreadGuard {

  std::thread & t;
  public:
  explicit ThreadGuard(std::thread & thread_) : t(thread_){}
  // Explicit prevents implicit conversions and copy-initialization.
  // You need to explicitly call the constructor in later code

  ~ThreadGuard() {
    // Check if the thread was not joined, and join it in that case
    if (t.joinable()) {
      t.join();
    }
  }
  // Disable  copy and assignment operators
  ThreadGuard(const ThreadGuard & other) = delete;
  ThreadGuard & operator=(const ThreadGuard & other) = delete;
};

// Now we can pass thread to this class and assure that if when the scope of muchBetterOops
// finish the thread has not been deallocated, the destructor of ThreadGuard will join it
void muchBetterOops() {
  int someLocalState = 0;
  Funct myFunct(someLocalState);
  std::thread t{myFunct};
  ThreadGuard aLocalThreadGuard(t);
  // more operations
}
// when this function goes out of scope aLocalThreadGuard is called, therefore the destructor
// o ThreadGuard is called and the thread is joined.
// Specifically, as is well known, the destruction of a local stack (or of a global) happens
// in reverse order, so aLocalThreadGuard is destroyed befoire than t. So t is joined, and
// std::thread destructor dont call std::terminate()
// Note: Order of destruction is logical thinking in the name, stack -> last in first out

// Note 2 : All this is not needed if we have called .detach instead of .join. detach breaks
// the connection between the thread and the std::thread object, and therefore the destruction
// of such object will not call std::terminate




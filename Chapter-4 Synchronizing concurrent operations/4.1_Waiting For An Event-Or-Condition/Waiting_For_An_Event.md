# 4.1 Waiting for an event or other condition

If we have a thread that is waiting for a second thread to completa a task we have the next options:

- Check flag in the shared data (protected by a mutex) and have the second thread to set the flag when it complete the task
  > Problem is that this is wasteful:
  > - The thread consume processing time repeatedly checking the flag.
  > - If the mutex is locked by the checking thread cannot be locked by any other thread.

- Put the thread to sleep for short periods with **std::this_thread::sleep_for:
  ```cpp
  bool flag;
  std::mutex m;
  void wait_for_flag() {
    std::unique_lock<std::mutex> lk(m);
    while(!flag) {
      lk.unlock(); // unlock
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //sleep. In between other thread may modify the flag
      lk.lock(); //lock
    }
  }
  ```
  > This is better than first option, but still problematic. Too short sleep and waste processing time. Too long
  > and the thread will be sleeping even when it could continue

- Third way. Use **condition variables** from C++ standard library
  > Basically one or more threads can wait for a condition or event to be satisfied. When a thread determine
  > that the condition is satisfied it  can then notify one or more of the waiting threads so that they
  > continue with the processing


## 4.1.1 Waiting for a condition with condition variables

The STD Library provides 2 implemenations in the header <condition_variable>.
Both woth with a a mutex to provide synchronization.

- **std:condition_variable**
  > Works only with **std::mutex**
- **std::condition_variable_any**
  > Works with any class that fill the criteria of being mutex-like.
  > This impose more costs in terms of size, performance and OS resources.
  > So, if we dont use an user-define mutex is better to use the basic condition variable.

```cpp
std::mutex mut;
std::queue<data_chunk> data_queue; // pass data between the threads
std::condition_variable data_cond; // condition variable

void data_preparation_thread() {
  while(more_data_to_prepare()) {
    data_chunk const data = prepare_data();
    {
      std::lock_guard<std::mutex> lk(mut); // When data is ready lock and push to the queue
      data_queue.push(data);
    }
    data_cond.notify_one(); // Notify any waiting thread AFTER unlocking the lock because scope ends
  }
}

void data_processing_thread() {
  while(true) {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk,[]{return !data_queue.empty();}); 
    // Pass to .wait the lock and the condition being waited
    // If the condition being waited is not true the .wait() method unlock the lock and put this thread
    // in a waiting or blocked state.
    // When a notify_one() in other thread is called, this thread is awaked and  check again the condition.
    // If now condition is true, computation continues, otherwise sleep this thread again and continue waiting.
    // Thats why unique_lock is needed, since .wait() needs to unlock several times
    data_chunk data = data_queue.front();
    data_queue.pop();
    lk.unlock()
    process(data);
    if (is_last_chunk(data)) {
      break;
    }
  }
}
```
So, they important member function of the condition variable here are **notify_one** and **wait()**
Any callable object can be used in wait. We could provide a function or a functor also.




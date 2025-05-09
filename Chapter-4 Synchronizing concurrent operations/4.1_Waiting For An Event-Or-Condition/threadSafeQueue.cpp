#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>


template<typename T>
class threadsafe_queue {
private:
  std::mutex mut;
  std::queue<T> data_queue;
  std::condition_variable data_cond;


public:
  threadsafe_queue() = default;
  threadsafe_queue(const threadsafe_queue&);
  threadsafe_queue(threadsafe_queue&&);

  threadsafe_queue & operator=(const threadsafe_queue &) = delete;

  void push(T new_value);
  bool try_pop(T & value);
  std::shared_ptr<T> try_pop();

  void wait_and_pop(T& value);
  std::shared_ptr<T> wait_and_pop();
};

// copy constructor
template<typename T>
threadsafe_queue<T>::threadsafe_queue(const threadsafe_queue & other) {
  std::lock_guard<std::mutex> lk(other.mutex); // lk other.mut
  // you dont need to lock this.mut since this object is being constructed and it is not accesible by any other thread
  data_queue = other.data_queue;
}

// move constructor. Imporant NOTE. NEVER ATTEMPT TO MOVE THE mutex or condition variable. It is forbidden by the standard
template<typename T>
threadsafe_queue<T>::threadsafe_queue(threadsafe_queue && other)
          : data_queue([&](){std::lock_guard<std::mutex> lk(other.mut); // [&] capture all local variables in the enclosing scope by reference. So you can use other
                             return std::move(other.data_queue);})  {}
                             // with the initialization list with lambda we assure that the object is build directly
                             // with the moved value

template<typename T>
void threadsafe_queue<T>::push(T new_value){
  std::lock_guard<std::mutex> lk(mut); // lock
  data_queue.push(new_value);          // push element
  data_cond.notify_one();              // notify a new element is there
}

template<typename T>
void threadsafe_queue<T>::wait_and_pop(T& value) {
  std::unique_lock<std::mutex> un_lk(mut);                          // need a unique lock to work with the wait
  data_cond.wait(un_lk,[this](){return !(this->data_queue.empty());}); // only awake if data_queue is not empty. Note you cannot capture member variables in lambdas, always capture this. later this could be omited
  value = data_queue.front();  // the caller of wait_and_pop(T& value) receive the element to be popped in value.
  data_queue.pop();            // do the actual pop
}






// e.g. usage
class data_chunk{};
bool more_data_to_prepare(){return true;} // dumb method
data_chunk prepare_data(){return data_chunk();}

threadsafe_queue<data_chunk> safe_queue;

void data_preparation_thread() {
  while(more_data_to_prepare()) {
    data_chunk const data = prepare_data();
    safe_queue.push(data);
  }
}
void process(data_chunk data){}
void data_processing_thread() {
  while (true) {
    data_chunk data;
    safe_queue.wait_and_pop(data);
    process(data);
  }
  
}

int main(){

}
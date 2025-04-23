#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <iostream>
#include <type_traits>

// We can decide the number of threads in run time in function of the number of cores in the system with std::thread::hardware_concurrency()
// With that we can create a truly parallel algorithm

template<typename Iterator, typename T>
struct accumulate_block {
  static_assert(std::is_default_constructible_v<T>, "T MUST be default constructible"); // Also T must be default constructible
  void operator()(Iterator first, Iterator last, T &result) {
    result = std::accumulate(first,last,result);
  }
};
// In a parallel algorithm you never want more threads than the hardware can support. 
// This is called oversubscription and will decrece performance, due to the context switching cost.
// This naive algorithm will work perfect with int. With doubles and floats where addition is not associative, the results can vary a bit from std::accumulate
// This is due to how blocks are splited.

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first, Iterator last, T & init) {
  static_assert(std::is_default_constructible_v<T>, "T MUST be default constructible"); // Also T must be default constructible
  unsigned long const length = std::distance(first,last); // distance between iterator classes
  if (!length) {
    return init;
  }
  unsigned long const min_per_thread = 25;
  unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
  unsigned long const hardware_threads = std::thread::hardware_concurrency(); // this should give a number but sometimes gives 0

  unsigned long const num_threads = std::min(hardware_threads != 0  ? hardware_threads : 2,max_threads); // protect against hardware_threads = 0 and chose the min between it and max number threads.
  unsigned long const block_size = length / num_threads;
  
  std::vector<T> results(num_threads,T{}); //initialise to default values
  std::vector<std::thread> threads(num_threads - 1);
  Iterator block_start = first;
  
  for (unsigned long i = 0; i < (num_threads -1);++i) {
    Iterator block_end = block_start;
    std::advance(block_end,block_size); // move iterato class forward, advance as the name says
    threads[i] = std::thread(accumulate_block<Iterator,T>(),
    block_start,block_end,std::ref(results[i]));
    block_start = block_end;
  }
  // last call?
  accumulate_block<Iterator,T>()(block_start,last,results[num_threads-1]);

  for (auto &entry :threads) {
    entry.join();
  }
  return std::accumulate(results.begin(),results.end(),init);

}

int main() {
  int result;
  std::vector<long int> target(1000);
  for (long int i = 0; i < target.size();++i) {
    target[i] = i;
  }

  long int normal_res = 0;
  normal_res = std::accumulate<std::vector<long int>::iterator,long int>(target.begin(),target.end(),normal_res);
  std::cout << normal_res << std::endl;

  long int init_value = 0;
  long int mySolution = parallel_accumulate<std::vector<long int>::iterator,long int>(target.begin(),target.end(),init_value);

  std::cout << "my parallel res = " << mySolution <<std::endl;

}
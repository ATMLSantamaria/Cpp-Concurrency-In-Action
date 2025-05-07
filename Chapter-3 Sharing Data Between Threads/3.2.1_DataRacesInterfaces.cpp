#include <deque>

// Interface to a stack class
template<typename T,typename  Container=std::deque<T> >
class stack {
public:
  //constructor
  // Copy constructor
  explicit stack(const Container&);
  
  // Move constructor
  explicit stack (Container&& = Container());

  // Allocator aware constructors
  template<class Alloc> 
  explicit stack(const Alloc&);

  template <class Alloc> 
  stack(const Container&,const Alloc&);

  template<class Alloc>
  stack(Container&&,const Alloc&);

  template<class Alloc>
  stack(stack&&, const Alloc&);

  // Operations
  bool empty() const;

  std::size_t size() const;

  T& top();

  T const & top() const; // for being able to check top in const instances

  void push(T const&);

  void push(T&&);

  void pop();

  void swap(stack&&);

  template<class... Args>
  void emplace(Args&&... args);

};

// Issue here is that in a stack before you pop() you check for empty() and to extract
// the value you use top()
// So, the usage is always
// 1 - check empty. This is done to acoid the overhead of catching an exception if stack empty
// 2 - extract value with top
// 3 - delete the value pop

// What if other thread pop between 1 and 2, or between 2 and 3. You have possible race conditions
// in both steps


// For the next use case if we protect the stack by a mutex internally, only one thread will run a stack
// member function at a time, but still we can have issues

// Thread A            Thread B
// value = s.top()     /////////
//////////////////     value = s.top() (same value than before)
// s.pop()             ////////////////
// do_something()      ////////////////
////////////////       s.pop() (the value popped here has not been even read and its deleted) 
////////////////       do_something()


void do_something(int){}
int main() {
  stack<int> s;
  if (s.empty()) {
    int const value = s.top();
    s.pop();
    do_something(value);
  }
}

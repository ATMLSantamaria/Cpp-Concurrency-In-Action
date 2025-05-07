// Here we will see some possible alternatives for our stack class
#include <vector>



// Option 1 - Pass in a reference to pop(), and receive the result in that variable.
template<typename T>
class ReferencePopStack {
public:
  void pop(T & result) {};
};
void passReference(){
  ReferencePopStack<std::vector<int>> some_stack;
  std::vector<int> result;

  some_stack.pop(result);
}
// Disadvantage: requires to construct a instance of stack value type before. Some possible issues
// 1 - This can be bad for some classes that are expensive to built.
// 2 - Some classes constructor require parameters that may not be available in this par of the code
// 3 - Require that stored type must be assignable. And many classes dont support assigment for different reasons


// Option 2 - Require a no-throw copy constructor or move constructor.  We can use type traits for this
#include <type_traits>
template<typename T>
class ProtectedStack {
  static_assert(std::is_nothrow_copy_constructible_v<T>);
  static_assert(std::is_nothrow_move_constructible_v<T>);
public:
void pop(){}
};
int protectedStack() {
  //ProtectedStack<std::vector<int>> willFailCopile; // vector copy constructor throw exception so this fails
}
// Disadvantage: Very limiting. Could be user defined class will have copy constructor that throw exception
// and that dont have a move constructor


// Option 3 - Reutrn a Pointer to popped item
template<typename T>
class PointerReturnStack {
public:
  T * pop(){return nullptr;}
};
// Pointers can be freely copied without fear of throwing an exception.
// We we need to manage the memory allcoated to the object, and for simple sytes that is a overhead
// If a interfave use this option, std::fhated_ptr is a great option

// Option 4 - Provide option 1 + 2 / 3 giving some flexibility
// Lets see the interface. We will use 2 overloads of pop, one for reference and one for pointer
#include <memory>
#include <exception> // we want the original class, to derive ours
struct  empty_stack : std::exception {
  const char * what() const noexcept;
};

template<typename T>
class threadsafe_stack {
public:
  threadsafe_stack();
  threadsafe_stack(const threadsafe_stack&);
  threadsafe_stack(threadsafe_stack&&);
  // now we delete the assignment operator of the stack
  threadsafe_stack & operator=(const threadsafe_stack&) = delete;
  void push(T new_value);
  std::shared_ptr<T> pop();
  void pop(T& value);
  bool empty() const;
};
// We restric operations in the whole stack. You cant assign it
// We dont have a swap
// The pop function will throw an empty_stack exception if the stack is empty

int main() {

}
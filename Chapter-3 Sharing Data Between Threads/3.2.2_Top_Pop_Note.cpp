/*
  In the previous example we saw the problems derived from the separation of top() and pop() in an stack
  So, why not to mix them.
  The reason is that we need to know if top() succeded before popping
*/

#include <stack>
#include <vector>

// Stack of vector. Vector is dinamically allocated. Lets imagine that the system is heavily loaded,
// or that there are resource constraints
// For this example we wont check for emptiness since the focus is in top / pop
int someFunction() {
  std::stack<std::vector<int>> s;
  std::vector<int> a{1,23};
  std::vector<int> b{1,44,66};
  s.push(a);
  s.push(b);

  // We try to copy the first element of the stack, to later operate with it
  std::vector<int> copied = s.top();
  // BUT. If the system is  heavily loaded, the dynamic allocation, and therefore the copy would have FAILED
  s.pop();
  
  // The copy constructor miight throw a std::bad_alloc
  // With top and pop separated we can split the execution and check

  // For example this:
  std::vector<int> copied_safe;
  try{
    copied_safe = s.top();
    s.pop();
  } catch (std::bad_alloc) {
    // Log or do whatever you want
    return -1;
  }
}

// The previous security structure is impossible without separating pop and top.
// If you cant safely copy the data it stays on the stack. You can free some memory and try again.

// Issue is that, as we saw in previous file, this split created difficulties with race conditions.
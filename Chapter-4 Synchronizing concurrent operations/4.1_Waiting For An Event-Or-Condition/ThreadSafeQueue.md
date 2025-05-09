# Analyse Thread Safe Queue requirements

## First. Generic Operations required for a Queue
First, lets see the required interface, we can take a look at the C++ std library, and what operation do they implement
```cpp
template<class T,class Container = std::deque<T> > //by default will be a std::deque
class queue {
public:
  explicit queue(const Container&);
  explicit queue(Container&& = Container());

  template <class Alloc> explicit queue(const Alloc&); //the allocator type, normally std::allocator for std classes
  template <class Alloc> queue(const Container&, const Alloc&);
  template <class Alloc> queue(Container &&, const Alloc&);
  template <class Alloc> queue(queue&&, const Alloc&);

  void swap(queue& q);
  bool empty() const;
  size_t type size() const;

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  void push(const T&& X);
  void pop();

  template<class.. Args> void emplace(Args&&.. args);
}

```

After constructor, assignment and swap we have 3 types of operations:
- Query state of **whole**: empty() and size()
- Query state of **elements of the queue**: front() and back()
- Modify the queue: push(), pop(), emplace()

As in the stack we analysed previously, there can be data races in front() and pop() so we need to combine them.

Also, sometimes the receiving thread needs to wait for the data, so we provide 2 pops:
- try_pop(): try pop, return inmediately indicating if a failure happens
- wait_and_pop(): waits until there is a value to retrieve

A simplified interface will look like this:

```cpp
#include <memory>

template<typename T>
class threadsafe_queue {
public:
  threadsafe_queue();
  threadsafe_queue(const threadsafe_queue &);
  threadsafe_queue(threadsafe_queue &&);

  threadsafe_queue & operator=(const threadsafe_queue&) = delete; //disallow assignment
  void push (T new_value);

  bool try_pop(T& value); // this pop stores the poped element into referenced value, so that return is used for status
  std::shared_ptr<T> try_pop(); // this returns the retrieved value, point to null is fails

  void wait_and_pop(T& value);
  std::shared_ptr<T>wait_and_pop();

  bool empty() const;
}
```


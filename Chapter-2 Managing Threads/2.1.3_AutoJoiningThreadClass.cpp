// One proposal in C++17 was to create a thread class that would "auto" join when its handler goes out of scoper. 

// This has some caveats, but here we will check how it could be implemented

#include <thread>

class AutoJoiningThread {
  std::thread t;
public:
  AutoJoiningThread() noexcept = default;


  // This is a templated constructor that allows to pass a type and its arguments.
  // Uses perfect forwarding with std::forward to preserve the value category (lvalue/rvalue) of arguments, avoiding unnecessary copies and enabling move semantics.
  // As with std::thread you can pass any callable type (function, functor, lambda)
  template<typename Callable,typename ... Args>
  explicit AutoJoiningThread(Callable&& func,Args&& ... args) : t(std::forward<Callable>(func), std::forward<Args>(args)...) {}

  // The constructor from std::thread. Take ownership of an existing std::thread moving it. NOTE: You need to std::move the thread in the place where this is called too
  // Pass-by-value is a common pattern for move-only types, but pass-by-rvalue-ref with std::move is also valid if you prefer explicitness.
  // This constructor accepts both rvalues (temporaries or created with std::move) and lvalues (normal stuff)
  explicit AutoJoiningThread(std::thread other_thread) noexcept : t(std::move(other_thread)) {}

  // This is pass-by-rvalue-ref. 
  // It is innecesary since it is covered by pass by value already for types that are move only like thread, or like unique_ptr
  // Nota: For types that are not move only THRERE IS DIFFERENCE. Just if they are move only they need to be moved in the place where this is call, so the normal copy is equivalent to this.
   explicit AutoJoiningThread(std::thread &&other_thread) noexcept : t(std::move(other_thread)) {}

  // Move Constructor.
  AutoJoiningThread(AutoJoiningThread && other) noexcept : t(std::move(other.t)){}

  // Move Assignment operator
  AutoJoiningThread & operator=(AutoJoiningThread&& other) noexcept {
    // Before assign other to this, assure that the thread hold by this has been joined.
    if (joinable()) {
      join();
    }
    t = std::move(other.t);
    return *this;
  }

  // Assignment ooperator
  AutoJoiningThread & operator=(std::thread other) {
    if (joinable()) {
      join();
    }
    t = std::move(other);
  }

  ~AutoJoiningThread() noexcept{
      if(joinable())
          join();
  }

  void swap(AutoJoiningThread& other) noexcept{
      t.swap(other.t);
  }

  bool joinable() const noexcept {
    return this->t.joinable();
  }

  void join() {
    this->t.join();
  }

  void detach() {
    this->t.detach();
  }

  std::thread& as_thread() noexcept{
      return t;
  }

  const std::thread& as_thread() const noexcept{
      return t;
  }

};
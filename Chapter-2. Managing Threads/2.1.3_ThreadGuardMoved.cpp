#include <thread>
#include <stdexcept>


class ScopedThreadGuard {
  std::thread t;
public:
  explicit ScopedThreadGuard(std::thread target) : t(std::move(target)) {
    if (!t.joinable()) {
      throw std::logic_error("no thread");
    }
  }
  ~ScopedThreadGuard() {
    t.join();
  }

  // Disable both copy and copy assignment operators
  ScopedThreadGuard(const ScopedThreadGuard &) = delete;
  ScopedThreadGuard & operator=(const ScopedThreadGuard&) = delete;
};
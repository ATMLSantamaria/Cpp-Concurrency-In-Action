#include <atomic>
#include <thread>
#include <cassert>
#include <iostream>

std::atomic<int> atomic_var{0};
int data = 0;

// Producer thread function
void producer() {
    data = 42; // Write shared data
    atomic_var.store(1, std::memory_order_release); // Release store
    atomic_var.fetch_add(1, std::memory_order_relaxed); // Continue release sequence
    atomic_var.store(2, std::memory_order_relaxed); // End release sequence
}

// Consumer thread function
void consumer() {
    // Wait until atomic_var is set by producer
    int val = 0;
    while ((val = atomic_var.load(std::memory_order_acquire)) < 1) {
        // Busy-wait (spin) until value is updated
    }
    // Now it's safe to read 'data'
    assert(data == 42);
    std::cout << "Consumer sees data = " << data << std::endl;
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    std::cout << "Both threads finished successfully.\n";
    return 0;
}

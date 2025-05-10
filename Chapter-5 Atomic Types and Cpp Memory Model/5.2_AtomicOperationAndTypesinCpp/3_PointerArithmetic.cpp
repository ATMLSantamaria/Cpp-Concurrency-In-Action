#include <atomic>
#include <iostream>
#include <cassert>
class Foo {
public:
  int a;
};

int main() {
  Foo arr[10];
  for (int i = 0; i < 10; ++i) {
    arr[i].a=i;
  }
  std::atomic<Foo*> x(arr);
  std::cout << x << std::endl;

  x += 3;

  std::cout << x << std::endl; // See that the address memory moves by 3
  Foo * check = x.load();
  std::cout << check->a << std::endl;

  Foo * check_fetch = x.fetch_add(3); // not should point to position 6th but returns position before
  std::cout << check_fetch->a << std::endl; // check fetch still point to third pos

  std::cout << x.load()->a << std::endl;


  check_fetch = x.fetch_sub(2);
  std::cout << check_fetch->a << std::endl; // now check fetch points to 6th
  std::cout << x.load()->a << std::endl; // x is in 4th

  
  Foo some_array[5];
  std::atomic<Foo *> p(some_array);
  Foo * xx = p.fetch_add(2);
  assert(xx == some_array);
  assert(p.load() == &some_array[2]);
  xx = (p-=1);
  assert(xx == &some_array[1]);
  assert(p.load() == &some_array[1]);

  p.fetch_add(3,std::memory_order_release);

}
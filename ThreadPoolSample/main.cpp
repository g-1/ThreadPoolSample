//
//  main.cpp
//  ThreadPoolSample
//


#include <iostream>
#include <mutex>
#include "ThreadPool.h"

static std::mutex mtx;

int main(int argc, const char * argv[])
{
  ThreadPool::get()->initialize(4);

  for(int i = 0; i < 100; ++i)
  {
    ThreadPool::get()->enqueue([=]{
      std::lock_guard<std::mutex> lock(mtx);
      std::cout << "test=" << i << std::endl;
    });
  }
  
  std::cout << "Hello, World!\n";
  return 0;
}

//
//  ThreadPool.hpp
//

#pragma once

#include <memory>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>


class ThreadPool
{
private:
  ThreadPool();
  
public:
  static ThreadPool* get();
  
  ~ThreadPool();
  
  void initialize(int num);
  void enqueue(std::function<void()> func);
  
private:
  static std::unique_ptr<ThreadPool> ms_instance;
  
  std::vector< std::thread > m_workers;
  std::queue< std::function<void()> > m_tasks;
  
  std::mutex m_mtx;
  std::condition_variable m_condition;
  bool m_stop;

};

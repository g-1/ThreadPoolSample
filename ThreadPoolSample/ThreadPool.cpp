//
//  ThreadPool.cpp

#include "ThreadPool.h"

std::unique_ptr<ThreadPool> ThreadPool::ms_instance;

ThreadPool* ThreadPool::get()
{
  if(!ms_instance)
  {
    ms_instance = std::unique_ptr<ThreadPool>(new ThreadPool);
  }
  
  return ms_instance.get();
}

ThreadPool::ThreadPool()
: m_workers()
, m_tasks()
, m_stop(false)
{
}

ThreadPool::~ThreadPool()
{
  {
    std::lock_guard<std::mutex> lock(m_mtx);
    m_stop = true;
  }
  
  m_condition.notify_all();
  
  for(auto &worker : m_workers)
  {
    worker.join();
  }
}

void ThreadPool::initialize(int num)
{
  m_workers.clear();
  m_workers.reserve(num);
  
  for(int i = 0; i < num; ++i)
  {
    m_workers.emplace_back([this]{
      for(;;)
      {
        std::function<void()> task;
        
        {
          std::unique_lock<std::mutex> lock(m_mtx);
          m_condition.wait(lock,[this]{
            if(m_stop)
            {
              return true;
            }
            
            return !m_tasks.empty();
          });
          
          if(m_tasks.empty())
          {
            return;
          }
          
          task = std::move(m_tasks.front());
          m_tasks.pop();
        }
        
        task();
      }
    });
  }
}

void ThreadPool::enqueue(std::function<void()> func)
{
  {
    std::lock_guard<std::mutex> lock(m_mtx);
    
    m_tasks.emplace(func);
  }
 
  m_condition.notify_one();
}

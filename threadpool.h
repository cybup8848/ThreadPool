#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include<thread>
#include<vector>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<future>
#include<functional>

class ThreadPool{
  public:
    ThreadPool(size_t threadNums=10);
    ~ThreadPool();

    template<typename F,typename ...Args>
    auto enqueue(F &&f,Args&& ...args)->std::future<typename std::result_of<F(Args...)>::type>;

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

ThreadPool::ThreadPool(size_t threadNums):stop(false){
  for(int i=0;i<threadNums;i++){
    this->workers.emplace_back([this](){
      for(;;){
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(this->queue_mutex);
          this->condition.wait(lock,[this]{return this->stop||!this->tasks.empty();});
          if(this->stop&&this->tasks.empty()){
            return;
          }
          task=std::move(tasks.front());
          this->tasks.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool(){
  {
    std::unique_lock<std::mutex> lk(this->queue_mutex);
    this->stop=true;
  }
  this->condition.notify_all();
  for(std::thread &t:workers){
    t.join();
  }
}

template<typename F,typename ...Args>
auto ThreadPool::enqueue(F &&f,Args&& ... args)->std::future<typename std::result_of<F(Args...)>::type>{
  using return_type=typename std::result_of<F(Args...)>::type;
  auto task=std::make_shared< std::packaged_task<return_type()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
  
  std::future<return_type> fu=task->get_future();
  { 
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    if(this->stop){
      throw std::runtime_error("enqueue the stoped queue");
    }
    this->tasks.emplace([task](){(*task)();});
  }
  this->condition.notify_one();
  return fu;
}

#endif




   









#include <chrono>
#include <iostream>

#include "threadpool.h"

int fun(int i){
  std::cout << "hello thread " << i << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "world " << i << std::endl;
  return i * i;
}

int main(int argc, char *argv[]) {
  ThreadPool pool(10);

  std::vector<std::future<int>> results;

  for (int i = 0; i < 20; i++) {
    results.emplace_back(pool.enqueue(fun,i));
  }


  for (auto &result : results) {
    std::cout << result.get() << "  ";
  }
  std::cout << std::endl;
  return 0;
}

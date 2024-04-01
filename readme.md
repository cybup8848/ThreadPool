ThreadPool
==========

A simple C++ Thread Pool implementation.

Basic usage:
```c++
// create thread pool with 4 worker threads
ThreadPool pool(10);

// enqueue and store future
auto result = pool.enqueue([](int answer) { return answer; }, 42);

// get result from future
std::cout << result.get() << std::endl;
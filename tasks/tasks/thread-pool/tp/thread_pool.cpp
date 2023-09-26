#include <cassert>
#include <tp/thread_pool.hpp>
#include <tp/helpers.hpp>

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/panic.hpp>

namespace tp {

static twist::ed::ThreadLocalPtr<ThreadPool> pool;

ThreadPool::ThreadPool(size_t workers_count)
    : workers_count_(workers_count) {
}

void ThreadPool::Start() {
  for (size_t i = 0; i < workers_count_; ++i) {
    workers_.emplace_back([this]() {
      pool = this;
      auto task = task_queue_.Take();
      while (task) {
        support::TryBlock(std::move(task));
        wg_.Done();
        task = task_queue_.Take();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  assert(workers_.empty());
}

void ThreadPool::Submit(Task task) {
  wg_.Add(1);
  task_queue_.Put(std::move(task));
}

ThreadPool* ThreadPool::Current() {
  return pool;
}

void ThreadPool::WaitIdle() {
  wg_.Wait();
}

void ThreadPool::Stop() {
  task_queue_.Close();
  JoinWorkers();
}

void ThreadPool::JoinWorkers() {
  for (auto& worker : workers_) {
    worker.join();
  }
  workers_.clear();
}

}  // namespace tp

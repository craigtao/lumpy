/*!
 * lumpy is a common c++1z library, mayby the missing library for you.
 * Copyright (C) 2016 lumpy.zhu@gmail.com
 *
 * This program is free software:
 *   you can redistribute it and/or modify it under GPLv3 (or any later version).
 *
 * This program is destributed in the hope that is will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You shold have received a copy of the GNU General Public License alone with the program.
 * If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include <lumpy/thread/common.h>

namespace lumpy
{

inline namespace thread
{

class ThreadPool
{
  public:
    explicit ThreadPool(size_t num_threads) {
        init(num_threads);
    }

    ~ThreadPool() {
        detach();
    }

    void detach() {
        for (auto& thread : threads_) {
            thread.detach();
        }
    }

    void run() {
        {
            LockGuard<> lock(mutex_);
            running_ = false;
            cond_.notify_all();
        }

        for (auto& thread : threads_) {
            thread.join();
        }

        threads_.clear();
    }

    template<class F, class... Args>
    auto commit(F&& f, Args&& ...args) {
        using   TResult = decltype(f(std::forward<Args>(args)...));
        auto    task    = std::make_shared<std::packaged_task<TResult()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        {
            LockGuard<> lock{ mutex_ };
            tasks_.emplace([task]() { (*task)(); });
        }
        cond_.notify_all();

        std::future<TResult> future = task->get_future();
        return future;
    }

  private:
    using Task = std::function<void()>;

    std::vector<Thread> threads_;
    std::queue<Task>    tasks_;

    Mutex               mutex_;
    CondVar             cond_;
    bool                running_ = true;

    void init(size_t num_threads) {
        for (size_t i = 0; i< num_threads; ++i) {
            threads_.emplace_back(&ThreadPool::schedual, this);
        }
    }

    Task getTask() {
        UniqueLock<> lock{ mutex_ };

        while(true) {
            cond_.wait(lock, [this]() { return !running_ || !tasks_.empty(); });
            if (!tasks_.empty())    break;
            else if (!running_)     break;
        }

        auto task(std::move(tasks_.front()));
        tasks_.pop();
        return task;
    }

    void schedual() {
        while (true) {
            auto task = getTask();
            if (task)   task();
            else        return;
        }
    }
};

}
}

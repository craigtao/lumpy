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
#include <lumpy/thread/task.h>
#include <lumpy/log.h>

namespace lumpy
{

inline namespace thread
{

/* --- ITask --- */
void ITask::run(TaskRunner* manager) noexcept {
    manager_ = manager;

    try {
        --manager_->semaphore_;
        run();
        ++manager_->semaphore_;
    }
    catch(IException& e) {
        log_error("unexcept: {}", cxxabi::demangle(typeid(e)));
        log_error("   {{ {} }}", e);
    }
    catch(...) {
        log_error("unknow exception");
    }
}

/* --- TaskRunner --- */
TaskRunner::TaskRunner(uint max_threads)
        : semaphore_(max_threads)
{}

TaskRunner::~TaskRunner() {
    for (auto ptask: tasks_) {
        delete ptask;
    }
}

void TaskRunner::run() {
    for(auto ptask: tasks_) {
        threads_.emplace_back([=] { ptask->run(this); });
    }
    for (auto& thread: threads_) {
        thread.join();
    }
}


void TaskRunner::addTask(ITask* ptask) {
    tasks_.push_back(ptask);
}

}

}

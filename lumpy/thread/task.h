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
#include <lumpy/thread/semaphore.h>

namespace lumpy
{

inline namespace thread
{
class EValueExists      {};
class EUsedbyExists     {};

class ITask;
class TaskRunner;

class ITaskData;
class ITaskParam;
template<class T> class TaskData;
template<class T> class TaskParam;

class ITaskData : INocopyable
{
  protected:
    friend class ITaskParam;
    ITaskData(): semaphore_(0) {}
    ~ITaskData() = default;
    ITaskData(ITaskData&&) = delete;

protected:
    Semaphore   semaphore_;
    void*       value_  = nullptr;
};

template<class Type>
class TaskData: public ITaskData
{
  public:
    TaskData()  = default;
    ~TaskData() = default;

    template<class T>
    void operator=(const T& value) {
        if (value_ != nullptr) {
            new(value_)Type(value);
            ++semaphore_;
        }
    }

    template<class T>
    void operator=(T&& value) {
        if (value_ != nullptr){
            new(value_)Type(std::move(value));
            ++semaphore_;
        }
        else {
            Type tmp(std::move(value));
        }
    }

  private:
    friend class TaskParam<Type>;
    void setValue(Type* value) {
        if (value_ != nullptr) throw EValueExists{};
        value_ = value;
    }

};

class ITaskParam : INocopyable
{
  protected:
    ITaskParam(ITaskData& depend): depend_(depend) {}

    ~ITaskParam() = default;
    ITaskParam(ITaskParam&&) = delete;

    void wait(Semaphore& runner) {
        if (depend_.semaphore_--) return;

        ++runner;
        --depend_.semaphore_;
        --runner;
    }

  private:
    ITaskData& depend_;
};


template<class Type>
class TaskParam : public ITaskParam
{
  public:
    explicit TaskParam(TaskData<Type>& depend): ITaskParam(depend){
        depend.setValue(reinterpret_cast<Type*>(buffer_));
    }

    template<class Task, class = If<isBase<ITask, Task> > >
    explicit TaskParam(Task& task): ITaskParam(task.result()) {
        task.result().setValue(reinterpret_cast<Type*>(buffer_));
    }

    ~TaskParam() {}

  private:
    friend class ITask;
    char buffer_[sizeof(Type)];

    Type operator()(Semaphore& semaphore) {
        wait(semaphore);
        auto ptr = reinterpret_cast<Type*>(buffer_);
        Type tmp(std::move(*ptr));
        ptr->~Type();
        return tmp;
    }
};


class TaskRunner : INocopyable
{
  public:
    explicit TaskRunner(uint max_threads);
    ~TaskRunner();
    void run();

    template<class Task, class... Args>
    Task& make(Args&& ...args) {
        auto ptask = new Task(std::forward<Args>(args)...);
        addTask(ptask);
        return *ptask;
    }

  private:
    friend class ITask;
    std::vector<Thread> threads_;
    std::vector<ITask*> tasks_;
    Semaphore           semaphore_;
    void addTask(ITask* ptask);
};

class ITask : INocopyable
{
  protected:
    template<class T> using Data = TaskData<T>;
    template<class T> using Param= TaskParam<T>;

    ITask()             = default;
    virtual ~ITask()    = default;

    template<class T>
    T load(TaskParam<T>& param) {
        return param(manager_->semaphore_);
    }

    template<class T, class U>
    void store(TaskData<T>& result, U&& value) {
        result = std::forward<U>(value);
    }

  private:
    friend class TaskRunner;
    TaskRunner* manager_  = nullptr;

    virtual void run() = 0;
    void run(TaskRunner* mamager) noexcept;
};

}

}

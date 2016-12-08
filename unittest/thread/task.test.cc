#include <lumpy/thread.h>
#include <lumpy/test.h>

namespace lumpy
{

inline namespace thread
{

class Number
{
  public:
    explicit Number(int rhs): value(rhs) {
        sleep(0.1);
    }

    Number(Number&& rhs) noexcept: value(rhs.value) {
        rhs.value = 0;
    }

    ~Number() {}

    int value;
};

static bool stats[16] ={ false };
static int  datas[16] ={ 0 };

void show(int n, int val) {
    static Mutex mutex;
    static Timer timer;
    LockGuard<> lock(mutex);

    printf("%5.2f taskflow ", timer.duration());
    for (auto i = 0; i < 8; ++i) {
        if (n==-1) val = datas[i];
        if (i==n||n==-1)    printf("|%s %-2d %s", stats[i]?"\033[42m":"\033[0m", val, "\033[0m");
        else                printf("|%s %-2c %s", stats[i]?"\033[42m":"\033[0m", ' ', "\033[0m");
    }
    printf("\n");
}

class TaskNum: public ITask
{
  public:
    TaskNum(int id, int val): id_(id), val_(val) {}

    void exec() {
        Number number(val_);
        result_ = std::move(number);
    }

    auto& result() { return result_; }

  private:
    void run() override {
        stats[id_] = true;
        show(id_, 0);
        exec();
        show(id_, val_);
        stats[id_] = false;
    }
    int     id_;
    int     val_;

    TaskData<Number>    result_;
};

class TaskSum: public ITask
{
  public:
    template<class ParamA, class ParamB>
    TaskSum(int id, ParamA& a, ParamB& b)
            : paramA_(a), paramB_(b), id_(id)
    {}

    void exec() {
        Number a = load(paramA_);   // yeild current thread, wait for paramA
        Number b = load(paramB_);   // yeild current thread, wait for paramB
        Number c(a.value+b.value);  // caculate: c= a+b
        show(id_, c.value);
        result_ = c;                // store c to result
    }

    auto& result() { return result_; }

  private:
    TaskParam<Number>   paramA_;
    TaskParam<Number>   paramB_;
    TaskData <Number>   result_;
    int                 id_;

    void run() override {
        stats[id_] = true;
        show(id_, 0);
        exec();
        stats[id_] = false;
    }

    Number load(TaskParam<Number>& number) {
        stats[id_] = false;
        Number result = ITask::load(number);
        stats[id_] = true;
        show(id_, result.value);
        return result;
    }
};

lumpy_unit(tasks) {
    lumpy_test(add) {
        {
            TaskRunner task_runner(4);
            auto& a = task_runner.make<TaskNum>(0, 1);
            auto& b = task_runner.make<TaskNum>(1, 2);
            auto& c = task_runner.make<TaskNum>(2, 3);
            auto& d = task_runner.make<TaskNum>(3, 4);

            auto& ab    = task_runner.make<TaskSum>(4, a, b);
            auto& cd    = task_runner.make<TaskSum>(5, c, d);
            auto& abcd  = task_runner.make<TaskSum>(6, ab, cd);
            (void)abcd;
            task_runner.run();
        }
    }
};

}

}

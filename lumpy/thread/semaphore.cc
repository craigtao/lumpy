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

#include <lumpy/thread/semaphore.h>

namespace lumpy
{

namespace external
{

#ifdef _LUMPY_OS_WINDOWS_
namespace windows
{
using sem_t = void;
static sem_t* sem_open(uint value) {
    sem_t* sem = CreateSemaphore(nullptr, value, 65536, nullptr);
    return sem;
}

static void sem_close(sem_t* sem) {
    CloseHandle(sem);
}

static void sem_post(sem_t* sem) {
    ReleaseSemaphore(sem, 1, nullptr);
}

static bool sem_wait(sem_t* sem, DWORD time=INFINITE) {
    auto stat = WaitForSingleObject(sem, INFINITE);
    if (stat == 0)          return true;

    const auto eid = errno;
    if (eid==WAIT_TIMEOUT)  return false;
    else                    throw ESystem(eid);
}

static bool sem_trywait(sem_t* sem) {
    return sem_wait(sem, 0);
}

}
#endif

#ifdef _LUMPY_OS_UNIX_
namespace unix
{
static sem_t* sem_open(uint value) {
    static int sem_index = 0;
    static int pid       = int(getpid());
    const char* fmt      = "lumpy.sem.%d:%d";
    char name[32];
    snprintf(name, sizeof(name), fmt, pid, sem_index++);

    auto sem = ::sem_open(name, O_CREAT, 0, value);
    if (sem == SEM_FAILED) {  throw ESystem(errno); }
    return sem;
}

static void sem_close(sem_t* sem) {
    auto stat = ::sem_close(sem);
    if (stat!=0) throw ESystem(errno);
}

static void sem_post(sem_t* sem) {
    auto stat = ::sem_post(sem);
    if (stat!=0) throw ESystem(errno);
}

static void sem_wait(sem_t* sem) {
    auto stat = ::sem_wait(sem);
    if (stat!=0) throw ESystem(errno);
}

static bool sem_trywait(sem_t* sem) {
    auto stat = ::sem_trywait(sem);
    if (stat==0)        return true;

    const auto eid = errno;
    if (eid==EAGAIN)    return false;
    else                throw ESystem(eid);
}

}
#endif
}


inline namespace thread
{

Semaphore::Semaphore(uint value)
        : sem_(external::sem_open(value))
{}

Semaphore::~Semaphore() {
    external::sem_close(sem_);
}

Semaphore& Semaphore::operator++() {
    external::sem_post(sem_);
    return *this;
}

Semaphore& Semaphore::operator--() {
    external::sem_wait(sem_);
    return *this;
}

bool Semaphore::operator--(int) {
    auto stat = external::sem_trywait(sem_);
    return stat;
}

}

}

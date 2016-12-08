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
#include <cuda_runtime.h>
#include <lumpy/cuda.h>

namespace lumpy
{

namespace cuda
{

namespace device
{

void sync() {
    const auto stat = cudaDeviceSynchronize();
    if (stat != cudaSuccess) throw ERuntime(cudaSuccess);
}

uint count() {
    auto count = 0;
    const auto stat = cudaGetDeviceCount(&count);

    if (stat != cudaSuccess) throw ERuntime(stat);
    return uint(count);
}

void current(uint id) {
    const auto stat = cudaSetDevice(int(id));
    if (stat != cudaSuccess) throw ERuntime(stat);
}

uint current() {
    int id;
    const auto stat = cudaGetDevice(&id);
    if (stat != cudaSuccess) throw ERuntime(stat);

    return uint(id);
}

cudaDeviceProp prop(uint id) {
    cudaDeviceProp p;
    const auto stat = cudaGetDeviceProperties(&p, int(id));
    if (stat != cudaSuccess) throw ERuntime(stat);

    return p;
}

const char* name(uint id) {
    const auto p = prop(id);
    thread_local char name[sizeof(p.name)];

    memcpy(name, p.name, sizeof(p.name));
    return name;
}

bool uva_support(uint id) {
    const auto p = prop(id);
    return p.unifiedAddressing == 1;
}

size_t totalmem(uint id) {
    const auto p = prop(id);
    return p.totalGlobalMem;
}

uint capability(uint id) {
    const auto p = prop(id);
    return p.major * 10 + p.minor;
}

}

}

}

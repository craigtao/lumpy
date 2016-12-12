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

namespace lumpy
{

namespace  external
{

#ifdef linux
#   undef linux
#endif

#ifdef unix
#   undef unix
#endif

namespace windows   {}
namespace linux     {}
namespace macos     {}
namespace unix      {}

namespace msvc      {}
namespace gcc       {}
namespace clang     {}
namespace cuda      {}

#if   defined(_WIN32)
#define _LUMPY_OS_WINDOWS_
using namespace windows;
#elif defined(__linux__) || defined(__linux) || defined(__CYGWIN__)
#define _LUMPY_OS_LINUX_
#define _LUMPY_OS_UNIX_
using namespace linux;
using namespace unix;
#elif defined(__APPLE__)
#define _LUMPY_OS_MACOS_
#define _LUMPY_OS_UNIX_
using namespace macos;
using namespace unix;
#endif

#if   defined(_MSC_VER)
#define _LUMPY_CC_MSVC_
#pragma warning(disable: 4566)
using namespace msvc;
#elif defined(__GNUC__)
#define _LUMPY_CC_GCC_
using namespace gcc;
#elif defined(__clang__)
#define _LUMPY_CC_CLANG_
using namespace clang;

#endif

#if defined(__CUDACC__)
#define _LUMPY_CC_CUDA_
using namespace cuda;
#endif

}

}

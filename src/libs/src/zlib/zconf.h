// SL: 2019-09-04 modified to allow for multiple build targets from a same source folder
#pragma once

#if defined(WIN32) || defined(WINDOWS)

#include "zconf_windows.h"

#else

#if defined(EMSCRIPTEN)

#include "zconf_emscripten.h"

#else

#include "zconf_cmake.h" // use the configuration generated at cmake time ...

#endif

#endif

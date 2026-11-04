#pragma once

#if defined(__SWITCH__) || defined(LUNARNX_NATIVE_UI)

#include <chiaki/log.h>

namespace lunar::ps {

ChiakiLog makeChiakiDiagnosticLog(const char* name);

} // namespace lunar::ps

#endif

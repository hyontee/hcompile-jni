#pragma once

// Build-compatible fallback for projects that referenced the optional xorstr
// header but did not ship the implementation.  The macro intentionally keeps
// the original call-site API (xorstr("literal")) while returning the literal.
// This file is for build compatibility; it does not provide compile-time
// string obfuscation.
#ifndef xorstr
#define xorstr(s) (s)
#endif

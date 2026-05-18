#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  value.h — Runtime value type for CVM++
//
//  All CVM values are 64-bit integers.  Booleans are stored as 1 (true) / 0 (false).
//  Strings and floats are intentionally out-of-scope for this project.
// ─────────────────────────────────────────────────────────────────────────────
#include <cstdint>

using Value = int64_t;

// Helper predicates
inline bool isTruthy(Value v) { return v != 0; }

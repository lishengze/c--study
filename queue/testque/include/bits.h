#pragma once

#include <cstdint>
#include "macro.h"

namespace tech
{

static __always_inline uint32_t roundup_pow_of_two(uint32_t v)
{
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return ++v;
}

static __always_inline uint64_t roundup_pow_of_two(uint64_t v)
{
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return ++v;
}

}   // namespace tech
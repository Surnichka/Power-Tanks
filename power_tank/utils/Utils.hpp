#include <type_traits>

#pragma once

namespace utils
{
    template<
        typename T, //real type
        typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    >
    inline T map(T value, T istart, T istop, T ostart, T ostop )
    {
        return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
    }
}

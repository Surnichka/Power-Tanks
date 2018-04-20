#ifndef MOVE_ON_COPY_H
#define MOVE_ON_COPY_H

#include <utility>

namespace nonstd
{
template <class T>
class move_on_copy_t
{
public:
    move_on_copy_t(T&& value)
        : _value(std::move(value))
    {
    }
    move_on_copy_t(const move_on_copy_t& other)
        : _value(std::move(other._value))
    {
    }
    const T& get() const
    {
        return _value;
    }
    T& get()
    {
        return _value;
    }

private:
    mutable T _value;
    move_on_copy_t& operator=(const move_on_copy_t<T>& other) = delete;
    move_on_copy_t& operator=(const move_on_copy_t<T>&& other) = delete;
};

template<typename T>
move_on_copy_t<T> make_move_on_copy(T&& value)
{
    return move_on_copy_t<T>(std::forward<T>(value));
}
template<typename T>
move_on_copy_t<T> monc(T& value)
{
    return make_move_on_copy(std::move(value));
}
}
#endif

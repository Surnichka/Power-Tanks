#ifndef FUNCTION_TRAITS_HPP_INCLUDED
#define FUNCTION_TRAITS_HPP_INCLUDED

#include <functional>
#include <tuple>
#include <type_traits>

namespace nonstd
{
template <class T>
struct unwrap_refwrapper
{
    using type = T;
};

template <class T>
struct unwrap_refwrapper<std::reference_wrapper<T>>
{
    using type = T&;
};

template <class T>
using special_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;

template <typename Functor>
struct function_traits : public function_traits<decltype(&Functor::operator())>
{
};

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    using result_type = R;
    using return_type = result_type;
    typedef result_type function_type(Args...);
    static const std::size_t arity = sizeof...(Args);

    using arg_types = std::tuple<Args...>;
    using arg_types_decayed = std::tuple<special_decay_t<Args>...>;
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)>
{
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)> : public function_traits<R(Args...)>
{
    typedef C& owner_type;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const> : public function_traits<R(Args...)>
{
    typedef const C& owner_type;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) volatile> : public function_traits<R(Args...)>
{
    typedef volatile C& owner_type;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const volatile> : public function_traits<R(Args...)>
{
    typedef const volatile C& owner_type;
};

template <typename Functor>
struct function_traits<std::function<Functor>> : public function_traits<Functor>
{
};

template <typename T>
struct function_traits<T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<volatile T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const volatile T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<T&&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const T&&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<volatile T&&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const volatile T&&> : public function_traits<T>
{
};

/*
* This seems to be slightly better than the standard library version as of now.
* */

template <typename F>
using fn_result_of = typename function_traits<F>::result_type;

/////////////////////////////////////////////////////////////////////////////////////
// use it like e.g:
// param_types<F, 0>::type

template <typename F, size_t Index>
struct param_types
{
    using type = typename std::tuple_element<Index, typename function_traits<F>::arg_types>::type;
};

template <typename F, size_t Index>
struct param_types_decayed
{
    using type = typename std::tuple_element<Index, typename function_traits<F>::arg_types_decayed>::type;
};

template <typename T, size_t Index>
using param_types_t = typename param_types<T, Index>::type;

template <typename T, size_t Index>
using param_types_decayed_t = typename param_types_decayed<T, Index>::type;

template <typename F, typename Tuple, std::size_t... I>
typename function_traits<F>::result_type apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}
template <typename F, typename Tuple>
typename function_traits<F>::result_type apply(F&& f, Tuple&& t)
{
    using Indices = std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>;
    return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices());
}

namespace detail
{

template <int> // begin with 0 here!
struct placeholder_template
{
};

template <class C, class Ret, class... Args, size_t... Is>
auto bind_mem_variadic_impl(C* const object_ptr, Ret (C::*mem_func)(Args...), std::index_sequence<Is...>)
    -> decltype(std::bind(mem_func, object_ptr, placeholder_template<Is>{}...))
{
    return std::bind(mem_func, object_ptr, placeholder_template<Is>{}...);
}


}
template <class C, class Ret, class... Args>
auto bind_mem_variadic(C* const object_ptr, Ret (C::*mem_func)(Args...))
    -> decltype(detail::bind_mem_variadic_impl(object_ptr, mem_func, std::make_index_sequence< sizeof...(Args) >{}))
{
    return detail::bind_mem_variadic_impl(object_ptr, mem_func, std::make_index_sequence< sizeof...(Args) >{});
}

}

namespace std
{
template <int N>
struct is_placeholder<nonstd::detail::placeholder_template<N>> : integral_constant<int, N + 1> // the one is important
{
};
}

#endif // FUNCTION_TRAITS_HPP_INCLUDED

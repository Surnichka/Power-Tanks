#ifndef _NONSTD_TYPE_TRAITS_
#define _NONSTD_TYPE_TRAITS_

#include <type_traits>
#include <cstddef>
#include <tuple>

namespace nonstd
{
    namespace detail
    {
        template<int... Is>
        struct seq { };

        template<int N, int... Is>
        struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

        template<int... Is>
        struct gen_seq<0, Is...> : seq<Is...> { };

        template<typename T, typename F, int... Is>
        void for_each(T&& t, F f, seq<Is...>)
        {
            auto l = { (f(std::get<Is>(t)), 0)... };
            (void)l;
        }
    }

    template<typename... Ts, typename F>
    void for_each_in_tuple(std::tuple<Ts...>& t, F f)
    {
        detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
    }

    template<typename F>
    void for_each_in_tuple(std::tuple<>& t, F f)
    {
    }

    template<typename... Ts, typename F>
    void for_each_in_tuple(const std::tuple<Ts...>& t, F f)
    {
        detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
    }

    template<typename F>
    void for_each_in_tuple(const std::tuple<>& t, F f)
    {
    }

    static inline bool check_all_true() { return true; }

    template<typename... BoolArgs>
    static bool check_all_true(bool arg1, BoolArgs... args) { return arg1 & check_all_true(args...); }

    template <class T>
    struct is_const_reference : std::false_type {};
    template <class T>
    struct is_const_reference<const T&> : std::true_type {};
}

///////////////////////////////////////////////////////
#define __try_using_rtti__ 1
///////////////////////////////////////////////////////

#ifdef _MSC_VER
#ifndef __cpp_rtti
#define __cpp_rtti _CPPRTTI
#endif // !__cpp_rtti
#endif

#define __cpp_rtti_enabled__ __try_using_rtti__ && __cpp_rtti
#if __cpp_rtti_enabled__
#include <typeindex>
#endif

namespace rtti
{
    class type_index_t;
    namespace detail
    {
    template<typename T>
    const type_index_t& type_id_impl();
    }

    class type_index_t
    {
    #if __cpp_rtti_enabled__
        using construct_t = const std::type_info;
    #else
        using construct_t = const type_index_t&();
    #endif
        construct_t* _info = nullptr;
        type_index_t(construct_t* info) noexcept : _info{ info } {}

        template<typename T>
        friend const type_index_t& detail::type_id_impl();
    public:
        bool operator==(const type_index_t& o) const noexcept
        {
            return hash_code() == o.hash_code();
        }
        bool operator!=(const type_index_t& o) const noexcept
        {
            return hash_code() != o.hash_code();
        }
        bool operator<(const type_index_t& o) const noexcept
        {
            return hash_code() < o.hash_code();
        }
        bool operator>(const type_index_t& o) const noexcept
        {
            return hash_code() > o.hash_code();
        }

        std::size_t hash_code() const noexcept
        {
    #if __cpp_rtti_enabled__
            return std::type_index(*_info).hash_code();
    #else
            return reinterpret_cast<std::size_t>(_info);
    #endif
        }
    };

    namespace detail
    {
        template<typename T>
        const type_index_t& type_id_impl()
        {
        #if __cpp_rtti_enabled__
            static type_index_t id(&typeid(T));
        #else
            static type_index_t id(&type_id_impl<T>);
        #endif
            return id;
        }
    }

    template<typename T>
    const type_index_t& type_id()
    {
        //this is required to copy the behavior of typeid operator
        return detail::type_id_impl<typename std::remove_cv<T>::type>();
    }

}

#endif

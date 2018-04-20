#ifndef EGT_DEMANGLE_TYPES_H
#define EGT_DEMANGLE_TYPES_H

#include <typeinfo>
#include <string>
using namespace std::literals;
#ifdef _MSC_VER
namespace demangle
{
//! Demangles the type encoded in a string
/*! @internal */
inline std::string _demangle( const char* name )
{
    return name;
}

//! Gets the demangled name of a type
/*! @internal */
template<class T> inline
std::string TypeName()
{
    return _demangle(typeid(T).name());
}

template<class T> inline
std::string TypeName(const T&)
{
    return _demangle(typeid(T).name());
}
}
#else // clang or gcc
#include <cxxabi.h>
#include <cstdlib>

namespace demangle
{
//! Demangles the type encoded in a string
/*! @internal */
inline std::string _demangle(const char* mangledName)
{
    int status = 0;
    char *demangledName = nullptr;
    std::size_t len;

    demangledName = abi::__cxa_demangle(mangledName, 0, &len, &status);

    std::string retName(demangledName);
    free(demangledName);

    return retName;
}

//! Gets the demangled name of a type
/*! @internal */
template<class T> inline
std::string TypeName()
{
    return _demangle(typeid(T).name());
}

template<class T> inline
std::string TypeName(const T&)
{
    return _demangle(typeid(T).name());
}

}
#endif


#endif //EGT_DEMANGLE_TYPES_H

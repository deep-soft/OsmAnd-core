#ifndef _OSMAND_CORE_COMMON_H_
#define _OSMAND_CORE_COMMON_H_

#include <cassert>
#include <memory>
#include <iostream>
#include <algorithm>

#include <OsmAndCore/QtExtraDefinitions.h>
#include <OsmAndCore/QtExtensions.h>

#if defined(TEXT) && defined(_T)
#   define xT(x) _T(x)
#else
#   if defined(_UNICODE) || defined(UNICODE)
#       define xT(x) L##x
#   else
#       define xT(x) x
#   endif
#endif

#define REPEAT_UNTIL(exp) \
    for(;!(exp);)


#if defined(SWIG)
#   define SWIG_DECLARE_UPCAST_POINTER(thisClass, parentClass)                                                                      \
        static std::shared_ptr<thisClass> upcastFrom(const std::shared_ptr<parentClass>& input);
#elif defined(OSMAND_SWIG)
#   define SWIG_DECLARE_UPCAST_POINTER(thisClass, parentClass)                                                                      \
        static std::shared_ptr<thisClass> upcastFrom(const std::shared_ptr<parentClass>& input)                                     \
        {                                                                                                                           \
            return std::dynamic_pointer_cast< thisClass >(input);                                                                   \
        }                                                                                                                           \
        static std::shared_ptr<const thisClass> upcastFrom(const std::shared_ptr<const parentClass>& input)                         \
        {                                                                                                                           \
            return std::dynamic_pointer_cast< const thisClass >(input);                                                             \
        }
#else
#   define SWIG_DECLARE_UPCAST_POINTER(thisClass, parentClass)
#endif

namespace OsmAnd
{
    template<typename T>
    Q_DECL_CONSTEXPR const T& constOf(T& value)
    {
        return value;
    }

    template<typename T>
    const T& assignAndReturn(T& var, const T& value)
    {
        var = value;
        return value;
    }

    template <typename FROM, typename TO>
    struct static_caster
    {
        TO operator()(const FROM& input)
        {
            return static_cast<TO>(input);
        }
    };

    template<typename T_OUT, typename T_IN>
    T_OUT copyAs(const T_IN& input)
    {
        T_OUT copy;
        std::transform(
            input.begin(), input.end(),
            std::back_inserter(copy),
            static_caster<typename T_IN::value_type, typename T_OUT::value_type>());
        return copy;
    }

    template <typename T>
    static int sign(T value)
    {
        return (T(0) < value) - (value < T(0));
    }

    template <typename T>
    static unsigned int enumToBit(const T value)
    {
        assert(static_cast<unsigned int>(value) <= sizeof(unsigned int) * 8);
        return (1u << static_cast<unsigned int>(value));
    }

    template <typename C, typename T>
    static bool hasEnumBit(const C mask, const T value)
    {
        assert(static_cast<unsigned int>(value) <= sizeof(C) * 8);
        return (mask & enumToBit(value)) != 0;
    }
}

#endif // !defined(_OSMAND_CORE_COMMON_H_)

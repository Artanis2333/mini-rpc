#pragma once

namespace mrpc::reflection
{

template <typename T, typename A>
constexpr size_t OffsetOfImpl(T const* t, A T::* a)
{
    return (char const*)&(t->*a) - (char const*)t;
}

#define OffsetOf(Type, Attr) OffsetOfImpl((Type const*)nullptr, &Type::Attr)

};
